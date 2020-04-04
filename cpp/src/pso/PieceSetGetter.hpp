#pragma once

#include <vector>
#include <stdlib.h> 
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/pso/IPieceSetGetter.h"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/variate_generator.hpp"
#include "boost/random/uniform_int.hpp"
#include <random>

static const int NUM_BLOCKS_PER_GAME = 50 + 575 + 1200;

struct PieceSetGetter: public IPieceSetGetter {
  const int DEFAULT_SEED = 88;
  int seed_ = DEFAULT_SEED;
  void setSeed(int seed) override { seed_ = seed; }

  std::vector<std::vector<BlockType>> getPieceSets(int numGames=NUM_GAMES) const override {
    boost::mt19937 gen, gen2;
    boost::uniform_int<> dist{0, NUM_BLOCK_TYPES};
    boost::uniform_int<> dist2{0, NUM_BLOCK_TYPES-1};

    boost::random::variate_generator<boost::mt19937&, boost::uniform_int<>> rd1{gen, dist};
    boost::random::variate_generator<boost::mt19937&, boost::uniform_int<>> rd2{gen2, dist2};

    gen.seed(seed_);
    gen2.seed(seed_+1);

    std::vector<std::vector<BlockType>> res(numGames);
    int last = -1;
    for (auto &game: res) {
      for (int i = 0; i < NUM_BLOCKS_PER_GAME; i++) {
        int block = rd1();
        if (block == last || block == NUM_BLOCK_TYPES) block = rd2();
        game.push_back(allBlockTypes[block]);
        last = block;
      }
    }
    return res;
  }
};
