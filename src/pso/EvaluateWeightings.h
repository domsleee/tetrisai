#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Weighting.hpp"
#include <vector>

class EvaluateWeightings {
 public:
  std::vector<std::pair<double, Weighting>> rankWeightings(const std::vector<Weighting> &weightings);
 private:
  std::vector<std::vector<BlockType>> getPieceSets(int numSets);
};
