#include "src/common/Weighting.hpp"
#include "src/pso/EvaluateWeightings.h"
#include <iostream>

int main() {
  EvaluateWeightings ew;
  auto weighting = WeightingFn::readFromString(basic_weighting_best);
  auto rankings = ew.rankWeightings({weighting});
  std::cout << rankings[0].first << '\n';
}