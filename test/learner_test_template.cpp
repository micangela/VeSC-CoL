/****************************************
* Created by:  Kuo-Kai Hsieh
* Time:        Wed Nov 29 15:06:08 2017
* Description: 
****************************************/

#include "gtest/gtest.h" // googletest header file

#include "__TESTTYPE__.h"

using namespace std;

void read_samples(string fname, int& n, vector<Sample>& pos_samples, vector<Sample>& neg_samples, KTermDnf& target);

TEST(__TESTTYPE__MonomialSuite, OneVarNoSample) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
}

TEST(__TESTTYPE__MonomialSuite, OneVarOnePos1) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples = { {1} };
    vector<Sample> neg_samples;
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{1}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(1);
    ASSERT_EQ(golden, learner.models[0]);

}

TEST(__TESTTYPE__MonomialSuite, OneVarOnePos2) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples = { {0} };
    vector<Sample> neg_samples;
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{0}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(0);
    ASSERT_EQ(golden, learner.models[0]);
}

TEST(__TESTTYPE__MonomialSuite, OneVarOneNeg1) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples = { {0} };
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{1}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(1);
    ASSERT_EQ(golden, learner.models[0]);
}

TEST(__TESTTYPE__MonomialSuite, OneVarOneNeg2) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples = { {1} };
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{0}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(0);
    ASSERT_EQ(golden, learner.models[0]);
}

TEST(__TESTTYPE__MonomialSuite, OneVarTwoPos) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples = {{0}, {1}};
    vector<Sample> neg_samples;
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_NO_HYPOTHESIS, learner.status);
}

TEST(__TESTTYPE__MonomialSuite, OneVarTwoNeg) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples = {{0}, {1}};
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_NO_HYPOTHESIS, learner.status);
}

TEST(__TESTTYPE__MonomialSuite, OneVarOnePosOneNeg1) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples = {{0}};
    vector<Sample> neg_samples = {{1}};
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{0}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(0);
    ASSERT_EQ(golden, learner.models[0]);
}

TEST(__TESTTYPE__MonomialSuite, OneVarOnePosOneNeg2) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    vector<Sample> pos_samples = {{1}};
    vector<Sample> neg_samples = {{0}};
    learner.fit(1, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    //ASSERT_EQ(KTermDnf({{1}}), learner.models[0]);
    KTermDnf golden; golden.resize(1);
    golden[0].push_back(1);
    ASSERT_EQ(golden, learner.models[0]);
}

TEST(__TESTTYPE__MonomialSuite, ManyModels) {
    __TESTTYPE__ learner;
    
    int k = 1;
    int l = 1;
    int B = 10;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    learner.fit(1, k, l, pos_samples, neg_samples, B);
    
    ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
    
    // create golden result
    vector<KTermDnf> golden; golden.resize(2);
    golden[0].resize(1); golden[0][0].push_back(0); 
    golden[1].resize(1); golden[1][0].push_back(1);

    // make sure terms are not permuted.
    sort(golden.begin(), golden.end());
    sort(learner.models.begin(), learner.models.end());

    ASSERT_EQ(golden, learner.models);
}



/***************
 * Parameterized tests for cardinality constraints
 ***/

// Tests for Monomail
class __TESTTYPE__MonomialCardinalityTest : public ::testing::TestWithParam<int> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(__TESTTYPE__MonomialCardinalityTest, CardinalityL) {
    __TESTTYPE__ learner;
    
    int l = GetParam();
    int k = 1;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    learner.fit(10, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
    
    int n_literals = 0;
    for(auto val: learner.models[0][0]){
        if(val == 0 || val == 1) ++n_literals;
    }
    ASSERT_EQ(l, n_literals);
}

INSTANTIATE_TEST_CASE_P(MonomialCardinalityTestInst,
                        __TESTTYPE__MonomialCardinalityTest,
                        ::testing::Range(1, 10+1));


// Tests for 2-term DNF
class __TESTTYPE__TwoTDnfCardinalityTest : public ::testing::TestWithParam<int> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(__TESTTYPE__TwoTDnfCardinalityTest, CardinalityL) {
    __TESTTYPE__ learner;
    
    int l = GetParam();
    int k = 2;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    learner.fit(10, k, l, pos_samples, neg_samples);
    
    ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
    
    int n_literals = 0;
    for(auto term: learner.models[0]){
        for(auto val: term){
            if(val == 0 || val == 1) ++n_literals;
        }
    }
    ASSERT_EQ(l, n_literals);
}

INSTANTIATE_TEST_CASE_P(TwoTDnfCardinalityTestInst,
                        __TESTTYPE__TwoTDnfCardinalityTest,
                        ::testing::Range(2, 10+1));

/***************
 * Parameterized tests for monomial normal tests
 ***/
class __TESTTYPE__MonomialNormalTest : public ::testing::TestWithParam<pair<string, int>> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(__TESTTYPE__MonomialNormalTest, Normal) {
    __TESTTYPE__ learner;
    
    auto params = GetParam(); 
    string fname = params.first;
    int n;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    KTermDnf target;
    int k = 1;
    int l = params.second;

    read_samples(fname, n, pos_samples, neg_samples, target);

    learner.fit(n, k, l, pos_samples, neg_samples);
  
    int n_literals = 0;
    for(auto val: target[0]){
        if(val == 0 || val == 1) ++n_literals;
    }

