#pragma once
#include <vector>
#include "src/common/Weighting.hpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MeMfPairProvider.h"
#include "src/pso/NewEvaluateWeightingsFactory.tpp"
#include "src/pso/SimpleApi.tpp"


template<typename MyMoveFinder>
std::vector<ScoreManager> getScores(const Weighting &w, const Config &config, std::string group) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w);
  auto ew = getEvaluateWeightings<MyMoveFinder>(me1, me1, config, 90);
  return ew.getSortedScoreManagers();
}

template<typename MyMoveFinder>
std::vector<ScoreManager> getScoresTransition(const Weighting &w1, const Weighting &w2, const Config &config, std::string group, int transitionLines) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w1);
  auto me2 = getMoveEvaluatorGroups().at(group).setWeights(w2);

  auto ew = getEvaluateWeightings<MyMoveFinder>(me1, me2, config, transitionLines);
  return ew.getSortedScoreManagers();
}

template<typename MyMoveFinder=MoveFinderFSM>
std::vector<ScoreManager> getScoresLookahead(const Config &config, MoveEvaluatorGroup me1, MoveEvaluatorGroup me2, int transitionLines) {
  MeMfPairProvider<MyMoveFinder> meMfPairProvider = getMeMfPairProvider<MyMoveFinder>(me1, me2, transitionLines);
  auto ew = NewEvaluateWeightingsFactory<MyMoveFinder>::getInstance(meMfPairProvider);
  // applyConfig
  config.applyConfig(ew);
  ew.setLookahead(1);
  return ew.getSortedScoreManagers();
}
