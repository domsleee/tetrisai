#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include <stdexcept>
#include <cassert>
#include <iostream>


BitBoard::BitBoard(const std::vector<std::vector<int>>& v) {
  BitBoardPre::precompute();
  int ind = 0;
  for (const auto &row: v) {
    for (const auto t: row) {
      if (t) bitset_.set(ind);
      ind++;
    }
  }
}

int BitBoard::applyPieceInfo(const BitPieceInfo& p) {
  const auto& pieceBitset = BitBoardPre::idToBitset(p.id_);
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

  return lineClears; // todo: number of line clears
}

BitPieceInfo BitBoard::getPiece(BlockType blockType) const {
  return {BitBoardPre::getStartingPieceId(blockType), this};
}

bool BitBoard::vacant(const BitPieceInfo& p) const {
  if (p.id_ == -1) return false;
  const auto& pieceBitset = BitBoardPre::idToBitset(p.id_);
  return (bitset_ & pieceBitset).none();
}

bool BitBoard::vacant(const Move& move) const { 
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
  return {BitBoardPre::moveToId(position), this};
}



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
