#pragma once
#include <vector>
#include "src/common/Weighting.hpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MeMfPairProvider.h"
#include "src/pso/NewEvaluateWeightingsFactory.tpp"
#include "src/pso/SimpleApi.tpp"


template<typename MyMoveFinder>
std::vector<int> getScores(const Weighting &w, const Config &config, std::string group) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w);
  auto ew = getEvaluateWeightings<MyMoveFinder>(me1, me1, config, 90);
  return ew.getSortedScoreInts();
}

template<typename MyMoveFinder>
std::vector<int> getScoresTransition(const Weighting &w1, const Weighting &w2, const Config &config, std::string group, int transitionLines) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w1);
  auto me2 = getMoveEvaluatorGroups().at(group).setWeights(w2);

  auto ew = getEvaluateWeightings<MyMoveFinder>(me1, me2, config, transitionLines);
  return ew.getSortedScoreInts();
}

template<typename MyMoveFinder=MoveFinderFSM>
std::vector<int> getScoresLookahead(const Weighting &w1, const Weighting &w2, const Config &config, std::string group, int transitionLines) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w1);
  auto me2 = getMoveEvaluatorGroups().at(group).setWeights(w2);
  MeMfPairProvider<MyMoveFinder> meMfPairProvider = getMeMfPairProvider<MyMoveFinder>(me1, me2, transitionLines);

  auto ew = NewEvaluateWeightingsFactory<MyMoveFinder>::getInstance(meMfPairProvider);
  // applyConfig
  config.applyConfig(ew);
  ew.setLookahead(1);
  return ew.getSortedScoreInts();
}
