#pragma once
#include <vector>
#include "src/common/BlockType.hpp"

struct IPieceSetGetter {
  virtual ~IPieceSetGetter() {};
  virtual void setSeed(int seed) = 0;
  virtual std::vector<std::vector<BlockType>> getPieceSets(int numGames) const = 0;
};
