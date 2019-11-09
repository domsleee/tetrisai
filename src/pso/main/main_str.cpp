#include "src/common/Weighting.hpp"
#include "src/pso/ClientApi.hpp"

#include <iostream>

int main() {
  auto weighting = WeightingFn::readFromString(basic_weighting_best);
  std::cout << get_score_regular(weighting) << '\n';
}