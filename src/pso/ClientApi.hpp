
#include <iostream>
#include <stdlib.h>
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator.hpp"
#include "src/shared/MoveFinder/CacheMoveFinder.tpp"
#include "src/pso/NewEvaluateWeightingsContainer.tpp"


double get_score_regular(const Weighting &w, int seed=-1) {
  auto ew_container = NewEvaluateWeightingsContainer(
    MoveEvaluatorAdapter(MoveEvaluator(), w),
    CacheMoveFinder<MoveFinder>()
  );
  auto ew = ew_container.getInstance();
  if (seed != -1) ew.setSeed(seed);
  return ew.runAllPieceSets();
}
