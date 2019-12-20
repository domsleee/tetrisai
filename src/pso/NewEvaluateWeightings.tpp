#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "src/common/common.hpp"
#include "src/pso/PieceSetGetter.hpp"


template<typename MyRunPieceSet>
class NewEvaluateWeightings {
 public:
  NewEvaluateWeightings(const MyRunPieceSet &runPieceSet_handler):
    runPieceSet_handler_(std::make_unique<MyRunPieceSet>(runPieceSet_handler))
    {}

  double runAllPieceSets() const;
  void setSeed(int seed);
  void setNumGames(int numGames);
  std::unique_ptr<MyRunPieceSet> runPieceSet_handler_;
 private:
  PieceSetGetter ps_;
  int num_games_ = NUM_GAMES;
};


template<typename MyRunPieceSet>
double NewEvaluateWeightings<MyRunPieceSet>::runAllPieceSets() const {
  auto pieceSets = ps_.getPieceSets(num_games_);
  std::vector<ScoreManager> scores;
  for (const auto &pieceSet: pieceSets) {
    scores.push_back(runPieceSet_handler_->runGame(pieceSet));
  }
  if (scores.size() == 1) return scores[0].getScore();
  
  std::sort(scores.begin(), scores.end(), [](auto &s1, auto &s2) {
    return s1.getScore() < s2.getScore();
  });
  std::vector<int> scoreInts;
  for (auto sm: scores) scoreInts.push_back(sm.getScore());

  for (auto sm: scores) {
    //printf("score: %d, lines: %d\n", sm.getScore(), sm.getTotalLines());
  }
  double score = average(scoreInts.end()-30, scoreInts.end());
  return score;
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setSeed(int seed) {
  ps_.setSeed(seed);
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setNumGames(int num_games) {
  num_games_ = num_games;
}
