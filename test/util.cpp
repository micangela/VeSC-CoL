/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Dec  6 14:56:33 2017 
* Description: 
****************************************/

#include <vector>
#include <fstream>
#include "assert.h"
#include <iostream>

#include "LearnerBase.h"

using namespace std;

void read_samples(string fname, int& n, vector<Sample>& pos_samples, vector<Sample>& neg_samples, KTermDnf& target){
    ifstream ifs (fname, ios::in);
    if (!ifs) {
        cerr << "Can't open input file" << endl;
        exit(1);
    }

    int mp, mn;
    
    // get n, mp, mn
    ifs >> n >> mp >> mn;

    pos_samples.resize(mp);
    for(int i=0; i<mp; ++i) pos_samples[i].resize(n);
    neg_samples.resize(mn);
    for(int i=0; i<mn; ++i) neg_samples[i].resize(n);
    
    // check target type. it must be monomial
    string target_type;
    ifs >> target_type;
    
    int n_terms = 0;
    n_terms = stoi(target_type);
    ifs >> target_type;
    ifs >> target_type;
    assert (n_terms != 0);

    // get target
    target.resize(n_terms);
    for(int tidx=0; tidx<n_terms; ++tidx){
        target[tidx].resize(n);
        for(int i=0; i!=n; ++i){
            ifs >> target[tidx][i];
        }
    }
    
    // get positive samples
    for(int i=0; i<mp; ++i){
        for(int j=0; j<n; ++j){
            ifs >> pos_samples[i][j];
        }
    }

    // get negative samples
    for(int i=0; i<mn; ++i){
        for(int j=0; j<n; ++j){
            ifs >> neg_samples[i][j];
        }
    }
}

