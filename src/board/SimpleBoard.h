#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include "src/common/PositionPieceGetter.hpp"
#include "src/common/RotateDirection.hpp"
#include "src/common/MoveDirection.hpp"
#include "src/common/common.hpp"


Coord getMoveDirectionVector(MoveDirection m);


class SimplePieceInfo;

class SimpleBoard {
 friend SimplePieceInfo;
 public:
  SimpleBoard();
  int applyPieceInfo(const SimplePieceInfo&);
  SimplePieceInfo getPiece(BlockType blockType) const;
  bool vacant(const SimplePieceInfo&) const;

  // convenience
  SimpleBoard(const std::vector<std::vector<int>>&);
  int applyMove(const Move&);
  SimplePieceInfo getPiece(BlockType blockType, Move& position) const;
  bool vacant(const Move&) const;
  bool vacant(const Coord&) const;
  void print();


 private:
  int board_[NUM_ROWS][NUM_COLUMNS];
  int rowCt_[NUM_ROWS];
 
};

template<>
struct std::hash<SimplePieceInfo> {
  std::size_t operator()(const SimplePieceInfo&) const;
};

class SimplePieceInfo {
  friend SimpleBoard;
 public:
 
  // the only way to get this is by calling getPiece()
  bool canRotate(RotateDirection) const;
  SimplePieceInfo rotate(RotateDirection) const;
  bool canMove(MoveDirection) const;
  SimplePieceInfo move(MoveDirection) const;
  Move getPosition() const;
  void print() const;

  friend bool operator==(const SimplePieceInfo& p1, const SimplePieceInfo& p2) {
    return p1.position_.coords_ == p2.position_.coords_;
  };
  friend void simplePieceSanity(const SimplePieceInfo &p);

  friend size_t std::hash<SimplePieceInfo>::operator ()(const SimplePieceInfo&) const;

 private:
  SimplePieceInfo(const SimpleBoard &board, const Move &move, BlockType blockType) : board_(&board), position_(move), blockType_(blockType) {}
  const SimpleBoard* board_;
  Move position_;
  BlockType blockType_;
  int rotateState_ = 0;
};
