#pragma once
#include "src/board/SimpleBoard.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"

#include <unordered_set>
#include <iostream>

template <typename MyBoard=BitBoard, typename MyBoardPieceInfo=BitPieceInfo>
class AllMoveFinder {
 public:
  AllMoveFinder() {}
  AllMoveFinder(int level) {}

  void setMaxDropRem(int v) {}
  void setFirstMoveDirectionChar(char c) {}
  void setOffsetHeight(int offsetHeight) { offsetHeight_ = offsetHeight; }

  std::vector<BitPieceInfo> findAllMoves(const MyBoard& board, BlockType blockType) {
    moves_.clear();
    auto pieceInfo = board.getPiece(blockType);
    auto height = board.getPileHeight();
    int canMoveDown = NUM_ROWS - height - offsetHeight_; // needs to be 3
    while (canMoveDown > 0) {
      pieceInfo = pieceInfo.move(MoveDirection::DOWN);
      canMoveDown--;
    }
  
    seen_ = std::vector<bool>(BitBoardPre::NUM_INDEXES, false);
    seen_[pieceInfo.getId()] = true;
    dp(pieceInfo);
    return {moves_.begin(), moves_.end()};
  }

  void dp(const MyBoardPieceInfo &p) {
    for (auto moveDirection: validMoveDirections) {
      if (p.canMove(moveDirection)) {
        auto nxPiece = p.move(moveDirection);
        if (!seen_[nxPiece.getId()]) {
          seen_[nxPiece.getId()] = true;
          dp(nxPiece);
        }
      }
    }
    // for (auto rotateDirection: allRotateDirections) {
    //   if (p.canRotate(rotateDirection)) {
    //     auto nxPiece = p.rotate(rotateDirection);
    //     if (!seen_.count(nxPiece)) {
    //       seen_.insert(nxPiece);
    //       dp(nxPiece);
    //     }
    //   }
    // }
    if (!p.canMove(MoveDirection::DOWN)) {
      moves_.insert(p);
    }
  }
 private:
  int offsetHeight_ = 3;
  std::vector<bool> seen_;
  std::unordered_set<MyBoardPieceInfo> moves_;
};
