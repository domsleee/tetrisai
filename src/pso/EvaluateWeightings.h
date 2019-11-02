#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Weighting.hpp"
#include <vector>

class EvaluateWeightings {
 public:
  std::vector<std::pair<double, Weighting>> rankWeightings(const std::vector<Weighting> &weightings);
  void setSeed(int seed);
 private:
  int seed_ = 88;
  std::vector<std::vector<BlockType>> getPieceSets(int numSets);
};
