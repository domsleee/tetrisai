#pragma once
#include "src/common/Move.hpp"
#include "src/common/Coord.hpp"
#include "src/common/BlockType.hpp"
#include <vector>


class Board {
 public:
  int applyMove(const Move& move, BlockType blockType) {
    for (auto & coord: move.coords_) {
      board_[coord.r][coord.c] = blockType;
    }
    return 0;
  }

  BlockType getCoord(const Coord& coord) {
    return board_[coord.r][coord.c];
  }

 private:
  std::vector<std::vector<BlockType>> board_;
};
