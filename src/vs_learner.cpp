/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Mar 14 14:57:01 2018
* Description: 
****************************************/

#include <vector>
#include <fstream>
#include "assert.h"
#include <iostream>

#include "vesc_col.h"

using namespace std;

void read_samples(string fname, int& n, vector<Sample>& pos_samples, vector<Sample>& neg_samples){
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


int main(int argc, char ** argv){
    LearnerBase *learner;
    int k, l, B;
    
    // Parse arguments
    if(argc != 5 && argc != 6){
        cerr << "Usage: " << argv[0] << " <data_name> (sat|bdd) k l [B=1]\n";
        return 1;
    }
    string ifname(argv[1]);
    string solver(argv[2]);
    k = atoi(argv[3]);
    l = atoi(argv[4]);
    B = (argc==6) ? atoi(argv[5]) : 1;

    
    // Instantiation
    if(solver == "bdd") learner = new BddLearner();
    else if(solver == "sat") learner = new LglSatLearner();
    else {
        cerr << "The second argument must be one of 'bdd' or 'sat'\n";
        return 1;
    }
    
    // Read data
    int n;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    read_samples(ifname, n, pos_samples, neg_samples);

    // fit
    learner->fit(n, k, l, pos_samples, neg_samples, B);

    // Show result status
    if(learner->status == LS_NO_HYPOTHESIS) cout << "No Hypothesis" << endl;
    else if(learner->status == LS_ONE_OR_FEW_HYPOTHESES) cout << "One or Few Hypotheses" << endl;
    else if(learner->status == LS_MANY_HYPOTHESES) cout << "Many Hypotheses" << endl;
    else cout << learner->status << endl;
    
    for(auto& model: learner->models){
        cout << model << endl;
    }
    cout << "It took " << learner->fit_exec_time  << " seconds." << endl; 

    delete learner;
    return 0;
}
