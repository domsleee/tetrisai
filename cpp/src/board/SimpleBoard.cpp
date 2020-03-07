#include "src/board/SimpleBoard.h"
#include "src/board/SimpleRotator.hpp"
#include <stdexcept>
#include <set>
#include <functional>
#include <iostream>


Coord getMoveDirectionVector(MoveDirection m) {
  switch(m) {
    case MoveDirection::LEFT: return {0,-1};
    case MoveDirection::RIGHT: return {0, 1};
    case MoveDirection::UP: return {-1, 0};
    case MoveDirection::DOWN: return {1, 0};
    default: throw std::runtime_error{"should never happen"};
  }
}


SimpleBoard::SimpleBoard() {
  for (int r = 0; r < NUM_ROWS; r++) for (int c = 0; c < NUM_COLUMNS; c++) board_[r][c] = 0;
  for (int r = 0; r < NUM_ROWS; r++) rowCt_[r] = 0;
}

SimpleBoard::SimpleBoard(const std::vector<std::vector<int>> &startingBoard) {
  for (int r = 0; r < NUM_ROWS; r++) rowCt_[r] = 0;
  if (startingBoard.size() != NUM_ROWS) {
    throw std::runtime_error("incorrect number of rows in constructor");
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    if (startingBoard[r].size() != NUM_COLUMNS) {
      throw std::runtime_error("incorrect number of columns!");
    }
    for (int c = 0; c < NUM_COLUMNS; c++) {
      board_[r][c] = startingBoard[r][c];
      rowCt_[r] += startingBoard[r][c];
    }
  }
}

SimplePieceInfo SimpleBoard::getPiece(BlockType blockType) const {
  auto mv = PositionPieceGetter::getStartingPosition(blockType);
  return getPiece(blockType, mv);
}

SimplePieceInfo SimpleBoard::getPiece(BlockType blockType, Move& position) const {
  return SimplePieceInfo(*this, position, blockType);
};

bool SimpleBoard::vacant(const SimplePieceInfo& piece) const {
  return vacant(piece.position_);
}

bool SimpleBoard::vacant(const Move &m) const {
  for (const auto& coord: m.coords_) {
    if (!vacant(coord)) return false;
  }
  return true;
}

bool SimpleBoard::vacant(const Coord &c) const {
  if (c.r < 0 || c.r >= NUM_ROWS || c.c < 0 || c.c >= NUM_COLUMNS) return false;
  return board_[c.r][c.c] == 0;
}

int SimpleBoard::applyPieceInfo(const SimplePieceInfo &piece) {
  return applyMove(piece.getPosition());
}

int SimpleBoard::applyMove(const Move &move) {
  std::set<int> rowsForDeletion;
  assert(vacant(move));
  for (const auto coord: move.coords_) {
    assert(0 <= coord.r && coord.r < NUM_ROWS && 0 <= coord.c && coord.c < NUM_COLUMNS);
    board_[coord.r][coord.c] = 1;
    rowCt_[coord.r]++;
    if (rowCt_[coord.r] == NUM_COLUMNS) rowsForDeletion.insert(coord.r);
  }
  for (int r: rowsForDeletion) {
    printf("deleting row: %d\n", r);
    rowCt_[r] = 0;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      board_[r][c] = 0;
    }
    for (int r2 = r; r2 > 0; r2--) {
      rowCt_[r2] = rowCt_[r2-1];
      for (int c = 0; c < NUM_COLUMNS; c++) {
        board_[r2][c] = board_[r2-1][c];
      }
    }
    rowCt_[0] = 0;
  }
  if (rowsForDeletion.size()) {
    printf("rowsForDeletion: %lu\n", rowsForDeletion.size());
    for (int c = 0; c < NUM_COLUMNS; c++) board_[0][c] = 0;
  }
  return rowsForDeletion.size();
}

void SimpleBoard::print() {
  std::cout << "PRINT BOARD\n";
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLUMNS; c++) {
      std::cout << board_[r][c];
    }
    std::cout << '\n';
  }
  std::cout << "===============\n";
}


// SimplePieceInfo
bool SimplePieceInfo::canRotate(RotateDirection rotateDirection) const {
  auto nextMove = SimpleRotator::getRotatePosition(position_, blockType_, rotateState_, rotateDirection);
  for (auto coord: nextMove.coords_) {
    if (!board_->vacant(coord)) return false;
  }
  return true;
};
SimplePieceInfo SimplePieceInfo::rotate(RotateDirection rotateDirection) const {
  assert(canRotate(rotateDirection));
  auto ret = *this;
  ret.position_ = SimpleRotator::getRotatePosition(position_, blockType_, rotateState_, rotateDirection);
  ret.rotateState_ = rotateState_ + (rotateDirection == RotateDirection::ROTATE_C ? 1 : -1);
  if (ret.rotateState_ < 0) ret.rotateState_ = 3;
  if (ret.rotateState_ > 3) ret.rotateState_ = 0;
  return ret;
};
bool SimplePieceInfo::canMove(MoveDirection moveDirection) const {
  auto newPosition = position_;
  newPosition += getMoveDirectionVector(moveDirection);
  for (auto coord: newPosition.coords_) {
    if (!board_->vacant(coord)) return false;
  }
  return true;
};
SimplePieceInfo SimplePieceInfo::move(MoveDirection moveDirection) const {
  assert(canMove(moveDirection));
  SimplePieceInfo ret = *this;
  ret.position_ += getMoveDirectionVector(moveDirection);
  return ret;
}

Move SimplePieceInfo::getPosition() const {
  return position_;
}

void SimplePieceInfo::print() const {
  for (const auto &coord: position_.coords_) {
    printf("(%d, %d), ", coord.r, coord.c);
  }
  printf("\n");
}


void simplePieceSanity(const SimplePieceInfo &p) {
  // check order of coords
  for (int i = 0; i < 3; i++) {
    const auto &coord1 = p.position_.coords_[i];
    const auto &coord2 = p.position_.coords_[i+1];

    if (!(coord1.r < coord2.r || (coord1.r == coord2.r && coord1.c < coord2.c))) {
      coord1.print();
      coord2.print();
      throw std::runtime_error{"um"};
    }
  }
  for (const auto& coord: p.position_.coords_) {
    if (!(0 <= coord.r && coord.r < NUM_ROWS && 0 <= coord.c && coord.c < NUM_COLUMNS)) {
      throw std::runtime_error{"out of bounds..."};
    }
  }
}

std::size_t std::hash<SimplePieceInfo>::operator()(const SimplePieceInfo &p) const {
  //simplePieceSanity(p);
  return (NUM_ROWS*NUM_COLUMNS+1) * p.blockType_ + (p.position_.coords_[0].c * NUM_ROWS + p.position_.coords_[0].r);
}
