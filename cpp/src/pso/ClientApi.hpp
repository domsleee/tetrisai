#pragma once
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/ScoreManager.hpp"

#include "src/shared/MoveFinder/CacheMoveFinder.tpp"
#include "src/pso/NewEvaluateWeightingsContainer.tpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"


double get_score_regular(MoveEvaluatorGroup me, const Config &cfg, bool is19=true) {
  auto moveFinder = MoveFinderFSM();
  auto ew_container = NewEvaluateWeightingsContainer(
    me,
    moveFinder
  );
  auto ew = ew_container.getInstance();
  cfg.applyConfig(ew);
  return ew.runAllPieceSets();
}

double get_score_regular(MoveEvaluatorGroup me, int seed=-1) {
  Config cfg;
  cfg.seed = seed;
  return get_score_regular(me, cfg);
}

double get_score_regular(const Weighting &w, int seed=-1) {
  return get_score_regular(getMoveEvaluatorGroups().at(MOVE_EVALUATOR_GROUP_NORMAL).setWeights(w), seed);
}

std::vector<ScoreManager> get_transition_evaluation(MoveEvaluatorGroup me1, MoveEvaluatorGroup me2, Config cfg, int moveEvaluatorLineTransition=100) {
  auto mf1 = MoveFinderRewrite();
  auto ew_container = NewEvaluateWeightingsContainer(
    me1,
    mf1
  );

  auto ew = ew_container.getInstance();
  cfg.applyConfig(ew);

  assert(moveEvaluatorLineTransition <= 130);
  auto mf2 = MoveFinderRewrite();
  mf2.setMaxDropRem(2);
  //auto cacheNxMoveFinder = CacheMoveFinder(nxMoveFinder);

  ew.runPieceSet_handler_->addMfTransition(0, mf1);
  ew.runPieceSet_handler_->addMeTransition(0, me1);

  ew.runPieceSet_handler_->addMeTransition(moveEvaluatorLineTransition, me2);
  ew.runPieceSet_handler_->addMfTransition(130, mf2);

  auto sms = ew.getScoreManagers();
  std::sort(sms.begin(), sms.end(), [](auto &s1, auto &s2) {
    return s1.getScore() < s2.getScore();
  });
  return sms;
}

