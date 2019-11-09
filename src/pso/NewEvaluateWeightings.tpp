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
 private:
  PieceSetGetter ps_;
  std::unique_ptr<const MyRunPieceSet> runPieceSet_handler_;
};


template<typename MyRunPieceSet>
double NewEvaluateWeightings<MyRunPieceSet>::runAllPieceSets() const {
  auto pieceSets = ps_.getPieceSets(NUM_GAMES);
  std::vector<int> scores;
  for (const auto &pieceSet: pieceSets) {
    scores.push_back(runPieceSet_handler_->runGame(pieceSet));
  }
  std::sort(scores.begin(), scores.end());
  double score = average(scores.end()-30, scores.end());
  return score;
}

template<typename MyRunPieceSet>
void NewEvaluateWeightings<MyRunPieceSet>::setSeed(int seed) {
  ps_.setSeed(seed);
}
