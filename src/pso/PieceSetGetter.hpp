#ifndef PIECE_SET_GETTER_H
#define PIECE_SET_GETTER_H

#include <vector>
#include <stdlib.h> 
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/variate_generator.hpp"
#include "boost/random/uniform_int.hpp"
#include <random>

static const int NUM_BLOCKS_PER_GAME = 50 + 575 + 1200;

struct PieceSetGetter {
  boost::mt19937 gen_;
  boost::uniform_int<> dist_{0, NUM_BLOCK_TYPES-1};
  boost::random::variate_generator<boost::mt19937&, boost::uniform_int<>> rd_{gen_, dist_};
  const int DEFAULT_SEED = 88;
  PieceSetGetter() {
    gen_.seed(DEFAULT_SEED);
  }
  void setSeed(int seed) { gen_.seed(seed); }

  std::vector<std::vector<BlockType>> getPieceSets(int numSets=NUM_GAMES) {
    std::vector<std::vector<BlockType>> res(numSets);
    for (auto &game: res) {
      for (int i = 0; i < NUM_BLOCKS_PER_GAME; i++) {
        int block = rd_();
        game.push_back(allBlockTypes[block]);
      }
    }
    return res;
  }
};

#endif
