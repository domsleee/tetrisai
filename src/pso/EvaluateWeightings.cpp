#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "src/pso/EvaluateWeightings.h"
#include "src/pso/RunOneGame.h"
#include <stdlib.h>     /* srand, rand */

static const int NUM_GAMES = 100;
// 50 to complete the board
// 575 to fill 230 lines
static const int NUM_BLOCKS_PER_GAME = 50 + 575 + 1200;


template<typename T>
double average(T it1, T it2) {
  return (double)std::accumulate(it1, it2, 0) / (it2-it1);
}

std::vector<std::pair<double, Weighting>> EvaluateWeightings::rankWeightings(const std::vector<Weighting> &weightings) {
  // get 
  RunOneGame rog;
  auto pieceSets = getPieceSets(NUM_GAMES);
  std::vector<std::pair<double, Weighting>> res;
  for (const auto weighting: weightings) {
    // run games with weightings
    std::vector<int> scores;
    for (const auto &pieceSet: pieceSets) {
      scores.push_back(rog.runGame(pieceSet, weighting));
    }
    // get an evaluation score
    std::sort(scores.begin(), scores.end());
    double score = average(scores.end()-30, scores.end());
    res.push_back({score, weighting});
  }
  return res;
}

std::vector<std::vector<BlockType>> EvaluateWeightings::getPieceSets(int numSets) {
  std::vector<std::vector<BlockType>> res(NUM_GAMES);
  const int seed = 88;
  srand(seed);
  int numBlocks = allBlockTypes.size();
  for (auto &game: res) {
    for (int i = 0; i < NUM_BLOCKS_PER_GAME; i++) {
      int block = rand() % numBlocks;
      game.push_back(allBlockTypes[block]);
    }
  }
  return res;
}
