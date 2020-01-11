#pragma once
#include "src/shared/MoveFinder/MoveFinderFSM/Action.h"
#include "src/board/bitboard/BitBoard.h"

enum FSMState {
  HOLDING,
  RELEASED,
  TAPPED_ONCE,
};

class MoveFinderState;
template<>
struct std::hash<MoveFinderState> {
  std::size_t operator()(const MoveFinderState&) const;
};


class MoveFinderState {
 public:
  FSMState fsmState_ = FSMState::HOLDING;
  bool isLeftHolding_;

  BitPieceInfo piece_;
  int rotateCooldown_[2] = {0, 0};
  int moveCooldown_ = 0; // on release ==> 1
  int dropRem_;
  int maxDropRem_;
  int dasRem_ = 1; // on zero, can use das (indicated by FSMState::HOLDING)

  // extra info for pathfinding
  int frameEntered_ = 0;

  MoveFinderState(const BitPieceInfo &piece, bool isLeftHolding, int maxDropRem):
    isLeftHolding_(isLeftHolding),
    piece_(piece),
    dropRem_(maxDropRem),
    maxDropRem_(maxDropRem)
  {
    dasRem_ = 1;
  }

  void setRotateCooldown(int cooldown) {
    rotateCooldown_[0] = std::max(rotateCooldown_[0], 1);
    rotateCooldown_[1] = std::max(rotateCooldown_[1], 1);
  }

  void nextFrame() {
    frameEntered_++;
    dasRem_ = std::max(dasRem_-1, 0);
    dropRem_ = std::max(dropRem_-1, 0);
    moveCooldown_ = std::max(moveCooldown_-1, 0);
    rotateCooldown_[(int)Action::ROTATE_AC] = std::max(rotateCooldown_[(int)Action::ROTATE_AC]-1, 0);
    rotateCooldown_[(int)Action::ROTATE_C] = std::max(rotateCooldown_[(int)Action::ROTATE_C]-1, 0);
  }

  friend bool operator==(const MoveFinderState &s1, const MoveFinderState &s2) {
    return s1.fsmState_ == s2.fsmState_
    && s1.isLeftHolding_ == s2.isLeftHolding_
    && s1.piece_ == s2.piece_
    && s1.rotateCooldown_[0] == s2.rotateCooldown_[0]
    && s1.rotateCooldown_[1] == s2.rotateCooldown_[1]
    && s1.moveCooldown_ == s2.moveCooldown_
    && s1.dropRem_ == s2.dropRem_
    && s1.dasRem_ == s2.dasRem_
    && s1.frameEntered_ == s2.frameEntered_; // you may not need this, however im leaving it in
  }

  friend std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState&) const;
};
