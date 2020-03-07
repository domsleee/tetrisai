#pragma once
#include "src/common/Action.h"
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

  static const int NUM_MOVE_COOLDOWNS = 3;
  int rotateCooldown_[2] = {0, 0};
  int releaseCooldown_ = 0;
  int moveCooldown_[NUM_MOVE_COOLDOWNS] = {0, 0, 0};
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
    rotateCooldown_[0] = std::max(rotateCooldown_[0], cooldown);
    rotateCooldown_[1] = std::max(rotateCooldown_[1], cooldown);
  }

  void setMoveCooldown(int cooldown) {
    for (int i = 0; i < NUM_MOVE_COOLDOWNS; ++i) {
      moveCooldown_[i] = std::max(moveCooldown_[i], cooldown);
    }
  }

  void nextFrame() {
    frameEntered_++;
    dasRem_ = std::max(dasRem_-1, 0);
    dropRem_ = std::max(dropRem_-1, 0);
    for (int i = 0; i < 3; ++i) {
      moveCooldown_[i] = std::max(moveCooldown_[i]-1, 0);
    }
    for (int i = 0; i < 2; ++i) {
      rotateCooldown_[i] = std::max(rotateCooldown_[i]-1, 0);
    }
    releaseCooldown_ = std::max(releaseCooldown_-1, 0);
  }

  friend bool operator==(const MoveFinderState &s1, const MoveFinderState &s2) {
    return s1.fsmState_ == s2.fsmState_
    && s1.isLeftHolding_ == s2.isLeftHolding_
    && s1.piece_ == s2.piece_
    && s1.rotateCooldown_[0] == s2.rotateCooldown_[0]
    && s1.rotateCooldown_[1] == s2.rotateCooldown_[1]
    && s1.moveCooldown_[0] == s2.moveCooldown_[0]
    && s1.moveCooldown_[1] == s2.moveCooldown_[1]
    && s1.moveCooldown_[2] == s2.moveCooldown_[2]
    && s1.releaseCooldown_ == s2.releaseCooldown_
    && s1.dropRem_ == s2.dropRem_
    && s1.dasRem_ == s2.dasRem_
    && s1.frameEntered_ == s2.frameEntered_; // you may not need this, however im leaving it in
  }

  friend std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState&) const;
};
