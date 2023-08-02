/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Mon Dec  4 10:21:31 2017
* Description: 
****************************************/


#ifndef __SATLEARNER__H__
#define __SATLEARNER__H__

#include "LearnerBase.h"

using namespace std;


typedef vector<int> Clause;

class SatLearner : public LearnerBase{
public:
    const int NEG = 1;
    const int POS = 2;
    const int DCARE = 3;
    
    // destructor
    virtual ~SatLearner(){}
    
    // time unit is second
    vector<double> timestamps;

    virtual void write_statistics(string ofname);

protected:
    vector<Clause> clauses;
    int n_sat_vars;
    
    virtual void init(const int& nn, const int& kk, const int& ll, const int& bb);
    //virtual void wrap_up();
    virtual void create_init_version_space();
    virtual void process_positive_samples(const vector<Sample>& pos_samples);
    virtual void process_negative_samples(const vector<Sample>& neg_samples);
    virtual void solve();

    virtual void create_length_constraint();
    virtual void add_lexi_order_constraint(const int& k1, const int& k2);
    virtual void add_lexi_order_constraint();
    virtual void block_last_sol();
    virtual void determine_learning_result();
    virtual bool is_sat() = 0;
    virtual void first_solve() = 0;
    virtual void retrieve_satisfiable_assignment() = 0;
    virtual void subsequent_solve() = 0;
};


#endif // __SATLEARNER__H__
