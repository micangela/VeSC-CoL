/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Mon Dec  4 10:26:24 2017
* Description: 
****************************************/

#include <iostream>
#include <fstream>
#include <numeric>
#include <assert.h>

#include "SatLearner.h"

using namespace std;

void SatLearner::init(const int& nn, const int& kk, const int& ll, const int& bb){
    // Base class's init
    LearnerBase::init(nn, kk, ll, bb);
    
    // Reset state/status variables
    timestamps.resize(0);
    n_sat_vars = 0;
    clauses.resize(0);
}

void SatLearner::create_init_version_space(){
    // X_{3i+1} := is negative
    // X_{3i+2} := is positive
    // X_{3i+3} := is don't care
    
    // base SAT variables
    n_sat_vars += 3*k*n;
    
    // One-hot constraints
    for(int ki=0; ki<k; ++ki){
        for(int vi=0; vi<n; ++vi){
            int offset = ki*(3*n) + 3*vi;
            clauses.emplace_back( Clause({offset+NEG, offset+POS, offset+DCARE}));
            clauses.emplace_back( Clause({-(offset+NEG), -(offset+POS)}));
            clauses.emplace_back( Clause({-(offset+NEG), -(offset+DCARE)}));
            clauses.emplace_back( Clause({-(offset+POS), -(offset+DCARE)}));
        }
    }
    
    // length constraint
    create_length_constraint();

    // lexicographical order constraint
    add_lexi_order_constraint();
}

void SatLearner::create_length_constraint(){
    // Return if no length constarint is required.
    if(l==0)
        return;

    // The start of new variables
    int extra_var_offset = n_sat_vars;
    
    // anonymous function for encoding readability
    // X(i) is the i-th variable, index in {1, 2, 3, ..., n*k}
    auto X = [](int i) -> int { return -(3*i); };
    
    // S(i,j) is the wire denoting at least j variables appear after processing i variables 
    auto S = [extra_var_offset, this](int i, int j) -> int { return extra_var_offset+(i-1)*l+j;};
    
    // Special case
    if(n == 1 && k==1 && l == 1){
        clauses.emplace_back( Clause({X(1)}));
        return;
    }
    
    // Implement sequencial counter method
    
    
    // width of the sequence counter (i.e. the number of input variables)
    int w = n*k;  
    // Add extra variables 
    n_sat_vars += l * (w-1);
    
    // 1st column
    clauses.emplace_back( Clause({ -X(1),  S(1,1)}));
    clauses.emplace_back( Clause({  X(1), -S(1,1)}));
     
    for(int j=2; j<=l; ++j){
        clauses.emplace_back( Clause({-S(1,j)}));
    }

    // length must <= l
    for(int i=2; i<=w; ++i){
        clauses.emplace_back( Clause({ -X(i), -S(i-1,l)}));
    }
    
    // length must >= l
    clauses.emplace_back( Clause({ X(w),        S(w-1,l)}));
    clauses.emplace_back( Clause({ S(w-1, l-1), S(w-1,l)}));

    // other columns
    for(int i=2; i<w; ++i){
        // 1st row
        clauses.emplace_back( Clause({  X(i), S(i-1,1), -S(i,1)})); 
        clauses.emplace_back( Clause({ -X(i),            S(i,1)})); 
        clauses.emplace_back( Clause({       -S(i-1,1),  S(i,1)})); 
        
        // other rows
        for(int j=2; j<=l; ++j){
            clauses.emplace_back( Clause({  X(i),               S(i-1,j), -S(i,j)})); 
            clauses.emplace_back( Clause({         S(i-1,j-1),  S(i-1,j), -S(i,j)})); 
            clauses.emplace_back( Clause({                     -S(i-1,j),  S(i,j)})); 
            clauses.emplace_back( Clause({ -X(i), -S(i-1,j-1),             S(i,j)})); 
        }
    }
}


void SatLearner::add_lexi_order_constraint(){
    for(int ki=1; ki<k; ++ki){
        add_lexi_order_constraint(ki-1, ki);
    }
}

