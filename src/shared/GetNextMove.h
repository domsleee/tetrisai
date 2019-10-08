#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/ScoreManager.hpp"

using Weighting = std::vector<double>;

class GetNextMove {
 public:
  Move getNextMove(const BitBoard& board, BlockType blockType, const Weighting &w, const ScoreManager &sm);
};
