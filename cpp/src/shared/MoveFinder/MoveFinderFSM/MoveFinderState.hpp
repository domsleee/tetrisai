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
  // extra info for pathfinding
  int frameEntered_ = 0;
  BitPieceInfo piece_;

  MoveFinderState(const BitPieceInfo &piece, bool isLeftHolding, int maxDropRem):
    isLeftHolding_(isLeftHolding),
    piece_(piece),
    dropRem_(maxDropRem),
    maxDropRem_(maxDropRem)
  {}

  int getDasRem() const { return dasRem_; }
  void setDasRem(int dasRem) { dasRem_ = dasRem; }

  int getDropRem() const { return dropRem_; }
  void setDropRem(int dropRem) { dropRem_ = dropRem; }

 

  FSMState getFsmState() const { return fsmState_; }
  void setFsmState(FSMState fsmState) {
    fsmState_ = fsmState;
  }
  
  bool getIsLeftHolding() const { return isLeftHolding_; }
  void setIsLeftHolding(bool isLeftHolding) { isLeftHolding_ = isLeftHolding; }

  // counters
  int getReleaseCooldown() const { return releaseCooldown_; }
  void setReleaseCooldown(int cooldown) {
    releaseCooldown_ = std::max(releaseCooldown_, cooldown);
  }

  int getRotateCooldown(RotateDirection rd) const { return rotateCooldown_[static_cast<int>(rd)]; }
  void setRotateCooldown(int cooldown) {
    for (auto rd: allRotateDirections) setRotateCooldown(rd, cooldown);
  }
  void setRotateCooldown(RotateDirection rd, int cooldown) {
    rotateCooldown_[static_cast<int>(rd)] = std::max(rotateCooldown_[static_cast<int>(rd)], cooldown);
  }

  int getMoveCooldown(MoveDirection md) const { return moveCooldown_[static_cast<int>(md)]; }
  void setMoveCooldown(MoveDirection md, int cooldown) { moveCooldown_[static_cast<int>(md)] = cooldown; }
  void setMoveCooldown(int cooldown) {
    for (auto md: cooldownMoveDirections) setMoveCooldown(md, cooldown);
  }

  void setSidewaysMoveCooldown(int cooldown) {
    static const int left = static_cast<int>(MoveDirection::LEFT);
    static const int right = static_cast<int>(MoveDirection::RIGHT);
    moveCooldown_[left] = std::max(moveCooldown_[left], cooldown);
    moveCooldown_[right] = std::max(moveCooldown_[right], cooldown);
  }

  void nextFrame() {
    frameEntered_++;
    dasRem_ = std::max(dasRem_-1, 0);
    dropRem_ = std::max(dropRem_-1, 0);
    for (int i = 0; i < NUM_MOVE_COOLDOWNS; ++i) {
      moveCooldown_[i] = std::max(moveCooldown_[i]-1, 0);
    }
    for (int i = 0; i < NUM_ROTATE_COOLDOWNS; ++i) {
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

 private:
  static const int NUM_MOVE_COOLDOWNS = static_cast<int>(cooldownMoveDirections.size());
  static const int NUM_ROTATE_COOLDOWNS = 2;

  FSMState fsmState_ = FSMState::HOLDING;
  bool isLeftHolding_;

  int rotateCooldown_[NUM_ROTATE_COOLDOWNS] = {0, 0};
  int releaseCooldown_ = 0;
  int moveCooldown_[NUM_MOVE_COOLDOWNS] = {0, 0, 0};
  int dropRem_;
  int maxDropRem_;
  int dasRem_ = 1; // on zero, can use das (indicated by FSMState::HOLDING)
};
