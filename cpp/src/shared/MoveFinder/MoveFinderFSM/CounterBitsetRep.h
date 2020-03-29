#pragma once
#include <cassert>
#include <stdexcept>

#define FieldForLoop(v) for (auto v = FSMStateFields::DROP_REM; v < FSMStateFields::LAST; v = static_cast<FSMStateFields::Field>(static_cast<int>(v) + 1))
namespace FSMStateFields {
  enum Field {
    DROP_REM,
    DAS_REM,
    RELEASE_COOLDOWN,
    ROTATE_COOLDOWN,
    MOVE_COOLDOWN,
    IS_LEFT_HOLDING,
    FSM_STATE,
    LAST
  };
};

template<int N>
struct CooldownGroupLookup {
  static int val(int cooldown) {
    switch(cooldown) {
      case 0: return ct(0);
      case 1: return ct(1);
      case 2: return ct(2);
    }
    throw std::runtime_error("unknown cooldown");
  }
  static constexpr int ct(int v) {
    int ret = 0;
    for (int i = 0; i < N; ++i) ret |= (v << (2*i));
    return ret;
  }
};

class CounterBitsetRep {
 public:
  uint32_t getCounterRep() const { return counterRep_; }

  int getField(FSMStateFields::Field field, int offIndex) const {
    assert(0 <= offIndex && offIndex < getFieldSize(field) / 2);
    int offset = offIndex * 2;
    int mask = 0b11 << offset;
    return (getField(field) & mask) >> offset;
  }

  int getField(FSMStateFields::Field field) const {
    //printf("counterRep_: %s\n", binRep(counterRep_).c_str());
    return (counterRep_ & getMask(field)) >> getOffset(field);
  }

  void setField(FSMStateFields::Field field, int val) {
    //printf("BEF counterRep_: %s\n", binRep(counterRep_).c_str());
    assert(0 <= val && val <= getMaxFromField(field));
    //printf("val: %d, offset: %d\n", val, getOffset(field));
    counterRep_ = (counterRep_ & ~getMask(field)) | (val << getOffset(field));
    //printf("AFT counterRep_: %s\n", binRep(counterRep_).c_str());
  }

  template<int N>
  void setFieldGroup(FSMStateFields::Field field, int val) {
    setField(field, CooldownGroupLookup<N>::val(val));
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

  friend bool operator==(const CounterBitsetRep &lhs, const CounterBitsetRep &rhs) {
    return lhs.counterRep_ == rhs.counterRep_;
  }

 private:
  uint32_t counterRep_ = 0;
};