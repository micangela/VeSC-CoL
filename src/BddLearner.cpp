/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Dec  6 13:22:39 2017
* Description: 
****************************************/

#include <iostream>
#include <fstream>

#include "BddLearner.h"
#include "cuddObj.hh"

using namespace std;


void BddLearner::init(const int& nn, const int& kk, const int& ll, const int& bb){
    // Base class's init
    LearnerBase::init(nn, kk, ll, bb);
    
    // Reset state/status variables
    timestamps.resize(0);
    n_minterms.resize(0);
    n_nodes.resize(0);
    pos_sample_bdds.resize(0);
    neg_sample_bdds.resize(0);

    // Bdd settings
    mgr.AutodynDisable();
    vspace = mgr.bddOne();
}

void BddLearner::wrap_up(){
    // Base class's wrap up
    LearnerBase::wrap_up();
}

void BddLearner::create_init_version_space(){
    // Variable i is encoded with two BDD variables. 
    // (Xi0, Xi1) =
    //    (0, 0) : negative
    //    (0, 1) : positive
    //    (1, 0) : don't care
    //    (1, 1) : invalid
  
    // index of Xi0 is 2*vi + 0
    // index of Xi1 is 2*vi + 1
    
    // For the vi-th feature in ki-th term, the index i in Xi is n*ki + vi

    vspace = mgr.bddOne();
    for(int vi=k*n-1; vi>=0; --vi){
        BDD invalid = mgr.bddVar(2*vi) & mgr.bddVar(2*vi+1);
        vspace &= !invalid;
    }

    // Add length constriant
    add_length_constraint();
}

void BddLearner::add_length_constraint(){
    // Return if no length constarint is required.
    if(l==0)
        return;
    
    // Implement the length constraint. Similar to the sequencial counter method
    
    // si[j] represents wire S(i,j) in sequencial counter method
    // dynamic programming is used to reduce one dimension
    vector<BDD> si(l+2); 
    si[0] = mgr.bddOne();
    for(int j=1; j<=l+1; ++j){
        si[j] = mgr.bddZero();
    }
    
    // Length constraints
    for(int vi=k*n-1; vi>=0; --vi){
        for(int j=l+1; j>=1; --j){
            si[j] |= (si[j-1] & (!mgr.bddVar(2*vi)));
        }
    }
    vspace &= si[l] & (!si[l+1]);
}

void BddLearner::add_lexi_order_constraint(){
    // The most efficient variable ordering for the lexicographical constraint is different 
    // to the most efficient variable oerdering for positive samples and negative samples,
    // hence we re-order the variable ordering before adding the lexicographical constraints.
    
    // Create permutation and inverse permutation
    vector<int> permute(2*k*n);
    vector<int> inv_permute(2*k*n);
    for(int ki=0; ki<k; ++ki){
        for(int vi=0; vi<n; ++vi){
            permute[2*ki*n+2*vi] = 2*(k*vi+ki);
            permute[2*ki*n+2*vi+1] = 2*(k*vi+ki) + 1;
        }
    }
    for(int idx=0; idx<2*k*n; ++idx){
        inv_permute[permute[idx]] = idx;
    }
    
    // use the most efficient variable ordering for the lexicographical constraint 
    vspace = vspace.Permute(permute.data());

    for(int ki=1; ki<k; ++ki){
        add_lexi_order_constraint(ki-1, ki);
    }
    
    // back to the original ordering
    vspace = vspace.Permute(inv_permute.data());
}

void BddLearner::add_lexi_order_constraint(const int& k1, const int& k2){
    // Implement the lexicographical order constraint that term k1 < term k2
    int var_offset1 = 2*k1;
    int var_offset2 = 2*k2;
    BDD dd = mgr.bddZero();
    BDD eq_dd = mgr.bddOne();
    for(int vi=0; vi<n; ++vi){
        BDD cond1 = (!mgr.bddVar(var_offset1) & !mgr.bddVar(var_offset1 + 1)) &
                    (!mgr.bddVar(var_offset2) &  mgr.bddVar(var_offset2 + 1));
        BDD cond2 = (!mgr.bddVar(var_offset1) & !mgr.bddVar(var_offset1 + 1)) &
                    ( mgr.bddVar(var_offset2) & !mgr.bddVar(var_offset2 + 1));
        BDD cond3 = (!mgr.bddVar(var_offset1) &  mgr.bddVar(var_offset1 + 1)) &
                    ( mgr.bddVar(var_offset2) & !mgr.bddVar(var_offset2 + 1));
        dd |= (eq_dd & (cond1 | cond2 | cond3)); 
        eq_dd &= ((!mgr.bddVar(var_offset1) & !mgr.bddVar(var_offset1 + 1) & 
                   !mgr.bddVar(var_offset2) & !mgr.bddVar(var_offset2 + 1))  |
                  (!mgr.bddVar(var_offset1) &  mgr.bddVar(var_offset1 + 1) & 
                   !mgr.bddVar(var_offset2) &  mgr.bddVar(var_offset2 + 1))  |
                  ( mgr.bddVar(var_offset1) & !mgr.bddVar(var_offset1 + 1) & 
                    mgr.bddVar(var_offset2) & !mgr.bddVar(var_offset2 + 1))
                 );
        
        var_offset1 += 2*k;
        var_offset2 += 2*k;
    }

    vspace &= dd;
}

