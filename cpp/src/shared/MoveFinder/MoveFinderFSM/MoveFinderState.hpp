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

#define FieldForLoop(v) for (auto v = FSMStateFields::IS_LEFT_HOLDING; v < FSMStateFields::LAST; v = static_cast<FSMStateFields::Field>(static_cast<int>(v) + 1))
namespace FSMStateFields {
  enum Field {
    IS_LEFT_HOLDING,
    DROP_REM,
    DAS_REM,
    RELEASE_COOLDOWN,
    ROTATE_COOLDOWN,
    MOVE_COOLDOWN,
    FSM_STATE,
    LAST
  };
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

  std::string getBinRep() const { return binRep(counterRep_); }

  BitPieceInfo getPiece(const BitBoard &b) const { return b.getPieceFromId(pieceId_); }
  int getPieceId() const { return pieceId_; }
  void setPiece(const BitPieceInfo &p) { pieceId_ = p.getId(); }

  int getDasRem() const { return getField(FSMStateFields::DAS_REM); }
  void setDasRem(int dasRem) { setField(FSMStateFields::DAS_REM, dasRem); }

  int getDropRem() const { return getField(FSMStateFields::DROP_REM); }
  void setDropRem(int dropRem) { setField(FSMStateFields::DROP_REM, dropRem); }

  FSMState getFsmState() const { return static_cast<FSMState>(getField(FSMStateFields::FSM_STATE)); }
  void setFsmState(FSMState fsmState) { setField(FSMStateFields::FSM_STATE, fsmState); }
  
  bool getIsLeftHolding() const { return getField(FSMStateFields::IS_LEFT_HOLDING); }
  void setIsLeftHolding(bool isLeftHolding) { setField(FSMStateFields::IS_LEFT_HOLDING, isLeftHolding); }

  // counters
  int getReleaseCooldown() const { return getField(FSMStateFields::RELEASE_COOLDOWN); }
  void setReleaseCooldown(int cooldown) { setField(FSMStateFields::RELEASE_COOLDOWN, cooldown); }

  int getRotateCooldown(RotateDirection rd) const { return getField(FSMStateFields::ROTATE_COOLDOWN, rd);}
  void setRotateCooldown(int cooldown) {
    for (auto rd: allRotateDirections) setRotateCooldown(rd, cooldown);
  }
  void setRotateCooldown(RotateDirection rd, int cooldown) { setField(FSMStateFields::ROTATE_COOLDOWN, rd, cooldown); }

  int getMoveCooldown(MoveDirection md) const { return getField(FSMStateFields::MOVE_COOLDOWN, md); }
  void setMoveCooldown(MoveDirection md, int cooldown) { setField(FSMStateFields::MOVE_COOLDOWN, md, cooldown); }
  void setMoveCooldown(int cooldown) {
    for (auto md: cooldownMoveDirections) setMoveCooldown(md, cooldown);
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
      setMoveCooldown(md, std::max(getMoveCooldown(md)-1, 0));
    }
    for (auto rd: allRotateDirections) {
      setRotateCooldown(rd, std::max(getRotateCooldown(rd)-1, 0));
    }
    setReleaseCooldown(std::max(getReleaseCooldown()-1, 0));
  }

  friend bool operator==(const MoveFinderState &s1, const MoveFinderState &s2) {
    return s1.pieceId_ == s2.pieceId_
    && s1.frameEntered_ == s2.frameEntered_ // you may not need this, however im leaving it in
    && s1.counterRep_ == s2.counterRep_;
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

  inline int getField(FSMStateFields::Field field, int offIndex) const {
    assert(0 <= offIndex && offIndex < getFieldSize(field) / 2);
    int offset = offIndex * 2;
    int mask = 0b11 << offset;
    return (getField(field) & mask) >> offset;
  }

  inline int getField(FSMStateFields::Field field) const {
    //printf("counterRep_: %s\n", binRep(counterRep_).c_str());
    return (counterRep_ & getMask(field)) >> getOffset(field);
  }

  inline void setField(FSMStateFields::Field field, int val) {
    //printf("BEF counterRep_: %s\n", binRep(counterRep_).c_str());
    assert(0 <= val && val <= getMaxFromField(field));
    //printf("val: %d, offset: %d\n", val, getOffset(field));
    counterRep_ = (counterRep_ & ~getMask(field)) | (val << getOffset(field));
    //printf("AFT counterRep_: %s\n", binRep(counterRep_).c_str());
  }

  inline void setField(FSMStateFields::Field field, int offIndex, int val) {
    assert(0 <= offIndex && offIndex < getFieldSize(field) / 2);
    int offset = offIndex * 2;
    int v = getField(field);
    int mask = 0b11 << offset;
    v = (v & (~mask)) + (val << offset);
    setField(field, v);
  }

  static constexpr int getFieldSize(FSMStateFields::Field field) {
    switch(field) {
      case FSMStateFields::IS_LEFT_HOLDING: return 1;
      case FSMStateFields::DAS_REM: return 3;
      case FSMStateFields::DROP_REM: return 4;
      case FSMStateFields::RELEASE_COOLDOWN: return 2;
      case FSMStateFields::ROTATE_COOLDOWN: return 2*2;
      case FSMStateFields::MOVE_COOLDOWN: return 3*2;
      case FSMStateFields::FSM_STATE: return 2;
      case FSMStateFields::LAST: constAssert(false);
    }
    throw std::runtime_error("fail");
  }
  static constexpr int getMaxFromField(FSMStateFields::Field field) {
    return getMaxFromFieldSize(getFieldSize(field));
  }
  static constexpr int getMaxFromFieldSize(int size) {
    return (1 << size) - 1; 
  }
  static constexpr auto constAssert(bool n) -> void {  
    !n ? throw std::runtime_error("") : 0;
  }

  static constexpr int getOffset(FSMStateFields::Field field) noexcept {
    int ct = 0;
    FieldForLoop(f) {
      int fSize = getFieldSize(f);
      if (f == field) {
        return ct;
      }
      ct += fSize;
    }
    constAssert(false);
    return 0;
  }

  static constexpr int getMask(FSMStateFields::Field field) noexcept {
    int offset = getOffset(field);
    int r = offset + getFieldSize(field);
    // [offset, r)
    int ret = 0;
    constAssert(r <= 31);
    for (int l = offset; l < r; ++l) {
      ret |= 1 << l;
    }
    return ret;
  }

  uint32_t counterRep_ = 0;
  int pieceId_;

  // on zero, can use das (indicated by FSMState::HOLDING)
  // total: 28 bits

};
