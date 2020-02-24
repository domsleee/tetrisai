#include <iostream>
#include <stdlib.h>
#include "src/pso/ClientApi.hpp"
#include "src/pso/main/all_evaluators.hpp"
#include "src/shared/Config.hpp"

using MoveEvaluatorT = MoveEvaluatorBlockLinearNoTetrisReady;

int main(int argc, char ** argv) {
  Config cfg;
  cfg.numLines = 230;
  cfg.startingLines = 130;
  cfg.maxDropRem = 2;

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    cfg.print();
    std::cout << "MoveEvaluator used: " << typeid(MoveEvaluatorT).name() << '\n';
    exit(0);
  }
  if (argc != MoveEvaluatorT::NUM_FACTORS+1) {
    printf("Expected exactly %d arguments (i.e. %d factors), given %d\n", MoveEvaluatorT::NUM_FACTORS+1, MoveEvaluatorT::NUM_FACTORS, argc);
    exit(1);
  }
  Weighting weightings(MoveEvaluatorT::NUM_FACTORS);
  for (int i = 0; i < MoveEvaluatorT::NUM_FACTORS; ++i) {
    weightings[i] = atof(argv[i+1]);
  }

  if (argc > MoveEvaluatorT::NUM_FACTORS+1) {
    cfg.seed = atoi(argv[MoveEvaluatorT::NUM_FACTORS+1]);
  }

  MoveEvaluatorT me(weightings);

  //cfg.print();
  std::cout << get_score_regular(me, cfg) << '\n';
}
