#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/common/PositionPieceGetter.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/BoardPrinter.tpp"
#include <iostream>

const int MAX_ROTATIONS = 5;
const int MAX_RELEASES = 1;

std::vector<BitPieceInfo> MoveFinder::findAllMoves(const BitBoard& b, BlockType blockType) const {
  // getStartingPosition
  seen_.clear();
  moves_.clear();
  auto pieceInfo = b.getPiece(blockType);
  b_ = &b;

  dp(pieceInfo, KeyStatus::LEFT_DOWN);
  dp(pieceInfo, KeyStatus::RIGHT_DOWN);

  //printf("solution space: %lu\n", seen_.size());
  return {moves_.begin(), moves_.end()};
}

// DP: coord(200), keyStatus(3), numKeyHits(2), das(16)
// currentPos, rotation
void MoveFinder::dp(BitPieceInfo currentPiece, KeyStatus keyStatus, int numReleases, int das, int numFrames, int numRotations) const {
  if (seen_.count({currentPiece, keyStatus, numReleases, das, numFrames, numRotations})) return;
  seen_.insert({currentPiece, keyStatus, numReleases, das, numFrames, numRotations});

  //printBoardWithPiece(*b_, currentPiece);

  if (numRotations != MAX_ROTATIONS) {
    for (auto rotateDirection: allRotateDirections) {
      if (!currentPiece.canRotate(rotateDirection)) continue;
      auto nxPiece = currentPiece.rotate(rotateDirection);
      bool considerRotate = false;
      for (auto moveDirection: validMoveDirections) {
        if (nxPiece.canMove(moveDirection) != currentPiece.canMove(moveDirection)) {
          considerRotate = true;
          break;
        }
      }
      if (considerRotate) {
        dp(nxPiece, keyStatus, numReleases, das, numFrames, numRotations+1);
      }
    }
  }

  if ((keyStatus != KeyStatus::NEITHER) && numReleases < MAX_RELEASES) {
    dp(currentPiece, KeyStatus::NEITHER, numReleases+1, 0, numFrames, numRotations);
  }

  switch(keyStatus) {
    case KeyStatus::LEFT_DOWN: 
    case KeyStatus::RIGHT_DOWN: {
      das = std::min(das+1, 16);
      if (das == 16) {
        auto direction = keyStatus == KeyStatus::LEFT_DOWN ? MoveDirection::LEFT : MoveDirection::RIGHT;
        if (currentPiece.canMove(direction)) {
          currentPiece = currentPiece.move(direction);
          das = 10;
        }
      }
    } break;
    default: break;
  }

  if ((numFrames & 2) == 0) {
    // move down
    if (!currentPiece.canMove(MoveDirection::DOWN)) {
      moves_.insert(currentPiece);
      return;
    }
    //currentPiece = currentPiece.move(MoveDirection::DOWN);
    auto nxPiece = currentPiece.move(MoveDirection::DOWN);
    return dp(nxPiece, keyStatus, numReleases, das, numFrames+1, numRotations);
  }

  dp(currentPiece, keyStatus, numReleases, das, numFrames+1, numRotations);
}