void SatLearner::add_lexi_order_constraint(const int& k1, const int& k2){
    // Implement the lexicographical order constraint that term k1 < term k2
   
    // Add extra variables
    int w = 3*k*n;
    int k1_offset = 3*k1*n;
    int k2_offset = 3*k2*n;
    int extra_var_offset_t = n_sat_vars;
    int extra_var_offset_e = extra_var_offset_t + w;
    n_sat_vars += 2*w;

    // anonymous function for encoding readability
    auto T = [extra_var_offset_t](const int& i) -> int { return extra_var_offset_t+i; };
    auto E = [extra_var_offset_e](const int& i) -> int { return extra_var_offset_e+i; };
    auto X = [k1_offset](const int& i) -> int { return k1_offset+i; }; 
    auto Y = [k2_offset](const int& i) -> int { return k2_offset+i; }; 

    // The lexicographical order must be true
    Clause cl(w);
    iota(cl.begin(), cl.end(), extra_var_offset_t+1);
    clauses.emplace_back(cl);

    // relation of t symbols
    for(int i=1; i<=w; ++i){
        clauses.emplace_back( Clause({ -T(i),  E(i)              })); 
        clauses.emplace_back( Clause({ -T(i)       ,  X(i)       })); 
        clauses.emplace_back( Clause({ -T(i)              , -Y(i)})); 
        clauses.emplace_back( Clause({  T(i), -E(i), -X(i),  Y(i)})); 
    }
    
    // relation of e symbols
    clauses.emplace_back( Clause({ E(1) })); 
    for(int i=2; i<=w; ++i){
        clauses.emplace_back( Clause({ -E(i),  E(i-1)                   })); 
        clauses.emplace_back( Clause({ -E(i)         ,  X(i-1), -Y(i-1) })); 
        clauses.emplace_back( Clause({ -E(i)         , -X(i-1),  Y(i-1) })); 
        clauses.emplace_back( Clause({  E(i), -E(i-1),  X(i-1),  Y(i-1) })); 
        clauses.emplace_back( Clause({  E(i), -E(i-1), -X(i-1), -Y(i-1) })); 
    }
}

void SatLearner::process_positive_samples(const vector<Sample>& pos_samples){
    for(auto s: pos_samples){
        // Add extra variables
        int extra_var_offset = n_sat_vars;
        n_sat_vars += k;
        
        // one of the monomial is true
        Clause cl(k);
        for(int ki=0; ki<k; ++ki){
            cl[ki] = extra_var_offset + ki + 1;
        }
        clauses.emplace_back(cl);

        // constarints for each monomial
        for(int ki=0; ki<k; ++ki){
            int base_var_offset = 3*ki*n;
            int z = extra_var_offset + ki + 1;
            
            Clause clki(n+1);
            for(int vi=0; vi<n; ++vi){
                if(s[vi] == 0){
                    // vi is not positive
                    int not_pos = -(base_var_offset+3*vi+POS);
                    clauses.emplace_back( Clause({ not_pos, -z })); 
                    clki[vi] = -not_pos;
                }
                else{
                    // vi is not negative
                    int not_neg = -(base_var_offset+3*vi+NEG);
                    clauses.emplace_back( Clause({ not_neg, -z })); 
                    clki[vi] = -not_neg;
                }
            }
            
            clki[n] = z;
            clauses.emplace_back(clki);
        }
         
    }
}

void SatLearner::process_negative_samples(const vector<Sample>& neg_samples){
    for(auto s: neg_samples){
        // constarints for each monomial
        for(int ki=0; ki<k; ++ki){
            int base_var_offset = 3*ki*n;
            
            Clause clki(n);
            for(int vi=0; vi<n; ++vi){
                if(s[vi] == 0){
                    // vi may be positive 
                    int pos = base_var_offset+3*vi+POS;
                    clki[vi] = pos;
                }
                else{
                    // vi may be negative
                    int neg = base_var_offset+3*vi+NEG;
                    clki[vi] = neg;
                }
            }
            clauses.emplace_back(clki);
        }
    }
}

void SatLearner::solve(){
    first_solve();
   
    while(is_sat() && static_cast<int>(models.size())<B){
        retrieve_satisfiable_assignment();
        block_last_sol();
        subsequent_solve();
    }

    determine_learning_result(); 
}


void SatLearner::block_last_sol(){
    Clause sol_clause(3*n*k);
    for(size_t i=0; i<sol_clause.size(); ++i){
        sol_clause[i] = i+1;
    }

    auto dnf = *(models.rbegin());
    for(int ki=0; ki<k; ++ki){
        for(int vi=0; vi<n; ++vi){
            switch(dnf[ki][vi]){
                case 0: sol_clause[3*ki*n+3*vi+NEG-1] *= -1; break;
                case 1: sol_clause[3*ki*n+3*vi+POS-1] *= -1; break;
                case 2: sol_clause[3*ki*n+3*vi+DCARE-1] *= -1; break;
                default: cout << "Unexpected result value. Abort." << endl; assert(0); break;
            }
        }
    }
    
    clauses.emplace_back(sol_clause);
}

void SatLearner::determine_learning_result(){
    if(models.size() == 0) status = LS_NO_HYPOTHESIS;
    else if(is_sat()) status = LS_MANY_HYPOTHESES;
    else status = LS_ONE_OR_FEW_HYPOTHESES;
}

void SatLearner::write_statistics(string ofname){
    ofstream ofile(ofname);
    ofile << "fit exec time," << fit_exec_time << "\n";
    ofile << "timestamps";
    for(auto x: timestamps) ofile << "," << x;
    ofile << "\n";
    ofile.close();
}
