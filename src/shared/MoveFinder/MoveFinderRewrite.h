#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"
#include <memory>

const int MAX_DAS_REM = 6;
const int DEFAULT_MAX_DROP_REM = 3; // 19 ==> 2

class MoveFinderRewrite {
 public:
  using AdjListT = std::unordered_map<BitPieceInfo, std::vector<BitPieceInfo>>;
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType) const;
  void setMaxDropRem(int v) { maxDropRem_ = v; }
  void setRecordEdges(bool record_edges) { record_edges_ = record_edges; }
  const AdjListT& getRecordedEdges() const { return pred_; }
  const auto& getPredPriority() const { return pred_priority_; }

 private:
  int maxDropRem_ = DEFAULT_MAX_DROP_REM;
  bool record_edges_ = false;
  //mutable std::unique_ptr<BitPieceInfo> startPiece_;
  mutable std::unordered_map<int, int> holdingSeen_[2];
  mutable std::unordered_map<int, int> releasedSeen_;
  mutable std::unordered_set<BitPieceInfo> moveSet_;
  mutable AdjListT pred_;
  mutable std::unordered_map<BitPieceInfo, int> pred_priority_;
  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md) const {
    runHolding(currentPiece, md, 0, maxDropRem_);
  }
  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md, int dasRem, int dropRem) const;
  void runReleased(const BitPieceInfo& currentPiece, bool lastHitUsed=false) const;
  void addEdge(const BitPieceInfo &u, const BitPieceInfo &v, int priority) const;
};
