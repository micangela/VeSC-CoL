#! /usr/bin/env python

'''
Created by:  Kuo-Kai Hsieh
Time:        Thu Mar 15 17:01:19 2018
Description: 
'''

import argparse
import os
import sys
import threading
import Queue
import subprocess
import time

# Path to the executable
VS_LEARNER = os.path.join(os.path.dirname(os.path.abspath(__file__)), "vs_learner")

# Variables for readability
RES_SAT = 0
RES_BDD = 1
RES_TIMEOUT = 2

def get_commandline_args(): 
    """Use argparse to parse command line arguments. Return parser.parse_args()"""
    # Create a parser
    desc = """
    VeSC-CoL (Version Space Cardinality Concept Learning) learns k-term DNF from given data.
    
    VeSC-CoL searches in hypothesis space in increased complexity and stops when a version 
    space is non-empty. Then, at most B hypotheses are returned as well as the cardinality of
    version space.
    
    Let (l, k) denotes an l-literal k-term DNF space. The search order is
      (1, 1) -> (2, 1) -> (2, 2) -> (3, 1) -> (3, 2) -> (3, 3) -> (4, 1) -> ...
    The range of k can be controlled by input arguments.

    Example Usages:
    (a) Learn a Monomial and the cardinality bound is set to 1
        $ python vesc_col -B 1 -k_min 1 -k_max 1 [filename]

    (a) Learn a Monomial or a 2-term DNF and the cardinality bound is set to 10
        $ python vesc_col -B 10 -k_min 1 -k_max 2 [filename]


    """
    parser = argparse.ArgumentParser(description=desc, formatter_class=argparse.RawTextHelpFormatter)
    
    # Add arguments
    parser.add_argument("filename",
                        help="Data file.")
    parser.add_argument("-B", type=int, default=1,
                        help="Cardinality bound. Default: 1.")
    parser.add_argument("-k_min", type=int, default=1,
                        help="The minimum number of terms. Default: 1.") 
    parser.add_argument("-k_max", type=int, default=1,
                        help="The maximum number of terms. Default: 1.") 
    parser.add_argument("-timeout", type=int, default=-1,
                        help="Timeout in seconds. Default: No timeout") 
    
    return parser.parse_args()


class StoppableSubprocess(threading.Thread):
    """Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition."""

    def __init__(self, msg_q, msg, cmd):
        super(StoppableSubprocess, self).__init__(target=self.call)
        self._stop_event = threading.Event()
        self.cmd = cmd
        self.msg_q = msg_q
        self.msg = msg
        self.stdout = None

    def stop(self):
        self._stop_event.set()

    def stopped(self):
        return self._stop_event.is_set()

    def call(self):
        p = subprocess.Popen(self.cmd, stdout=subprocess.PIPE)
       
        # Wait for the stop event or subprocess completion
        while True:
            if self.stopped() or p.poll() is not None:
                break

        if self.stopped():
            # Caused by stop event, then terminate the subprocess
            try:
                p.terminate()
                p.kill()
            except OSError:
                pass
        else:
            # Caused by subprocess completion, then get result from stdout
            self.stdout, _ = p.communicate() 
            # Indicate this process is ended
            self.msg_q.put(self.msg)
        

def check_executable():
    """ Return True iff the executable exists. """
    if not os.path.isfile(VS_LEARNER):
        print VS_LEARNER, "does not exist."
        sys.exit(1)


def hypo_order(k_min, k_max):
    """ A generator for increased complexity hypothesis space (l, k), 
        where k_min <= k <= k_max.
    """
    l = k_min
    k = k_min

    while True:
        yield (l, k)
        k += 1
        if k > k_max:
            l += 1
            k = k_min


def retrieve_result(stdout):
    """ Print version space learning result. Return Ture iff the cardinality of 
        version space is not 0. """
    lines = stdout.splitlines()
    print stdout

    stop = False
    if lines[0] == "Many Hypotheses" or  lines[0] == "One or Few Hypotheses":
        stop = True
    
    return stop

def version_space_learning(fname, k, l, B, timeout):
    """ Run BDD learning and SAT learning in parallel with timeout. 
        Return Ture iff the cardinality of version space is not 0.

        If timeout <= 0, there is no timeout mechanism.
    """
    msg_q = Queue.Queue() 
    
    # executable or timer commands
    bdd_cmd = [VS_LEARNER, fname, "bdd", str(k), str(l), str(B)] 
    sat_cmd = [VS_LEARNER, fname, "sat", str(k), str(l), str(B)] 
    timer_cmd = ["sleep", str(timeout)] 

    bdd_learner = StoppableSubprocess(msg_q, RES_BDD, bdd_cmd) 
    sat_learner = StoppableSubprocess(msg_q, RES_SAT, sat_cmd) 
    timer = StoppableSubprocess(msg_q, RES_TIMEOUT, timer_cmd) 
   
    # start threads
    bdd_learner.start()
    sat_learner.start()
    if timeout > 0:
        timer.start()

    # wait until any thread ends
    res = msg_q.get()
    
    # terminate all the threads
    bdd_learner.stop()
    sat_learner.stop()
    if timeout > 0:
        timer.stop()

    # wait until all the process ends
    bdd_learner.join()
    sat_learner.join()
    if timeout > 0:
        timer.join()

    # process results
    stop = False
    if res == RES_SAT:
        print "SAT completed."
        stop = retrieve_result(sat_learner.stdout)
    
    elif res == RES_BDD:
        print "BDD completed."
        stop = retrieve_result(bdd_learner.stdout)
    
    elif res == RES_TIMEOUT:
        print "Timeout."
        stop = True
    
    else:
        print "Error: Unexpected learning result '{}'.".format(res)
        sys.exit(1)
    
    return stop


def main(fname, B, k_min, k_max, timeout):
    # iterate hypothesis space increased complexity
    for l, k in hypo_order(k_min, k_max):
        # version space learning
        print "Searching space (l, k) = ({}, {})".format(l, k)
        stop = version_space_learning(fname, k, l, B, timeout)
        
        if stop:
            break

if __name__ == '__main__':
    # get arguments
    args = get_commandline_args()

    check_executable()
    main(args.filename, args.B, args.k_min, args.k_max, args.timeout)
