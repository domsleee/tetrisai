#include <iostream>
#include <stdlib.h>
#include "src/pso/ClientApi.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"

using MoveEvaluatorT = MoveEvaluatorTetrisReady;

int main(int argc, char ** argv) {
  if (argc < MoveEvaluatorT::NUM_FACTORS+1) {
    printf("Expected at least %d arguments (i.e. %d factors), given %d\n", MoveEvaluatorT::NUM_FACTORS+1, MoveEvaluatorT::NUM_FACTORS, argc);
    exit(1);
  }
  Weighting weightings(MoveEvaluatorT::NUM_FACTORS);
  for (int i = 0; i < MoveEvaluatorT::NUM_FACTORS; ++i) {
    weightings[i] = atof(argv[i+1]);
  }

  int seed = -1;
  if (argc > MoveEvaluatorT::NUM_FACTORS+1) {
    int seed = atoi(argv[MoveEvaluatorT::NUM_FACTORS+1]);
  }

  MoveEvaluatorT me(weightings);
  std::cout << get_score_regular(me, seed) << '\n';
}
