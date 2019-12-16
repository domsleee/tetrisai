#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"

const int MAX_DAS_REM = 6;
const int MAX_DROP_REM = 3; // 19 ==> 2

class MoveFinderRewrite {
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType) const;

 private:   
  mutable std::unordered_map<int, int> holdingSeen_[2];
  mutable std::unordered_map<int, int> releasedSeen_;
  mutable std::unordered_set<BitPieceInfo> moveSet_;
  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md, int dasRem=0, int dropRem=MAX_DROP_REM) const;
  void runReleased(const BitPieceInfo& currentPiece, bool lastHitUsed=false) const;
};
