#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"
#include <memory>


class MoveFinderFSM {
  std::unordered_map<MoveFinderState, std::pair<MoveFinderState, Action>> pred_;
  std::unordered_map<BitPieceInfo, std::pair<MoveFinderState, int>> finalMoveToState_;
  int maxDropRem_ = 3;
  char firstMoveDirectionChar_ = '.';
  bool hasFirstMoveConstraint_ = false;
  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, Action action);
  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, MoveDirection md) {
#ifdef RECORD_MOVEFINDER_EDGES
    addEdge(s1, s2, toAction(md));
#else
    return;
#endif
  }
  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, RotateDirection rd) {
#ifdef RECORD_MOVEFINDER_EDGES
    addEdge(s1, s2, toAction(rd));
#else
    return;
#endif
  }
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType);
  void setFirstMoveDirectionChar(char firstMoveDirectionChar) {
    hasFirstMoveConstraint_ = true;
    firstMoveDirectionChar_ = firstMoveDirectionChar;
  }
  std::vector<std::string> getShortestPath(const BitPieceInfo piece) const;
  void setMaxDropRem(int maxDropRem) { maxDropRem_ = maxDropRem; }
  void setRecordEdges(bool ok) {}
};
