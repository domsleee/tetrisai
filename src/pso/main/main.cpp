#include "src/pso/EvaluateWeightings.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char ** argv) {
  if (argc < NUM_FACTORS+1) {
    printf("Expected at least %d arguments, given %d\n", NUM_FACTORS+1, argc);
    exit(1);
  }
  Weighting weightings(NUM_FACTORS);
  for (int i = 0; i < NUM_FACTORS; i++) {
    weightings[i] = atof(argv[i+1]);
  }
  EvaluateWeightings ew;
  if (argc > NUM_FACTORS+1) {
    int seed = atoi(argv[NUM_FACTORS+1]);
    ew.setSeed(seed);
  }

  auto rankings = ew.rankWeightings({weightings});
  std::cout << rankings[0].first << '\n';
}
