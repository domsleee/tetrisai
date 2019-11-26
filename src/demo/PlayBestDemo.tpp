#pragma once
#include "src/common/Weighting.hpp"
#include "src/common/BlockType.hpp"


template<typename MyPlayDemo, typename MyRunPieceSet>
class PlayBestDemo {
 public:
  PlayBestDemo(const MyPlayDemo &playDemo, const MyRunPieceSet &runPieceSetHandler):
    playDemoHandler_(playDemo),
    runPieceSetHandler_(runPieceSetHandler)
    {}
  void playBestDemo(std::vector<std::vector<BlockType>> &pieceSets);

 private:
  const MyPlayDemo &playDemoHandler_;
  const MyRunPieceSet &runPieceSetHandler_;
};

template<typename MyPlayDemo, typename MyRunPieceSet>
void PlayBestDemo<MyPlayDemo, MyRunPieceSet>::playBestDemo(std::vector<std::vector<BlockType>> &pieceSets) {
  // weightings
  // seed

  std::vector<BlockType> myPieceSet = {};
  int bestScore = -1;
  int ct = 1;
  for (const auto &pieceSet: pieceSets) {
    printf("play best demo %d/%lu, bestScore: %d\n", ct, pieceSets.size(), bestScore);
    int score = runPieceSetHandler_.runGame(pieceSet);
    printf("score: %d\n", score);
    if (score > bestScore) {
      bestScore = score;
      myPieceSet.resize(0);
      for (const auto piece: pieceSet) myPieceSet.push_back(piece);
    }
    ct++;
  }
  printf("bestScore: %d\n", bestScore);

  playDemoHandler_.playDemo(myPieceSet);
}
