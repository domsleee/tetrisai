#pragma once
#include "src/common/BlockType.hpp"
#include <vector>

const int NUM_FACTORS = 17;
using Weighting = std::vector<double>;

class EvaluateWeightings {
 public:
  std::vector<std::pair<double, Weighting>> rankWeightings(const std::vector<Weighting> &weightings);
 private:
  std::vector<std::vector<BlockType>> getPieceSets(int numSets);
};
