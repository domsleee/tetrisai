#include <iostream>
#include <stdlib.h>
#include "src/pso/ClientApi.hpp"

int main(int argc, char ** argv) {
  if (argc < NUM_FACTORS+1) {
    printf("Expected at least %d arguments, given %d\n", NUM_FACTORS+1, argc);
    exit(1);
  }
  Weighting weightings(NUM_FACTORS);
  for (int i = 0; i < NUM_FACTORS; i++) {
    weightings[i] = atof(argv[i+1]);
  }

  int seed = -1;
  if (argc > NUM_FACTORS+1) {
    int seed = atoi(argv[NUM_FACTORS+1]);
  }

  std::cout << get_score_regular(weightings, seed) << '\n';
}
