#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include <stdexcept>
#include <cassert>
#include <iostream>


std::size_t std::hash<BitBoard>::operator()(const BitBoard& b) const {
  return std::hash<decltype(b.bitset_)>{}(b.bitset_);
}

BitBoard::BitBoard(const std::vector<std::vector<int>>& v) {
  BitBoardPre::precompute();
  int ind = 0;
  int h = NUM_ROWS;
  for (const auto &row: v) {
    for (const auto t: row) {
      if (t) {
        height_ = std::max(height_, h);
        bitset_.set(ind);
      }
      ind++;
    }
    h--;
  }
}

int BitBoard::applyPieceInfo(const BitPieceInfo& p) {
  const auto& pieceBitset = BitBoardPre::idToBitset(p.id_);

  int h = BitBoardPre::getMoveHeight(p.id_);
  height_ = std::max(height_, h);

  assert(vacant(p));
  bitset_ |= pieceBitset;
  static const int maskInt = (2 << (NUM_COLUMNS-1))-1;
  std::bitset<200> b(maskInt); // 1111111111
  std::bitset<200> mask; // covers everything AFTER (below) b
  mask.flip();
  int lineClears = 0;
  for (int r = 0; r < NUM_ROWS; r++) {
    mask <<= NUM_COLUMNS;
    if ((b & bitset_) == b) {
      std::bitset<200> myLowerMask = (~mask) >> NUM_COLUMNS;
      bitset_ = (mask & bitset_) | ((myLowerMask & bitset_) << NUM_COLUMNS);
      lineClears++;
    }
    b <<= NUM_COLUMNS;
  }

  return lineClears;
}

BitPieceInfo BitBoard::getPiece(BlockType blockType) const {
  return {BitBoardPre::getStartingPieceId(blockType), this};
}

bool BitBoard::vacant(const BitPieceInfo& p) const {
  if (p.id_ == -1) return false;
  const auto& pieceBitset = BitBoardPre::idToBitset(p.id_);
  return (bitset_ & pieceBitset).none();
}


// slower checks
bool BitBoard::vacant(const Move& move) const {
  //int id = BitBoardPre::getMoveFromId(move);
  for (const auto& coord: move.coords_) {
    if (!vacant(coord)) return false;
  }
  return true;
}
bool BitBoard::vacant(const Coord& coord) const {
  return !bitset_.test(coord.r*NUM_COLUMNS+coord.c);
}

int BitBoard::applyMove(const Move& move) {
  return applyPieceInfo(getPiece(BlockType::I_PIECE, move));
}

BitPieceInfo BitBoard::getPiece(BlockType blockType, const Move& position) const {
  return {BitBoardPre::getMoveFromId(position), this};
}

BitPieceInfo BitBoard::getPiece(const Move& position) const {
  return getPiece(BlockType::I_PIECE, position);
}

BitPieceInfo BitBoard::getPieceFromId(int id) const {
  return BitPieceInfo(id, this);
}

BitPieceInfo BitBoard::getEmptyPiece() const {
  return {BitBoardPre::getEmptyMoveId(), this};
}

int BitBoard::getPileHeight() const {
  return height_;

  // old method...
  int my_height = height_;
  static const int maskInt = (2 << (NUM_COLUMNS-1))-1;
  B b(maskInt); // 1111111111
  B zero;
  int h = 20;
  while (h > 0) {
    if ((bitset_ & b) != zero) {
      if (my_height != h) {
        printf("my_height:%d, h:%d\n", my_height, h);
        exit(1);
      }
      return h;
    }
    b <<= NUM_COLUMNS;
    h--;
  }
  return h;
};


// BitPieceInfo
std::size_t std::hash<BitPieceInfo>::operator()(const BitPieceInfo& p) const {
  return p.id_;
}

bool BitPieceInfo::canRotate(RotateDirection rd) const {
  auto nxId = BitBoardPre::getRotate(id_, rd);
  const BitPieceInfo &p = {nxId, b_};
  return b_->vacant(p);
}

BitPieceInfo BitPieceInfo::rotate(RotateDirection rd) const {
  assert(canRotate(rd));
  return {BitBoardPre::getRotate(id_, rd), b_};
}

bool BitPieceInfo::canMove(MoveDirection md) const {
  auto nxId = BitBoardPre::getMove(id_, md);
  const BitPieceInfo &p = {nxId, b_};
  return b_->vacant(p);
}

BitPieceInfo BitPieceInfo::move(MoveDirection md) const {
  assert(canMove(md));
  return {BitBoardPre::getMove(id_, md), b_};
}

Move BitPieceInfo::getPosition() const {
  return BitBoardPre::idToMove(id_);
}

void BitPieceInfo::print() const {
  getPosition().print();
}
