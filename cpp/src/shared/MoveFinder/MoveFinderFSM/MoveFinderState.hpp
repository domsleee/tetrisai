#pragma once
#include "src/common/Action.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveFinder/MoveFinderFSM/CounterBitsetRep.h"

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

  MoveFinderState(const BitPieceInfo &piece, bool isLeftHolding, int dropRem):
    pieceId_(piece.getId())
  {
    setIsLeftHolding(isLeftHolding);
    setDropRem(dropRem);
    setDasRem(1);
    setFsmState(FSMState::HOLDING);
  }

  std::string getBinRep() const { return binRep(counterVars_.getCounterRep()); }

  BitPieceInfo getPiece(const BitBoard &b) const { return b.getPieceFromId(pieceId_); }
  int getPieceId() const { return pieceId_; }
  void setPiece(const BitPieceInfo &p) { pieceId_ = p.getId(); }

  int getDasRem() const { return counterVars_.getField<FSMStateFields::DAS_REM>(); }
  void setDasRem(int dasRem) { counterVars_.setField<FSMStateFields::DAS_REM>(dasRem); }

  int getDropRem() const { return counterVars_.getField<FSMStateFields::DROP_REM>(); }
  void setDropRem(int dropRem) { counterVars_.setField<FSMStateFields::DROP_REM>(dropRem); }

  FSMState getFsmState() const { return static_cast<FSMState>(counterVars_.getField<FSMStateFields::FSM_STATE>()); }
  void setFsmState(FSMState fsmState) { counterVars_.setField<FSMStateFields::FSM_STATE>(fsmState); }
  
  bool getIsLeftHolding() const { return counterVars_.getField<FSMStateFields::IS_LEFT_HOLDING>(); }
  void setIsLeftHolding(bool isLeftHolding) { counterVars_.setField<FSMStateFields::IS_LEFT_HOLDING>(isLeftHolding); }

  // counters
  int getReleaseCooldown() const { return counterVars_.getField<FSMStateFields::RELEASE_COOLDOWN>(); }
  void setReleaseCooldown(int cooldown) { counterVars_.setField<FSMStateFields::RELEASE_COOLDOWN>(cooldown); }

  int getAllRotateCooldowns() const { return counterVars_.getField<FSMStateFields::ROTATE_COOLDOWN>(); }
  int getRotateCooldown(RotateDirection rd) const { return counterVars_.getField<FSMStateFields::ROTATE_COOLDOWN>(rd);}
  void setRotateCooldown(RotateDirection rd, int cooldown) {
    assert(cooldown >= getRotateCooldown(rd));
    counterVars_.setField<FSMStateFields::ROTATE_COOLDOWN>(rd, cooldown);
  }
  void decRotateCooldown(RotateDirection rd) {
    counterVars_.setField<FSMStateFields::ROTATE_COOLDOWN>(rd, std::max(getRotateCooldown(rd)-1, 0));
  }

  void setRotateCooldown(int cooldown) {
    for (auto rd: allRotateDirections) {
      setRotateCooldown(rd, std::max(getRotateCooldown(rd), cooldown));
    }
    // counterVars_.setFieldGroup<2, FSMStateFields::ROTATE_COOLDOWN>(cooldown);
  }

  int getAllMoveCooldowns() const { return counterVars_.getField<FSMStateFields::MOVE_COOLDOWN>(); }
  int getMoveCooldown(MoveDirection md) const { return counterVars_.getField<FSMStateFields::MOVE_COOLDOWN>(md); }
  void setMoveCooldown(MoveDirection md, int cooldown, bool skipAssert=false) {
    assert(skipAssert || cooldown >= getMoveCooldown(md));
    counterVars_.setField<FSMStateFields::MOVE_COOLDOWN>(md, cooldown);
  }
  void decMoveCooldown(MoveDirection md) {
    counterVars_.setField<FSMStateFields::MOVE_COOLDOWN>(md, std::max(getMoveCooldown(md)-1, 0));
  }
  void setMoveCooldown(int cooldown) {
    counterVars_.setFieldGroup<2, FSMStateFields::MOVE_COOLDOWN>(0);
  }
  void setSidewaysMoveCooldown(int cooldown) {
    for (auto md: sidewaysMoveDirections) {
      setMoveCooldown(md, std::max(getMoveCooldown(md), cooldown));
    }
  }

  void nextFrame() {
    frameEntered_++;
    setDasRem(std::max(getDasRem()-1, 0));
    setDropRem(std::max(getDropRem()-1, 0));
    for (auto md: validMoveDirections) {
      decMoveCooldown(md);
    }
    for (auto rd: allRotateDirections) {
      decRotateCooldown(rd);
    }
    setReleaseCooldown(std::max(getReleaseCooldown()-1, 0));
  }

  friend bool operator==(const MoveFinderState &s1, const MoveFinderState &s2) {
    return s1.pieceId_ == s2.pieceId_
    && s1.frameEntered_ == s2.frameEntered_ // you may not need this, however im leaving it in
    && s1.counterVars_.getCounterRep() == s2.counterVars_.getCounterRep();
  }

  friend std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState&) const;

 private:
  static const int NUM_MOVE_COOLDOWNS = static_cast<int>(cooldownMoveDirections.size());
  static const int NUM_ROTATE_COOLDOWNS = 2;

  std::string binRep(uint32_t v) const {
    std::string res = "";
    for (int i = 31; i >= 0; --i) res += (v & (1 << i) ? "1" : "0");
    return res;
  }

  CounterBitsetRep counterVars_;
  int pieceId_;

  // on zero, can use das (indicated by FSMState::HOLDING)
  // total: 28 bits

};
