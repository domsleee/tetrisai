#pragma once
#include <vector>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"
#include <memory>

const int MAX_DAS_REM = 6;
const int DEFAULT_MAX_DROP_REM = 3; // 19 ==> 2

class MoveFinderRewrite {
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType) const;
  void setMaxDropRem(int v) { maxDropRem_ = v; }

 private:
  int maxDropRem_ = DEFAULT_MAX_DROP_REM;

  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md, int dasRem, int dropRem, int frame) const;
  void runReleased(const BitPieceInfo& currentPiece, bool lastHitUsed, int frame, int dropRem) const;
  void addEdge(const BitPieceInfo &u, const BitPieceInfo &v, int priority, int frame) const;
};
