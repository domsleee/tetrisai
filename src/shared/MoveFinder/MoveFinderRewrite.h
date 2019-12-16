#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"

const int MAX_DAS_REM = 6;
const int DEFAULT_MAX_DROP_REM = 3; // 19 ==> 2

class MoveFinderRewrite {
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType) const;
  void setMaxDropRem(int v) { maxDropRem_ = v; }

 private:
  int maxDropRem_ = DEFAULT_MAX_DROP_REM;
  mutable std::unordered_map<int, int> holdingSeen_[2];
  mutable std::unordered_map<int, int> releasedSeen_;
  mutable std::unordered_set<BitPieceInfo> moveSet_;
  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md) const {
    runHolding(currentPiece, md, 0, maxDropRem_);
  }
  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md, int dasRem, int dropRem) const;
  void runReleased(const BitPieceInfo& currentPiece, bool lastHitUsed=false) const;
};
