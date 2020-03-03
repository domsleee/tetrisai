#include <iostream>
#include <stdlib.h>
#include <string>
#include "src/pso/ClientApi.hpp"
#include "src/pso/main/all_evaluators.hpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"


#define NEW_METHOD 1
const std::string moveEvaluatorGroup = MOVE_EVALUATOR_GROUP_LINEAR;

// for old method
//using MoveEvaluatorT = MoveEvaluatorBlockLinear;

void run(int argc, char ** argv, Config cfg);

int main(int argc, char ** argv) {
  Config cfg;
  cfg.numLines = 130;
  cfg.startingLines = 0;
  cfg.maxDropRem = 3;
  cfg.startingLevel = 18;
  cfg.averageAmount = 50;

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    cfg.print();
#ifdef NEW_METHOD
    std::cout << "MoveEvaluatorGroup used: " << moveEvaluatorGroup << '\n';
#else
    std::cout << "MoveEvaluator used: " << typeid(MoveEvaluatorT).name() << '\n';
#endif
    exit(0);
  }

  run(argc, argv, cfg);
}

void run(int argc, char ** argv, Config cfg) {
#ifdef NEW_METHOD
  auto me = getMoveEvaluatorGroups().at(moveEvaluatorGroup);
  int numFactors = me.NUM_FACTORS;
#else
  int numFactors = MoveEvaluatorT::NUM_FACTORS;
#endif

  if (argc != numFactors+1) {
    printf("Expected exactly %d arguments (i.e. %d factors), given %d\n", numFactors+1, numFactors, argc);
    exit(1);
  }
  Weighting weightings(numFactors);
  for (int i = 0; i < numFactors; ++i) {
    weightings[i] = atof(argv[i+1]);
  }

  if (argc > numFactors+1) {
    cfg.seed = atoi(argv[numFactors+1]);
  }

#ifdef NEW_METHOD
  me.setWeights(weightings);
#else
  MoveEvaluatorT me(weightings);
#endif
  std::cout << get_score_regular(me, cfg) << '\n';
}


