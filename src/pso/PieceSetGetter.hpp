#ifndef PIECE_SET_GETTER_H
#define PIECE_SET_GETTER_H

#include <vector>
#include <stdlib.h> 
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"

static const int NUM_BLOCKS_PER_GAME = 50 + 575 + 1200;

struct PieceSetGetter {
  int seed_;
  void setSeed(int seed) { seed_ = seed; }

  std::vector<std::vector<BlockType>> getPieceSets(int numSets) {
    std::vector<std::vector<BlockType>> res(numSets);
    srand(seed_);
    int numBlocks = allBlockTypes.size();
    for (auto &game: res) {
      for (int i = 0; i < NUM_BLOCKS_PER_GAME; i++) {
        int block = rand() % numBlocks;
        game.push_back(allBlockTypes[block]);
      }
    }
    return res;
  }
};

#endif