void BddLearner::process_positive_samples(const vector<Sample>& pos_samples){
    for(auto sample: pos_samples){
        BDD sample_bdd = mgr.bddZero();
         
        for(int ki=0; ki<k; ++ki){
            int var_offset = 2*n*ki;
            
            BDD term_bdd = mgr.bddOne();
            for(int vi=n-1; vi>=0; --vi){
                if(sample[vi]){
                    // sample vi[i] == 1 -> target must not have its negative literal
                    term_bdd &= (mgr.bddVar(var_offset + 2*vi) | mgr.bddVar(var_offset + 2*vi+1)); 
                }
                else{
                    // sample vi[i] == 0 -> target must not have its positive literal
                    term_bdd &= (mgr.bddVar(var_offset + 2*vi) | (!mgr.bddVar(var_offset + 2*vi+1))); 
                }
            }
            sample_bdd |= term_bdd;
        }
        
        pos_sample_bdds.emplace_back(sample_bdd);
    }
}

void BddLearner::process_negative_samples(const vector<Sample>& neg_samples){
    for(auto sample: neg_samples){
        BDD sample_bdd = mgr.bddOne();
        
        for(int ki=0; ki<k; ++ki){
            int var_offset = 2*n*ki;
            
            BDD term_bdd = mgr.bddZero();
            for(int vi=n-1; vi>=0; --vi){
                if(sample[vi]){
                    // sample vi[i] == 1 -> target may have its negative literal
                    term_bdd |= ((!mgr.bddVar(var_offset + 2*vi)) & (!mgr.bddVar(var_offset + 2*vi+1))); 
                }
                else{
                    // sample vi[i] == 0 -> target may have its positive literal
                    term_bdd |= ((!mgr.bddVar(var_offset + 2*vi)) & mgr.bddVar(var_offset + 2*vi+1)); 
                }
            }
            sample_bdd &= term_bdd;
        }
        
        neg_sample_bdds.emplace_back(sample_bdd);
    }
}

void BddLearner::intersect_positive_dds(){
    for(size_t i=0; i<pos_sample_bdds.size(); ++i){
        vspace &= pos_sample_bdds[i];
        record_stat();
        //cout << i << " pos " << vspace.nodeCount() << " " << vspace.CountMinterm(2*k*n) << endl;
    }
}

void BddLearner::intersect_negative_dds(){
    for(size_t i=0; i<neg_sample_bdds.size(); ++i){
        vspace &= neg_sample_bdds[i];
        record_stat();
        //cout << i << " neg " << vspace.nodeCount() << " " << vspace.CountMinterm(2*k*n) << endl;
    }
}

void BddLearner::solve(){
    record_stat();
  
    if(k<=2){
        intersect_positive_dds();
        intersect_negative_dds();
    }
    else{
        intersect_negative_dds();
        intersect_positive_dds();
    }
    
    // Add lexicographical constraint
    //cout << "Before lexicographical, number of minterms = " << vspace.CountMinterm(2*k*n) << endl;
    add_lexi_order_constraint();
    //cout << "After lexicographical, number of minterms = " << vspace.CountMinterm(2*k*n) << endl;
    //vspace.PrintMinterm();
    
    // Check result
    retrieve_result();
}

void BddLearner::retrieve_result(){
    int hypo_size = (int) vspace.CountMinterm(2*k*n);
    if(hypo_size == 0){
        status = LS_NO_HYPOTHESIS;
    }
    else if(hypo_size <= B){
        status = LS_ONE_OR_FEW_HYPOTHESES;
        retrieve_hypotheses(hypo_size);
    }
    else{
        status = LS_MANY_HYPOTHESES;
        retrieve_hypotheses(B);
    }
}

void BddLearner::record_stat(){
    timestamps.push_back(get_cpu_time());
    n_minterms.push_back(vspace.CountMinterm(2*k*n));
    n_nodes.push_back(vspace.nodeCount());
}

KTermDnf BddLearner::retrieve_one_hypothesis(BDD& dd){
    // Retrieve and remove one hypothesis from dd,
    // then return the retrieved hypothesis
   
    // initialize the returned object
    KTermDnf dnf = KTermDnf::create(n, k);

    // provide required BDD variables, i.e. all the variables, for getting a minterm
    vector<BDD> all_bdd_vars;
    for(int i=0; i<2*k*n; ++i){
        all_bdd_vars.emplace_back(mgr.bddVar(i));
    }

    // select one minterm
    BDD minterm = dd.PickOneMinterm(all_bdd_vars);
    // remove the selected minterm from the input dd
    dd = dd & !minterm;

    // convert minterm BDD to an array
    char* buf = new char[2*k*n];
    minterm.PickOneCube(buf);
    
    // process result
    for(int ki=0; ki<k; ++ki){
        int var_offset = 2*n*ki;
        
        for(int vi=0; vi<n; ++vi){
            if(buf[var_offset+2*vi] == 1){
                dnf[ki][vi] = 2;
            }
            else{
                dnf[ki][vi] = (int) buf[var_offset+2*vi+1];
            }
        }
    }
    
    delete [] buf;
    return dnf;
}

void BddLearner::retrieve_hypotheses(const int& n_hypo){
    // Retrieve at most B hypotheses
    BDD dd = vspace;
    for(int i=0; i!=n_hypo; ++i){
        models.emplace_back(retrieve_one_hypothesis(dd));
    }
}

void BddLearner::write_statistics(string ofname){
    ofstream ofile(ofname);
    ofile << "fit exec time," << fit_exec_time << "\n";
   
    ofile << "timestamps,";
    for(auto x: timestamps) ofile << "," << x;
    ofile << "\n";

    ofile << "n_nodes,";
    for(auto x: n_nodes) ofile << x << ",";
    ofile << "\n";
    
    ofile << "n_minterms,";
    for(auto x: n_minterms) ofile << x << ",";
    ofile << "\n";

    ofile.close();
}
