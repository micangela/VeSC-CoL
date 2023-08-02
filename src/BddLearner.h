/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Dec  6 13:17:04 2017
* Description: 
****************************************/


#ifndef __BDDLEARNER__H__
#define __BDDLEARNER__H__

#include "LearnerBase.h"
#include "cuddObj.hh"

using namespace std;


class BddLearner : public LearnerBase{
public:
    // time unit is second
    vector<double> timestamps;
    
    // BDD statistics
    vector<double> n_minterms; 
    vector<long long> n_nodes;

    // Destructor
    virtual ~BddLearner() {}
    virtual void write_statistics(string ofname);

protected:
    Cudd mgr;
    BDD vspace;
    vector<BDD> pos_sample_bdds;
    vector<BDD> neg_sample_bdds;
    
    virtual void init(const int& nn, const int& kk, const int& ll, const int& bb);
    virtual void wrap_up();
    virtual void create_init_version_space();
    virtual void add_length_constraint();
    virtual void add_lexi_order_constraint(const int& k1, const int& k2);
    virtual void add_lexi_order_constraint();
    virtual void process_positive_samples(const vector<Sample>& pos_samples);
    virtual void process_negative_samples(const vector<Sample>& neg_samples);
    virtual void intersect_positive_dds();
    virtual void intersect_negative_dds();
    virtual void solve();
    virtual void record_stat();
    virtual void retrieve_result();
    virtual KTermDnf retrieve_one_hypothesis(BDD& dd);
    virtual void retrieve_hypotheses(const int& n_hypo);
};


#endif // __BDDLEARNER__H__
