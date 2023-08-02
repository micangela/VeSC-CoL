# VeSC-Col (Version Space Cardinality Concept Learning)

## Build the package
    
    Execute
        $ make all
    
    It builds CUDD library, Lingeling SAT solver library,
    google_test library and VeSC-CoL.

    To check build result, execute
        $ make check

    There are 98 tests and all of them should be passed.

## Use the package
    Python 2.7 is required to run this script, which runs SAT learner 
    and BDD learner in parallel, with timeout mechanism.

    To check its usage, execute 
        $ ./bin/vesc_col.py -h 

        usage: vesc_col.py [-h] [-B B] [-k_min K_MIN] [-k_max K_MAX]
                           [-timeout TIMEOUT]
                           filename
        
        positional arguments:
          filename          Data file.
        
        optional arguments:
          -h, --help        show this help message and exit
          -B B              Cardinality bound. Default: 1.
          -k_min K_MIN      The minimum number of terms. Default: 1.
          -k_max K_MAX      The maximum number of terms. Default: 1.
          -timeout TIMEOUT  Timeout in seconds. Default: No timeout
   

    The format for the input file is described below.

    The first line contains three integers n, mp and mn, seperated by space,
    denoting the number of features, the number positive samples and the number
    of negative samples respectively.

    Each line in the next mp lines describe a positive sample. Each line contains
    n integers in {0, 1}, saperated by sapce. The i-th integer denotes the value 
    of the i-th feature.
    
    Each line in the next mn lines describe a negative sample. Each line contains
    n integers in {0, 1}, saperated by sapce. The i-th integer denotes the value 
    of the i-th feature.

    A sample file is shown below.

        5 2 3         // 5 featuers, 2 positive samples, 3 negative samples
        0 0 1 1 0     // The first positive sample is [0, 0, 1, 1, 0]
        1 0 1 1 0     // The second positive sample is [1, 0, 1, 1, 0]
        0 1 0 0 0     // The first negative sample is [0, 1, 0, 0, 0]
        0 1 1 1 1     // The second negative sample is [0, 1, 1, 1, 1]
        1 0 1 0 1     // The third negative sample is [0, 1, 1, 1, 1]

    For learning 1-term DNF with cardinality bound 1, execute
        $ python vesc_col.py <input file> -k_min 1 -k_max 1 -B 1

    The last few line of outputs are
        Searching space (l, k) = (2, 1) // 2-literal 1-term DNF space
        SAT completed.                  // SAT solver completed
        Many Hypotheses                 // There are many hypotheses in its version space
        [[-2, -5]]                      // One hypothesis in version space is X2'X5'
        It took 0.000678 seconds.

    It setting the cardinality bound to 5, execute
        $ python vesc_col.py <input file> -k_min 1 -k_max 1 -B 5

    The result becomes
        Searching space (l, k) = (2, 1)
        SAT completed.
        One or Few Hypotheses           // Version space cardinality is no larger than the bound
        [[-2, -5]]                      // X2'X5'
        [[3, -5]]                       // X3X5'          these four terms agree with
        [[4, -5]]                       // X4X5'          the given samples.
        [[-2, 4]]                       // X2'X4
        It took 0.000835 seconds.

## Link the pacakge as a library
    
    In your C++ code, include the header file 
        #include "vesc_col.h"

    While linking, BDD, Lingeling and VeSC-Col libraris are required.
    Use the following link options:
       
        VESC_DIR = ${TOP}/src
        LGL_DIR = ${TOP}/lingeling-bbc-9230380-160707
        CUDD_DIR = ${TOP}/cudd-3.0.0
        CUDD_LIB_DIR1 = $(CUDD_DIR)/cplusplus/.libs
        CUDD_LIB_DIR2 = $(CUDD_DIR)/cudd/.libs
        
        LFLAGS = -L$(CUDD_LIB_DIR1) -L$(CUDD_LIB_DIR2) -L$(LGL_DIR) -L$(VESC_DIR)

        $ g++  [options] <object files> -o <output_name> ${LFLAGS} -lvesc -lobj -lcudd -llgl -lsat
