/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Nov 29 17:19:38 2017
* Description: 
****************************************/

#include <sys/time.h>
#include <sys/resource.h>

#include "LearnerBase.h"

void LearnerBase::fit(const int& nn, const int& kk, const int& ll, const vector<Sample>& pos_samples, const vector<Sample>& neg_samples, int bb){
    // Initialize learner variables
    init(nn, kk, ll, bb);
   
    create_init_version_space();

    process_positive_samples(pos_samples);

    process_negative_samples(neg_samples);

    solve();

    // Collect statistics
    wrap_up();
}

void LearnerBase::init(const int& nn, const int& kk, const int& ll, const int& bb){
    // Record current time
    fit_start_time = get_cpu_time();
    fit_end_time = -1;
    fit_exec_time = -1;
    
    // Get learner variables
    n = nn;
    k = kk;
    l = ll;
    B = bb;

    // Reset status/state/result variables
    status = LS_UNDETERMINED;
    models.resize(0);
}

//void LearnerBase::init_models(const int& n_models){
//    // Create k-term DNFs
//
//    models.resize(n_models);
//    // initialize each k-term DNF
//    for(auto& model: models){
//        model.resize(k);
//        for(auto& term: model){
//            term.resize(n, -1);
//        }
//    }
//}

void LearnerBase::wrap_up(){
    fit_end_time = get_cpu_time();
    fit_exec_time = fit_end_time - fit_start_time;
}

double LearnerBase::get_cpu_time(){
    double res;
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    res = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
    res *= 1e-6;
    res += usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
    return res;
}
