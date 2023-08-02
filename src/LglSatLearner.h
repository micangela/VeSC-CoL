/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Mon Dec 11 16:41:44 2017
* Description: 
****************************************/

#ifndef __LGLSATLEARNER__H__
#define __LGLSATLEARNER__H__

#include "SatLearner.h"

extern "C" {
    #include "lglib.h"
}

using namespace std;


class LglSatLearner : public SatLearner{
public:
    // destructor
    virtual ~LglSatLearner(){}
protected:
    LGL* sat_mgr;
    int sat_status;
    
    virtual void init(const int& nn, const int& kk, const int& ll, const int& bb);
    virtual void wrap_up();
   
    virtual bool is_sat();
    virtual void first_solve();
    virtual void retrieve_satisfiable_assignment();
    virtual void subsequent_solve();
};



#endif // __LGLSATLEARNER__H__
