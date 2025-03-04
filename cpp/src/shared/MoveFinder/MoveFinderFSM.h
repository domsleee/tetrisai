#pragma once
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <vector>
#include <queue>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"
#include <memory>

#define MOVE_FINDER_FSM_PERFORMANCE 1
#define RECORD_MOVEFINDER_EDGES 1


namespace FSMTypes {
  using SeenT = std::unordered_set<MoveFinderState>;
  using MovesT = std::vector<bool>;
  using PairT = std::pair<int, MoveFinderState>;
}

class TopInfo;
class BfsInfo;

class MoveFinderFSM {
  const int SCORE_FRAME_ENTERED = 10000;
  const int SCORE_ROTATED = 100;
  std::unordered_map<MoveFinderState, std::pair<MoveFinderState, Action>> pred_;
  std::unordered_map<BitPieceInfo, std::pair<MoveFinderState, int>> finalMoveToState_;
  int maxDropRem_ = 3;
  char firstMoveDirectionChar_ = '.';
  bool hasFirstMoveConstraint_ = false;


  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, Action action);
  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, MoveDirection md) {
    addEdge(s1, s2, toAction(md));
  }
  void addEdge(const MoveFinderState &s1, const MoveFinderState &s2, RotateDirection rd) {
    addEdge(s1, s2, toAction(rd));
  }
 public:
  MoveFinderFSM() {};
 
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType);
  void setFirstMoveDirectionChar(char firstMoveDirectionChar) {
    if (firstMoveDirectionChar == '.') {
      hasFirstMoveConstraint_ = false;
      return;
    }
    hasFirstMoveConstraint_ = true;
    firstMoveDirectionChar_ = firstMoveDirectionChar;
  }
  std::vector<std::string> getShortestPath(const BitPieceInfo &piece) const;
  std::vector<std::pair<int, Action>> getShortestPathActions(const BitPieceInfo &piece) const;
  void setMaxDropRem(int maxDropRem) { maxDropRem_ = maxDropRem; }
  void setRecordEdges(bool ok) {}
  void setupInitialStates(BfsInfo &bfsInfo, const BitBoard &b, BlockType blockType);
  void runNode(const BitBoard &b, BfsInfo &bfsInfo);
  void runNode(const TopInfo &topInfo, BfsInfo &bfsInfo);
  void runHolding(const TopInfo &topInfo, BfsInfo &bfsInfo);
  void runReleased(const TopInfo &topInfo, BfsInfo &bfsInfo);
  void runTapped(const TopInfo &topInfo, BfsInfo &bfsInfo);

  void addNxFrame(const TopInfo&, BfsInfo&);
  void addNxFrame(const TopInfo& topInfo, BfsInfo& bfsInfo, int frames);
  void addNxFrameIntoReleaseQ(const TopInfo& topInfo, BfsInfo& bfsInfo, int frames=1);
  void considerRotate(const TopInfo& topInfo, BfsInfo& bfsInfo) {
    return considerRotate(topInfo, bfsInfo, maxDropRem_ + 1);
  }
  void considerRotate(const TopInfo& topInfo, BfsInfo& bfsInfo, int rotateRestrictionDropRem);
  bool considerMovingDown(const TopInfo&, BfsInfo&);
};
