#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"

class MoveEvaluator {
 public:
  double evaluate(const BitBoard&, const BitPieceInfo&) {
    return 0;
  }
};

