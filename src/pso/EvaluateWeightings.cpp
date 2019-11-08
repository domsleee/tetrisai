#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "src/pso/EvaluateWeightings.h"
#include "src/pso/RunOneGame.h"
#include <stdlib.h>     /* srand, rand */
#include "src/common/common.hpp"

template<typename T>
double average(T it1, T it2) {
  return (double)std::accumulate(it1, it2, 0) / (it2-it1);
}

std::vector<std::pair<double, Weighting>> EvaluateWeightings::rankWeightings(const std::vector<Weighting> &weightings) {
  // get 
  RunOneGame rog;
  auto pieceSets = ps_.getPieceSets(NUM_GAMES);
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

void EvaluateWeightings::setSeed(int seed) {
  ps_.setSeed(seed);
}
