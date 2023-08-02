
all: cudd googletest lingeling vesc_col

lingeling:
	cd lingeling-bbc-9230380-160707; ./configure.sh
	make -C lingeling-bbc-9230380-160707 liblgl.a

BUILD_CUDD = 0
ifeq ("$(wildcard ./cudd-3.0.0/cudd/.libs/libcudd.a)","")
BUILD_CUDD = 1
endif
ifeq ("$(wildcard ./cudd-3.0.0/cplusplus/.libs/libobj.a)","")
BUILD_CUDD = 1
endif

cudd:
ifeq ($(BUILD_CUDD),1)
	cd cudd-3.0.0; touch aclocal.m4 configure Makefile.in Makefile.am; ./configure
	make -C cudd-3.0.0
endif
	

googletest:
	cd googletest-release-1.8.0; cmake -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF .
	make -C googletest-release-1.8.0

vesc_col:
	make -C src all

check:
	make -C test all
