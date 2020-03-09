#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/pso/PieceSetGetter.hpp"
#include "src/shared/ScoreManager.hpp"
#include <execution>

#ifndef PARALLEL
#define PARALLEL par_unseq
#endif

template<typename MyRunPieceSet>
class NewEvaluateWeightings {
 public:
  NewEvaluateWeightings(const MyRunPieceSet &runPieceSet_handler):
    runPieceSet_handler_(std::make_unique<MyRunPieceSet>(runPieceSet_handler))
    {}

  double runAllPieceSets() const;
  std::vector<int> getSortedScoreInts() const;
  std::vector<ScoreManager> getScoreManagers() const;
  void setSeed(int seed);
  void setNumGames(int numGames);
  void setNumLines(int numLines);
  void setStartingLines(int startingLines);
  void setMaxDropRem(int dropRem);
  void setLookahead(int lookahead) { lookahead_ = lookahead; }
  void setAverageAmount(int averageAmount) { averageAmount_ = averageAmount; }
  void setStartingLevel(int startingLevel) { runPieceSet_handler_->setStartingLevel(startingLevel); }
  std::unique_ptr<MyRunPieceSet> runPieceSet_handler_;
 private:
  PieceSetGetter ps_;
  int num_games_ = NUM_GAMES;
  int averageAmount_ = 30;
  int lookahead_ = 0;
};


template<typename MyRunPieceSet>
double NewEvaluateWeightings<MyRunPieceSet>::runAllPieceSets() const {
  auto scoreInts = getSortedScoreInts();
  if (scoreInts.size() == 1) return scoreInts[0];
  double score = average(scoreInts.cend()-averageAmount_, scoreInts.cend());
  return score;
}

template<typename MyRunPieceSet>
std::vector<int> NewEvaluateWeightings<MyRunPieceSet>::getSortedScoreInts() const {
  auto scores = getScoreManagers();
  std::sort(scores.begin(), scores.end(), [](auto &s1, auto &s2) {
    return s1.getScore() < s2.getScore();
  });
  std::vector<int> scoreInts;
  for (auto sm: scores) scoreInts.push_back(sm.getScore());
  return scoreInts;
}

template<typename MyRunPieceSet>
std::vector<ScoreManager> NewEvaluateWeightings<MyRunPieceSet>::getScoreManagers() const {
  auto pieceSets = ps_.getPieceSets(num_games_);
  std::vector<ScoreManager> scores(pieceSets.size());

  auto &runPieceSet = *runPieceSet_handler_;
  BitBoardPre::precompute();

  auto fn = [runPieceSet](const std::vector<BlockType> &pieceSet) -> ScoreManager {
    return runPieceSet.runGame(pieceSet);
  };
  auto fn2 = [runPieceSet](const std::vector<BlockType> &pieceSet) -> ScoreManager {
    return runPieceSet.runGameWithLookahead(pieceSet);
  };

  if (lookahead_ > 0) {
    std::transform(std::execution::PARALLEL, // par, seq, par_unseq
               pieceSets.begin(), pieceSets.end(), 
               scores.begin(), fn2);
  } else {
    std::transform(std::execution::PARALLEL, // par, seq, par_unseq
               pieceSets.begin(), pieceSets.end(), 
               scores.begin(), fn);
  }

  return scores;
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setSeed(int seed) {
  ps_.setSeed(seed);
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setNumGames(int num_games) {
  num_games_ = num_games;
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setNumLines(int numLines) {
  runPieceSet_handler_->setNumLines(numLines);
}


template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setStartingLines(int startingLines) {
  runPieceSet_handler_->setStartingLines(startingLines);
}


template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setMaxDropRem(int maxDropRem) {
  runPieceSet_handler_->setMaxDropRem(maxDropRem);
}

