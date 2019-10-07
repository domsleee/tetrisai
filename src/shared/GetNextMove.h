#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"

class GetNextMove {
 public:
  Move getNextMove(const BitBoard& board, BlockType blockType);
};
