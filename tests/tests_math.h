// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef TEST_MATH_HH
#define TEST_MATH_HH 1

#include "pmgdlib_math.h"
using namespace pmgd;

TEST(pmlib_math, random) {
  EXPECT_EQ(rint(0), 0);

  int lbound = 20;
  int ubound = 60;
  for(int i = 0; i < (ubound - lbound)*100; ++i){
    int r = rint(lbound, ubound);
    EXPECT_TRUE(r >= lbound && r <= ubound);
  }
}

TEST(pmlib_math, random_sample){
  int population_size = 1000;
  int sample_size = 800;

  std::set<int> veto;
  std::vector<int> sample = rsample(population_size, sample_size);

  // correct size  
  EXPECT_EQ(sample.size(), sample_size);

  // uniques
  for(int i = 0; i < sample.size(); ++i){
    int val = sample[i];
    EXPECT_EQ(veto.count(val), 0);
    veto.insert(val);
  }
}

#endif