#pragma once
#include "src/board/SimpleBoard.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"

#include <unordered_set>

class OptAllMoveFinder {
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& board, BlockType blockType) {
    auto pieceInfo = board.getPiece(blockType);
    auto height = board.getPileHeight();
    int canMoveDown = NUM_ROWS - height - 3;
    while (canMoveDown > 0) {
      pieceInfo = pieceInfo.move(MoveDirection::DOWN);
      canMoveDown--;
    }

    seen_.insert(pieceInfo.getId());
    dp(pieceInfo);
    std::vector<BitPieceInfo> ret;
    for (auto id: moves_) ret.push_back(board.getPieceFromId(id));
    return ret;
  }

  void dp(const BitPieceInfo &p) {
    for (auto moveDirection: validMoveDirections) {
      if (p.canMove(moveDirection)) {
        auto nxPiece = p.move(moveDirection);
        if (!seen_.count(nxPiece.getId())) {
          seen_.insert(nxPiece.getId());
          dp(nxPiece);
        }
      }
    }
    for (auto rotateDirection: allRotateDirections) {
      if (p.canRotate(rotateDirection)) {
        auto nxPiece = p.rotate(rotateDirection);
        if (!seen_.count(nxPiece.getId())) {
          seen_.insert(nxPiece.getId());
          dp(nxPiece);
        }
      }
    }
    if (!p.canMove(MoveDirection::DOWN)) {
      moves_.insert(p.getId());
    }
  }
 private:
  std::unordered_set<int> seen_;
  std::unordered_set<int> moves_;
};
