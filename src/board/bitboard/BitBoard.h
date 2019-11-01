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

class BitPieceInfo;

class BitBoard {
 friend BitPieceInfo;
 public:
  using B = std::bitset<NUM_ROWS*NUM_COLUMNS>;
  BitBoard(){ BitBoardPre::precompute(); };
  int applyPieceInfo(const BitPieceInfo&);
  BitPieceInfo getPiece(BlockType blockType) const;
  bool vacant(const BitPieceInfo&) const;

  // convenience
  BitBoard(const std::vector<std::vector<int>>&);
  int applyMove(const Move&move);
  BitPieceInfo getPiece(const Move& position) const;
  BitPieceInfo getPiece(BlockType blockType, const Move& position) const;
  bool vacant(const Move&) const;
  bool vacant(const Coord&) const;
  bool vacant(int id) const;
  void print() const { std::cout << "BOARD: " << bitset_ << "\n\n"; return; } // todo
  friend bool operator==(const BitBoard &b1, const BitBoard &b2) {
    return b1.bitset_ == b2.bitset_;
  }

  // hmmm.
  BitPieceInfo getEmptyPiece() const;
  int getPileHeight() const;

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
  void print() const;

  friend bool operator==(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return p1.id_ == p2.id_;
  };

  friend size_t std::hash<BitPieceInfo>::operator ()(const BitPieceInfo&) const;

 private:
  BitPieceInfo(int id, const BitBoard *b) : id_(id), b_(b) {}
  int id_;
  const BitBoard* b_;
};
