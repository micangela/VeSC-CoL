/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Nov 29 17:06:54 2017
* Description: 
****************************************/


#ifndef __LEARNERBASE__H__
#define __LEARNERBASE__H__

#include <vector>
#include <string>
#include <iostream>

using namespace std;

typedef vector<int> Sample;
//typedef vector<int> Monomial;
//typedef vector<Monomial> KTermDnf;

enum LearnerStatusT {
    LS_UNDETERMINED,
    LS_NO_HYPOTHESIS,
    LS_ONE_OR_FEW_HYPOTHESES,
    LS_MANY_HYPOTHESES
};


class Monomial : public vector<int>{
    friend ostream& operator<<(ostream& os, const Monomial& obj){
        os << "[";
        bool is_first_literal = 1;
        for(size_t i=0; i!=obj.size(); ++i){
            if(obj[i] != 0 && obj[i] != 1) continue;

            if(!is_first_literal) os << ", ";
            is_first_literal = 0;

            if(obj[i] == 0) os << "-" << (i+1);
            else os << (i+1);
            
        }
        os << "]";
        return os;
    }
};

class KTermDnf : public vector<Monomial>{
public:
    static KTermDnf create(int n, int k){  
        KTermDnf dnf;
        dnf.resize(k);
        for(auto& term: dnf){
            term.resize(n, -1);
        }
        return dnf;
    }
private:    
    friend ostream& operator<<(ostream& os, const KTermDnf& obj){
        os << "[";
        for(size_t i=0; i!=obj.size(); ++i){
            if(i!=0) os << ", ";
            os << obj[i];
        }
        os << "]";
        return os;
    }
};


class LearnerBase{
public:
    LearnerStatusT status;
    vector<KTermDnf> models; 

    // statistics
    // time unit is second
    double fit_start_time;
    double fit_end_time;
    double fit_exec_time;
   
    LearnerBase(){}
    virtual ~LearnerBase(){}
    void fit(const int& nn, const int& kk, const int& ll, const vector<Sample>& pos_samples, const vector<Sample>& neg_samples, int bb=1);

    virtual void write_statistics(string ofname) = 0;
protected:
    // number of features
    int n;
    
    // number of terms
    int k;
    
    // number of literals
    int l; 
   
    // maximum number of hypothesis to report
    int B;

    double get_cpu_time();
    virtual void init(const int& nn, const int& kk, const int& ll, const int& bb);
    //virtual void init_models(const int& n_models);
    virtual void wrap_up();
    virtual void create_init_version_space() = 0;
    virtual void process_positive_samples(const vector<Sample>& pos_samples) = 0;
    virtual void process_negative_samples(const vector<Sample>& neg_samples) = 0;
    virtual void solve() = 0;
};


#endif // __LEARNERBASE__H__
