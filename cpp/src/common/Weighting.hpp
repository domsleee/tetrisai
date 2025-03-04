#pragma once
#include <istream>
#include <string>
#include <sstream>
#include <vector>

const int NUM_FACTORS = 17;
using Weighting = std::vector<double>;

namespace WeightingFn {
  Weighting readFromStream(std::istream &is);
  Weighting readFromString(const std::string &s);
  /*
  Weighting readFromStream(std::istream &is, int num_factors=NUM_FACTORS);
  Weighting readFromString(const std::string &s, int num_factors=NUM_FACTORS);*/
};
