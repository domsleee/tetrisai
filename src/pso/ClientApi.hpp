
#include <iostream>
#include <stdlib.h>
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"

#include "src/shared/MoveFinder/CacheMoveFinder.tpp"
#include "src/pso/NewEvaluateWeightingsContainer.tpp"


// todo: use config
// get_score_move_finder_all
// get_score_level18_start_6frame_move
// get_score_level19_start_6frame_move
// get_score_level18_start_actual_das
// get_score_level19_start_actual_das

// later
// get_score_level18_start_6frame_move_lookahead
// get_score_level18_start_6frame_move_lookahead_quicktap
// get_score_level18_start_actual_das_lookahead
// get_score_level18_start_actual_das_lookahead_quicktap

double get_score_regular(const Weighting &w, int seed=-1) {
  auto ew_container = NewEvaluateWeightingsContainer(
    MoveEvaluatorAdapter(MoveEvaluator(), w),
    CacheMoveFinder<MoveFinder>()
  );
  auto ew = ew_container.getInstance();
  if (seed != -1) ew.setSeed(seed);
  return ew.runAllPieceSets();
}


double get_score_regular2(const Weighting &w, int seed=-1) {
  auto ew_container = NewEvaluateWeightingsContainer(
      MoveEvaluatorPenalty(w),
      CacheMoveFinder<MoveFinder>()
  );
  auto ew = ew_container.getInstance();
  if (seed != -1) ew.setSeed(seed);
  return ew.runAllPieceSets();
}
