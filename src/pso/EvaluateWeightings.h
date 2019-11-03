#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Weighting.hpp"
#include "src/pso/PieceSetGetter.hpp"
#include <vector>

class EvaluateWeightings {
 public:
  std::vector<std::pair<double, Weighting>> rankWeightings(const std::vector<Weighting> &weightings);
  void setSeed(int seed);
 private:
  PieceSetGetter ps_;
};
