#pragma once
#include <iostream>
#include <stdlib.h>
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"

#include "src/shared/MoveFinder/CacheMoveFinder.tpp"
#include "src/pso/NewEvaluateWeightingsContainer.tpp"
#include "src/shared/Config.hpp"


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

template <typename MoveEvaluator>
double get_score_regular(MoveEvaluator me, const Config &cfg) {
  auto moveFinder = MoveFinderRewrite();
  moveFinder.setMaxDropRem(2);
  auto ew_container = NewEvaluateWeightingsContainer(
    me,
    CacheMoveFinder(moveFinder)
  );
  auto ew = ew_container.getInstance();
  cfg.applyConfig(ew);
  return ew.runAllPieceSets();
}

template <typename MoveEvaluator>
double get_score_regular(MoveEvaluator me, int seed=-1) {
  Config cfg;
  cfg.seed = seed;
  return get_score_regular(me, cfg);
}

double get_score_regular(const Weighting &w, int seed=-1) {
  return get_score_regular(MoveEvaluatorAdapter(MoveEvaluator(), w), seed);
}

double get_score_regular_bench(const Weighting &w, int num_games=1, int seed=-1) {
  auto ew_container = NewEvaluateWeightingsContainer(
    MoveEvaluatorAdapter(MoveEvaluator(), w),
    CacheMoveFinder<MoveFinderRewrite>()
  );
  auto ew = ew_container.getInstance();
  ew.setNumGames(num_games);
  if (seed != -1) ew.setSeed(seed);
  return ew.runAllPieceSets();
}

double get_score_18_19(const Weighting &w1, const Weighting &w2) {
  auto ew_container = NewEvaluateWeightingsContainer(
      MoveEvaluatorAdapter(MoveEvaluator(), w1),
      CacheMoveFinder(MoveFinderRewrite())
  );
  auto ew = ew_container.getInstance();

  ew.setNumGames(500);
  ew.setSeed(25);

  auto nxMoveFinder = MoveFinderRewrite();
  nxMoveFinder.setMaxDropRem(2);
  auto cacheNxMoveFinder = CacheMoveFinder(nxMoveFinder);
  auto nxMoveEvaluator = MoveEvaluatorAdapter(MoveEvaluator(), w2);

  ew.runPieceSet_handler_->addTransition(100, [&](auto& rps) -> void {
    rps.getNextMoveHandler_.setMoveEvaluator(nxMoveEvaluator);
  });

  ew.runPieceSet_handler_->addTransition(130, [&](auto& rps) -> void {
    rps.getNextMoveHandler_.setMoveFinder(cacheNxMoveFinder);
  });

  return ew.runAllPieceSets();
}


