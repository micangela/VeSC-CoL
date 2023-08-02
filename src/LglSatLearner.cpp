/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Mon Dec 11 16:41:50 2017
* Description: 
****************************************/

#include "time.h"
#include "LglSatLearner.h"

extern "C" {
    #include "lglib.h"
}

void LglSatLearner::init(const int& nn, const int& kk, const int& ll, const int& bb){
    // Base class's init
    SatLearner::init(nn, kk, ll, bb);
    
    // Initialize SAT solver
    sat_mgr = lglinit();
    //lglsetopt(sat_mgr, "seed", time(NULL));
    sat_status = -1;
}

void LglSatLearner::wrap_up(){
    // Release SAT solver
    lglrelease(sat_mgr);
    
    // Base class's wrap up
    SatLearner::wrap_up();
}

bool LglSatLearner::is_sat(){
    if(sat_status == LGL_SATISFIABLE) return true;
    else if(sat_status == LGL_UNSATISFIABLE) return false;
    else{
        cout << "Unexpected LGL result status: " << sat_status << ".\n";
        cout << "Abort.\n";
        exit(1);
        return 0;
    }
}


void LglSatLearner::first_solve(){
    // Add clauses to lgl
    for(auto cl: clauses){
        for(auto v: cl){
            lgladd(sat_mgr, v);
        }
        lgladd(sat_mgr, 0);
    }
    
    // For increamental solving
    for(int ki=0; ki<k; ++ki){
        for(int vi=0; vi<n; ++vi){
            lglfreeze(sat_mgr, 3*ki*n + 3*vi + NEG);
            lglfreeze(sat_mgr, 3*ki*n + 3*vi + POS);
            lglfreeze(sat_mgr, 3*ki*n + 3*vi + DCARE);
        }
    }

    // 1st solve
    timestamps.push_back(get_cpu_time());
    sat_status = lglsat(sat_mgr);
    timestamps.push_back(get_cpu_time());
}

void LglSatLearner::subsequent_solve(){
    // Add a clause to block the first solution
    for(auto v: *(clauses.rbegin())){
        lgladd(sat_mgr, v);
    }
    lgladd(sat_mgr, 0);
    
    // subsequent solve
    sat_status = lglsat(sat_mgr);
    timestamps.push_back(get_cpu_time());
}

void LglSatLearner::retrieve_satisfiable_assignment(){
    KTermDnf dnf = KTermDnf::create(n,k);

    for(int ki=0; ki<k; ++ki){
        for(int vi=0; vi<n; ++vi){
            if(lglderef(sat_mgr, 3*ki*n + 3*vi + NEG) > 0){
                dnf[ki][vi] = 0;
            }
            else if(lglderef(sat_mgr, 3*ki*n + 3*vi + POS) > 0){
                dnf[ki][vi] = 1;
            }
            else{
                dnf[ki][vi] = 2;
            }
        }
    }

    models.emplace_back(dnf);
}