    if(l < n_literals){
        ASSERT_EQ(LS_NO_HYPOTHESIS, learner.status);
    }
    else if(l == n_literals){
        ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
        ASSERT_EQ(target, learner.models[0]);
    }
    else{
        ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
    }
}

INSTANTIATE_TEST_CASE_P(MonomialNormalTestInst,
                        __TESTTYPE__MonomialNormalTest,
                        ::testing::Values(
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_0_500_idx_1.dat", 1),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_0_500_idx_1.dat", 2),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_0_500_idx_1.dat", 3),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_0_500_idx_1.dat", 4),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_0_500_idx_1.dat", 5),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_1_100_idx_1.dat", 1),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_1_100_idx_1.dat", 2),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_1_100_idx_1.dat", 3),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_1_100_idx_1.dat", 4),
                            pair<string, int>("./test_data/n_k_mp_mn_10_3_1_100_idx_1.dat", 5)
                        ));

/***************
 * Parameterized tests for 2-term DNF normal tests
 ***/

class __TESTTYPE__TwoTermDNFNormalTest : public ::testing::TestWithParam<tuple<string, int, LearnerStatusT>> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(__TESTTYPE__TwoTermDNFNormalTest, Normal) {
    __TESTTYPE__ learner;
    
    string fname;
    int n;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    KTermDnf target;
    int k = 2;
    int l;
    LearnerStatusT golden_status;

    tie(fname, l, golden_status) = GetParam();

    read_samples(fname, n, pos_samples, neg_samples, target);

    learner.fit(n, k, l, pos_samples, neg_samples);

    if(golden_status == LS_ONE_OR_FEW_HYPOTHESES){
        ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
        
        // Make sure terms are not permuted.
        sort(target.begin(), target.end());
        sort(learner.models[0].begin(), learner.models[0].end());
        ASSERT_EQ(target, learner.models[0]);
    }
    else if(golden_status == LS_NO_HYPOTHESIS){
        ASSERT_EQ(LS_NO_HYPOTHESIS, learner.status);
    }
    else{
        ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
    }
}

INSTANTIATE_TEST_CASE_P(TwoTermDNFNormalTestInst,
                        __TESTTYPE__TwoTermDNFNormalTest,
                        ::testing::Values(
                            tuple<string, int, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", 2, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", 3, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", 4, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", 5, LS_ONE_OR_FEW_HYPOTHESES),
                            tuple<string, int, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", 6, LS_MANY_HYPOTHESES)
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {1,1}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {2,1}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {2,2}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {3,1}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {3,2}, LS_ONE_OR_FEW_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {3,3}, LS_MANY_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {4,1}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {4,2}, LS_MANY_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {4,3}, LS_MANY_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {4,4}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {5,1}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {5,2}, LS_MANY_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {5,3}, LS_MANY_HYPOTHESES),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {5,4}, LS_NO_HYPOTHESIS),
//                            tuple<string, vector<int>, LearnerStatusT>("test_data/n_k1_k2_mp_mn_10_2_3_5_100_idx_1.dat", {5,5}, LS_NO_HYPOTHESIS)
                        ));


/***************
 * Parameterized tests for 2-term DNF normal tests
 ***/

class __TESTTYPE__ThreeTermDNFNormalTest : public ::testing::TestWithParam<tuple<string, int, LearnerStatusT>> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(__TESTTYPE__ThreeTermDNFNormalTest, Normal) {
    __TESTTYPE__ learner;
    
    string fname;
    int n;
    vector<Sample> pos_samples;
    vector<Sample> neg_samples;
    KTermDnf target;
    int k = 3;
    int l;
    LearnerStatusT golden_status;

    tie(fname, l, golden_status) = GetParam();

    read_samples(fname, n, pos_samples, neg_samples, target);

    learner.fit(n, k, l, pos_samples, neg_samples);

    if(golden_status == LS_ONE_OR_FEW_HYPOTHESES){
        ASSERT_EQ(LS_ONE_OR_FEW_HYPOTHESES, learner.status);
        
        // Make sure terms are not permuted.
        sort(target.begin(), target.end());
        sort(learner.models[0].begin(), learner.models[0].end());
        ASSERT_EQ(target, learner.models[0]);
    }
    else if(golden_status == LS_NO_HYPOTHESIS){
        ASSERT_EQ(LS_NO_HYPOTHESIS, learner.status);
    }
    else{
        ASSERT_EQ(LS_MANY_HYPOTHESES, learner.status);
    }
}

INSTANTIATE_TEST_CASE_P(ThreeTermDNFNormalTestInst,
                        __TESTTYPE__ThreeTermDNFNormalTest,
                        ::testing::Values(
                            tuple<string, int, LearnerStatusT>("test_data/n_10_k_2_2_2_mp_10_mn_100_idx_0001.dat", 3, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_10_k_2_2_2_mp_10_mn_100_idx_0001.dat", 4, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_10_k_2_2_2_mp_10_mn_100_idx_0001.dat", 5, LS_NO_HYPOTHESIS),
                            tuple<string, int, LearnerStatusT>("test_data/n_10_k_2_2_2_mp_10_mn_100_idx_0001.dat", 6, LS_ONE_OR_FEW_HYPOTHESES),
                            tuple<string, int, LearnerStatusT>("test_data/n_10_k_2_2_2_mp_10_mn_100_idx_0001.dat", 7, LS_MANY_HYPOTHESES)
                        ));
