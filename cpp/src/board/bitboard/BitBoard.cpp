#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include <stdexcept>
#include <cassert>
#include <iostream>


std::size_t std::hash<BitBoard>::operator()(const BitBoard& b) const {
  return std::hash<decltype(b.bitset_)>{}(b.bitset_);
}

BitBoard::BitBoard(const std::string& s) {
  std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
  int ind = 0;
  for (int r = 0; r < NUM_ROWS; ++r) {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      vs[r][c] = s[ind++] - '0';
    }
  }
  *this = {vs};
}

BitBoard::BitBoard(const std::vector<std::vector<int>>& vs) {
  BitBoardPre::precompute();
  for (int r = 0; r < NUM_ROWS; ++r) {
    assert(vs[r].size() == NUM_COLUMNS);
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (vs[r][c]) {
        height_ = std::max(height_, NUM_ROWS-r);
        int ind = r * NUM_COLUMNS + c;
        bitset_.set(ind);
      }
    }
  }

}

int BitBoard::applyPieceInfo(const BitPieceInfo& p) {
  const auto& pieceBitset = BitBoardPre::idToBitset(p.id_);

  int h = BitBoardPre::getMoveHeight(p.id_);
  height_ = std::max(height_, h);

  assert(vacant(p));
  bitset_ |= pieceBitset;
  static const int maskInt = (2 << (NUM_COLUMNS-1))-1;
  std::bitset<NUM_ROWS * NUM_COLUMNS> b(maskInt); // 1111111111
  std::bitset<NUM_ROWS * NUM_COLUMNS> mask; // covers everything AFTER (below) b
  mask.flip();
  int lineClears = 0;
  for (int r = 0; r < NUM_ROWS; r++) {
    mask <<= NUM_COLUMNS;
    if ((b & bitset_) == b) {
      std::bitset<NUM_ROWS * NUM_COLUMNS> myLowerMask = (~mask) >> NUM_COLUMNS;
      bitset_ = (mask & bitset_) | ((myLowerMask & bitset_) << NUM_COLUMNS);
      lineClears++;
    }
    b <<= NUM_COLUMNS;
  }
  return lineClears;
}

BitPieceInfo BitBoard::getPiece(BlockType blockType) const {
  return {BitBoardPre::getStartingPieceId(blockType), *this};
}

bool BitBoard::vacant(const BitPieceInfo& p) const {
  if (p.id_ == -1) return false;
  return vacant(p.id_);
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

bool BitBoard::vacant(int id) const {
  const auto& pieceBitset = BitBoardPre::idToBitset(id);
  return (bitset_ & pieceBitset).none();
}

int BitBoard::applyMove(const Move& move) {
  return applyPieceInfo(getPiece(BlockType::I_PIECE, move));
}

BitPieceInfo BitBoard::getPiece(BlockType blockType, const Move& position) const {
  return {BitBoardPre::getMoveFromId(position), *this};
}

BitPieceInfo BitBoard::getPiece(const Move& position) const {
  return getPiece(BlockType::I_PIECE, position);
}

BitPieceInfo BitBoard::getPieceFromId(int id) const {
  return {id, *this};
}

BitPieceInfo BitBoard::getEmptyPiece() const {
  return {BitBoardPre::getEmptyMoveId(), *this};
}

int BitBoard::getPileHeight() const {
  return height_;

  {
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
  }
};

bool BitBoard::hasNoMoves(BlockType blockType) const {
  return !vacant(getPiece(blockType));
}


// BitPieceInfo
std::size_t std::hash<BitPieceInfo>::operator()(const BitPieceInfo& p) const {
  return p.id_;
}

bool BitPieceInfo::canRotate(RotateDirection rd) const {
  auto nxId = BitBoardPre::getRotate(id_, rd);
  const BitPieceInfo &p = {nxId, b_};
  return b_.vacant(p);
}

BitPieceInfo BitPieceInfo::rotate(RotateDirection rd) const {
  assert(canRotate(rd));
  return {BitBoardPre::getRotate(id_, rd), b_};
}

bool BitPieceInfo::canMove(MoveDirection md) const {
  auto nxId = BitBoardPre::getMove(id_, md);
  const BitPieceInfo &p = {nxId, b_};
  return b_.vacant(p);
}

BitPieceInfo BitPieceInfo::move(MoveDirection md) const {
  assert(canMove(md));
  return {BitBoardPre::getMove(id_, md), b_};
}

BitPieceInfo BitPieceInfo::doAction(Action action) const {
  switch(action) {
    case Action::ROTATE_AC: return rotate(RotateDirection::ROTATE_AC);
    case Action::ROTATE_C: return rotate(RotateDirection::ROTATE_C);
    case Action::DOWN: return move(MoveDirection::DOWN);
    case Action::UP: return move(MoveDirection::UP);
    case Action::LEFT: return move(MoveDirection::LEFT);
    case Action::RIGHT: return move(MoveDirection::RIGHT);
    default: throw std::runtime_error("unknown action");
  }
  throw std::runtime_error("suppress compiler");
}

std::pair<bool, BitPieceInfo> BitPieceInfo::doActionCopy(Action action) const {
  int id = BitBoardPre::doActionOnEmptyBoard(getId(), action);
  if (id == -1) return {false, *this};
  if (!b_.vacant(id)) return {false, *this};
  return {true, {b_.getPieceFromId(id)}};
}

std::pair<bool, int> BitPieceInfo::doActionCopyId(Action action) const {
  int id = BitBoardPre::doActionOnEmptyBoard(getId(), action);
  if (id == -1) return {false, -1};
  if (!b_.vacant(id)) return {false, -1};
  return {true, id};
}


Move BitPieceInfo::getPosition() const {
  return BitBoardPre::idToMove(id_);
}

void BitPieceInfo::print() const {
  getPosition().print();
}
