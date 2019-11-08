#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include "src/common/PositionPieceGetter.hpp"
#include "src/common/RotateDirection.hpp"
#include "src/common/MoveDirection.hpp"
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include <bitset>
#include <iostream>

class BitBoard;
class BitPieceInfo;


template<>
struct std::hash<BitBoard> {
  std::size_t operator()(const BitBoard&) const;
};

class BitBoard {
 friend BitPieceInfo;
 public:
  using B = std::bitset<NUM_ROWS*NUM_COLUMNS>;
  BitBoard(){ BitBoardPre::precompute(); };
  int applyPieceInfo(const BitPieceInfo&);
  BitPieceInfo getPiece(BlockType blockType) const;
  bool vacant(const BitPieceInfo&) const;

  // probably shouldn't use
  BitPieceInfo getPieceFromId(int id) const;

  // convenience
  BitBoard(const std::vector<std::vector<int>>&);
  int applyMove(const Move&move);
  BitPieceInfo getPiece(const Move& position) const;
  BitPieceInfo getPiece(BlockType blockType, const Move& position) const;
  bool vacant(const Move&) const;
  bool vacant(const Coord&) const;
  bool vacant(int id) const;
  friend bool operator==(const BitBoard &b1, const BitBoard &b2) {
    return b1.bitset_ == b2.bitset_;
  }
  friend std::ostream& operator<<(std::ostream& os, const BitBoard& b) {
    os << b.bitset_;
    return os;
  }

  // hmmm.
  BitPieceInfo getEmptyPiece() const;
  int getPileHeight() const;

  friend size_t std::hash<BitBoard>::operator ()(const BitBoard&) const;


 private:
  B bitset_;
  int height_ = 0;
};



template<>
struct std::hash<BitPieceInfo> {
  std::size_t operator()(const BitPieceInfo&) const;
};

class BitPieceInfo {
  friend BitBoard;
 public:
  BitPieceInfo(const BitPieceInfo &other) : BitPieceInfo(other.id_, other.b_) {}
  bool canRotate(RotateDirection) const;
  BitPieceInfo rotate(RotateDirection) const;
  bool canMove(MoveDirection) const;
  BitPieceInfo move(MoveDirection) const;
  Move getPosition() const;
  int getId() const { return id_; }
  void print() const;

  friend bool operator==(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return p1.id_ == p2.id_;
  };
  friend std::ostream& operator<<(std::ostream& os, const BitPieceInfo& p) {
    os << p.id_;
    return os;
  };
  friend bool operator<(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return p1.id_ < p2.id_;
  }

  friend size_t std::hash<BitPieceInfo>::operator ()(const BitPieceInfo&) const;

 private:
  BitPieceInfo(int id, const BitBoard *b) : id_(id), b_(b) {}
  int id_;
  const BitBoard* b_;
};
