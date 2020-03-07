#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"

class IMoveRecorder {
 public:
  virtual ~IMoveRecorder() {};
  virtual void recordMoves(const BitBoard &board, const BlockType blockType, const std::vector<BitPieceInfo> &allMoves) = 0;
};