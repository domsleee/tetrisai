#ifndef PIECE_SET_GETTER_H
#define PIECE_SET_GETTER_H

#include <vector>
#include <stdlib.h> 
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include <random>

static const int NUM_BLOCKS_PER_GAME = 50 + 575 + 1200;

struct PieceSetGetter {
  std::mt19937 rd_;
  std::uniform_int_distribution<> dis_{0, NUM_BLOCKS_TYPES};
  const int DEFAULT_SEED = 88;
  PieceSetGetter() {
    rd_.seed(DEFAULT_SEED);
  }
  void setSeed(int seed) { rd_.seed(seed); }

  std::vector<std::vector<BlockType>> getPieceSets(int numSets) {
    std::vector<std::vector<BlockType>> res(numSets);
    for (auto &game: res) {
      for (int i = 0; i < NUM_BLOCKS_PER_GAME; i++) {
        int block = dis_(rd_);
        game.push_back(allBlockTypes[block]);
      }
    }
    return res;
  }
};

#endif
