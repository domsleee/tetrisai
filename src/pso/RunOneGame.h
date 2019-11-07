#pragma once
#include "src/pso/EvaluateWeightings.h"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/shared/GetNextMove.h"
#include <vector>

class RunOneGame {
 public:
  RunOneGame() {
    getNextMoveHandler_ = std::make_unique<GetNextMove>();
  }
  int runGame(const std::vector<BlockType> &pieceSet, const Weighting &weighting);

 private:
  bool hasNoMoves(const BitBoard &b, BlockType blockType) const;
  std::unique_ptr<GetNextMove> getNextMoveHandler_;
};
