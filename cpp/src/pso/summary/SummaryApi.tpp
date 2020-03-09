#include "src/pso/summary/SummaryApi.h"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/pso/NewEvaluateWeightingsContainer.tpp"
#include "src/pso/summary/SummaryApiUtility.tpp"

template<typename MyMoveFinder>
SummaryResult SummaryApi::getSummaryHelper(const std::string &name) const {
  auto info = readLogFile(name);
  auto w = info.weights;
  Config config = info.config;
  config.seed = 200;
  auto scores = getScores<MyMoveFinder>(w, config, info.group);

  return {
    name,
    info.group,
    scores,
    config
  };
}

template<typename MyMoveFinder>
SummaryResult SummaryApi::getSummaryHelper(const std::string &name1, const std::string &name2, int transitionLines) const {
  auto info1 = readLogFile(name1);
  auto info2 = readLogFile(name2);
  Config config = info1.config;
  config.startingLevel = Config::UNDEF;
  config.maxDropRem = Config::UNDEF;
  config.numLines = Config::UNDEF;
  config.seed = 200;
  auto scores = getScoresTransition<MyMoveFinder>(info1.weights, info2.weights, config, info1.group, transitionLines);

  return {
    name1 + "_" + name2 + "_FSM",
    info1.group + "_" + info2.group,
    scores,
    config
  };
}

