#pragma once
#include <fstream>
#include <iostream>
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveRecorder/IMoveRecorder.h"

struct MoveRecorder: public IMoveRecorder {
  void recordMoves(const BitBoard &board, const BlockType blockType, const std::vector<BitPieceInfo> &allMoves) override {
    
  }
};
