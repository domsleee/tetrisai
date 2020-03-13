#include <iostream>
#include <stdlib.h>
#include <string>
#include "src/shared/Config.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/pso/SimpleApi.tpp"


const std::string moveEvaluatorGroup = MOVE_EVALUATOR_AUGMENTED_LOC;

void run(int argc, char ** argv, Config cfg);

int main(int argc, char ** argv) {
  Config cfg;
  cfg.numLines = 230;
  cfg.startingLines = 130;
  cfg.maxDropRem = 2;
  cfg.startingLevel = 19;
  cfg.averageAmount = 250;
  cfg.seed = 800;
  cfg.numGames = 500;

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    cfg.print();
    std::cout << "MoveEvaluatorGroup used: " << moveEvaluatorGroup << '\n';
    exit(0);
  }

  run(argc, argv, cfg);
}

void run(int argc, char ** argv, Config cfg) {
  auto me = getMoveEvaluatorGroups().at(moveEvaluatorGroup);
  int numFactors = me.NUM_FACTORS;

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

  me.setWeights(weightings);
  std::cout << getEvaluateWeightings(me, cfg).runAllPieceSets() << '\n';
}


