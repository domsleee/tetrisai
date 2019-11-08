#pragma once
#include "src/common/Weighting.hpp"
#include "src/common/BlockType.hpp"
#include "src/pso/RunOneGame.h"


template<typename MyPlayDemo>
class PlayBestDemo {
 public:
  PlayBestDemo(const MyPlayDemo &playDemo): playDemoHandler_(playDemo) {}
  void playBestDemo(std::vector<std::vector<BlockType>> &pieceSets, const Weighting &weighting);

 private:
  const MyPlayDemo &playDemoHandler_;
};

template<typename MyPlayDemo>
void PlayBestDemo<MyPlayDemo>::playBestDemo(std::vector<std::vector<BlockType>> &pieceSets, const Weighting &weighting) {
  // weightings
  // seed

  std::vector<BlockType> myPieceSet = {};
  int bestScore = -1;
  int ct = 1;
  for (const auto &pieceSet: pieceSets) {
    RunOneGame rog; // todo. replace with parameter (should be based on the same things as MyPlayDemo)
    printf("play best demo %d/%lu, bestScore: %d\n", ct, pieceSets.size(), bestScore);
    int score = rog.runGame(pieceSet, weighting);
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
