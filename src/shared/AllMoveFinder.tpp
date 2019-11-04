#include "src/board/SimpleBoard.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"

#include <unordered_set>
#include <unordered_map>
#include <iostream>

template <typename MyBoard, typename MyBoardPieceInfo>
using MyT = std::vector<std::unordered_map<MyBoard, std::vector<MyBoardPieceInfo>>>;

template <typename MyBoard=BitBoard, typename MyBoardPieceInfo=BitPieceInfo>
class AllMoveFinder {
 public:
  static MyT<MyBoard, MyBoardPieceInfo> glob_map_;
  AllMoveFinder() {}
  std::vector<BitPieceInfo> findAllMoves(const MyBoard& board, BlockType blockType) {
    if (glob_map_[blockType].count(board)) {
      return glob_map_[blockType][board];
    }
  
    auto pieceInfo = board.getPiece(blockType);
    auto height = board.getPileHeight();
    //printf("finding all moves... (%d)\n", height);
    //board.print();
    int canMoveDown = NUM_ROWS - height - 3;
    //printf("canMoveDown: %d\n", canMoveDown);
    while (canMoveDown > 0) {
      pieceInfo = pieceInfo.move(MoveDirection::DOWN);
      canMoveDown--;
    }

    seen_.insert(pieceInfo);
    dp(pieceInfo);
    return glob_map_[blockType][board] = {moves_.begin(), moves_.end()};
  }

  void dp(const MyBoardPieceInfo &p) {
    for (auto moveDirection: validMoveDirections) {
      if (p.canMove(moveDirection)) {
        auto nxPiece = p.move(moveDirection);
        if (!seen_.count(nxPiece)) {
          seen_.insert(nxPiece);
          dp(nxPiece);
        }
      }
    }
    for (auto rotateDirection: allRotateDirections) {
      if (p.canRotate(rotateDirection)) {
        auto nxPiece = p.rotate(rotateDirection);
        if (!seen_.count(nxPiece)) {
          seen_.insert(nxPiece);
          dp(nxPiece);
        }
      }
    }
    if (!p.canMove(MoveDirection::DOWN)) {
      moves_.insert(p);
    }
  }
 private:
  std::unordered_set<MyBoardPieceInfo> seen_;
  std::unordered_set<MyBoardPieceInfo> moves_;
};


template <typename MyBoard, typename MyBoardPieceInfo>
MyT<MyBoard, MyBoardPieceInfo> AllMoveFinder<MyBoard, MyBoardPieceInfo>::glob_map_(static_cast<size_t>(NUM_BLOCK_TYPES));
