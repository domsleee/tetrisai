#include <iostream>
#include <stdlib.h>
#include "src/pso/ClientApi.hpp"

int main(int argc, char ** argv) {
  auto weighting = WeightingFn::readFromString(basic_weighting_best);
  std::cout << get_score_regular_bench(weighting) << '\n';
}
