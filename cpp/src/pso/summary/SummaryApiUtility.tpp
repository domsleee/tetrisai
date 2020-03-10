#pragma once
#include <vector>
#include "src/common/Weighting.hpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MeMfPairProvider.h"
#include "src/pso/NewEvaluateWeightingsFactory.tpp"

template<typename MyMoveFinder>
std::vector<int> getScores(const Weighting &w, const Config &config, std::string group) {
  auto ewContainer = NewEvaluateWeightingsContainer(
    getMoveEvaluatorGroups().at(group).setWeights(w),
    MyMoveFinder()
  );
  auto ew = ewContainer.getInstance();
  config.applyConfig(ew);
  return ew.getSortedScoreInts();
}

template<typename MyMoveFinder>
std::vector<int> getScoresTransition(const Weighting &w1, const Weighting &w2, const Config &config, std::string group, int transitionLines) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w1);
  auto me2 = getMoveEvaluatorGroups().at(group).setWeights(w2);
  
  auto mf1 = MyMoveFinder();
  auto mf2 = MyMoveFinder();

  auto ewContainer = NewEvaluateWeightingsContainer(
    me1,
    mf1
  );
  auto ew = ewContainer.getInstance();
  config.applyConfig(ew);

  mf2.setMaxDropRem(2);
  ew.runPieceSet_handler_->addMfTransition(0, mf1);
  ew.runPieceSet_handler_->addMeTransition(0, me1);

  ew.runPieceSet_handler_->addMeTransition(transitionLines, me2);
  ew.runPieceSet_handler_->addMfTransition(130, mf2);

  return ew.getSortedScoreInts();
}

template<typename MyMoveFinder=MoveFinderFSM>
std::vector<int> getScoresLookahead(const Weighting &w1, const Weighting &w2, const Config &config, std::string group, int transitionLines) {
  auto me1 = getMoveEvaluatorGroups().at(group).setWeights(w1);
  auto me2 = getMoveEvaluatorGroups().at(group).setWeights(w2);
  
  auto mf1 = MyMoveFinder();
  auto mf2 = MyMoveFinder();
  auto mf3 = MyMoveFinder();

  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);

  MeMfPairProvider<MyMoveFinder> meMfPairProvider(
    {
      {transitionLines, std::make_shared<MoveEvaluatorGroup>(me1)},
      {1000, std::make_shared<MoveEvaluatorGroup>(me2)}
    },
    {
      {130, std::make_shared<MyMoveFinder>(mf1)},
      {230, std::make_shared<MyMoveFinder>(mf2)},
      {1000, std::make_shared<MyMoveFinder>(mf3)}
    }
  );

  auto ew = NewEvaluateWeightingsFactory<MyMoveFinder>::getInstance(meMfPairProvider);
  // applyConfig
  Config cfg = config;
  cfg.startingLevel = Config::UNDEF;
  cfg.maxDropRem = Config::UNDEF;
  cfg.numLines = Config::UNDEF;
  cfg.seed = 200;
  cfg.numGames = 5;
  ew.setLookahead(1);
  cfg.applyConfig(ew);
  return ew.getSortedScoreInts();
}
