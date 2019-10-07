#pragma once
#include "src/common/Weighting.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/shared/GetNextMove.h"
#include <vector>

class RunOneGame {
 public:
  RunOneGame(std::shared_ptr<GetNextMove> getNextMoveHandler = std::make_shared<GetNextMove>()): getNextMoveHandler_(getNextMoveHandler) {}
  int runGame(const std::vector<BlockType> &pieceSet, const Weighting &weighting);

 private:
  bool hasNoMoves(const BitBoard &b, BlockType blockType) const;
  std::shared_ptr<GetNextMove> getNextMoveHandler_ = std::make_unique<GetNextMove>();
};
