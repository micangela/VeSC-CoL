CUDD_DIR = ../cudd-3.0.0
ZCHAFF_DIR = ../zchaff64
LGL_DIR = ../lingeling-bbc-9230380-160707


# Compiler options
CXX = g++
CXXFLAGS += -Wall -O3 -std=c++11 -pthread

INCDIR = -I$(ZCHAFF_DIR) -I$(CUDD_DIR)/cplusplus -I$(CUDD_DIR)/cudd -I$(LGL_DIR)

# Flags passed to the C++ linker
CUDD_LIB_DIR1 = $(CUDD_DIR)/cplusplus/.libs
CUDD_LIB_DIR2 = $(CUDD_DIR)/cudd/.libs
LFLAGS = -L$(CUDD_LIB_DIR1) -L$(CUDD_LIB_DIR2) -L$(ZCHAFF_DIR) -L$(LGL_DIR) -L.

RANLIB = ranlib
AR = ar

#SRC_FILES = Lgl2SatLearner.cpp LglSatLearner.cpp ZchaffSatLearner.cpp BddLearner.cpp SatLearner.cpp  LearnerBase.cpp 
SRC_FILES = BddLearner.cpp  LearnerBase.cpp SatLearner.cpp LglSatLearner.cpp
SRC_OBJS = $(SRC_FILES:.cpp=.o)



all: libvesc.a vs_learner

clean :
	rm *.o *.a vs_learner

LearnerBase.o: LearnerBase.cpp LearnerBase.h
	$(CXX) $(CXXFLAGS) -c $<

SatLearner.o: SatLearner.cpp SatLearner.h LearnerBase.o
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

ZchaffSatLearner.o: ZchaffSatLearner.cpp ZchaffSatLearner.h SatLearner.o
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

LglSatLearner.o: LglSatLearner.cpp LglSatLearner.h SatLearner.o
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

Lgl2SatLearner.o: Lgl2SatLearner.cpp Lgl2SatLearner.h LglSatLearner.o
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

BddLearner.o: BddLearner.cpp BddLearner.h  LearnerBase.o
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

libvesc.a: $(SRC_OBJS)
	@rm -f $@
	$(AR) cr $@ $(SRC_OBJS)
	$(RANLIB) $@



vs_learner.o: vs_learner.cpp vesc_col.h
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $<

vs_learner: libvesc.a vs_learner.o
	$(CXX) $(CXXFLAGS) vs_learner.o -lpthread $(LFLAGS) -lvesc -lobj -lcudd -llgl -o $@
	cp $@ ../bin
	
