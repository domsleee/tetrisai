#include "src/shared/MoveFinder.h"
#include "src/common/PositionPieceGetter.hpp"
#include "src/board/SimpleBoard.h"
#include <iostream>

const int MAX_ROTATIONS = 5;
const int MAX_RELEASES = 1;

std::vector<Move> MoveFinder::findAllMoves(const SimpleBoard& b, BlockType blockType) {
  // getStartingPosition
  auto pieceInfo = b.getPiece(blockType);
  dp(pieceInfo, KeyStatus::LEFT_DOWN, 0, 16, 0, 0);
  dp(pieceInfo, KeyStatus::RIGHT_DOWN, 0, 16, 0, 0);
  //printf("solution space: %lu\n", seen_.size());
  return {moves_.begin(), moves_.end()};
}

// DP: coord(200), keyStatus(3), numKeyHits(2), das(16)
// currentPos, rotation
void MoveFinder::dp(SimplePieceInfo currentPiece, KeyStatus keyStatus, int numReleases, int das, int numFrames, int numRotations) {
  if (seen_.count({currentPiece, keyStatus, numReleases, das, numFrames, numRotations})) return;
  seen_.insert({currentPiece, keyStatus, numReleases, das, numFrames, numRotations});

  if (numFrames % 2 == 0) {
    // move down
    if (!currentPiece.canMove(MoveDirection::DOWN)) {
      moves_.insert(currentPiece.getPosition());
      return;
    }
    currentPiece = currentPiece.move(MoveDirection::DOWN);
  }

  // options:
  // - release the key
  // - keep holding the key
  // - rotate (independently)
  if ((keyStatus == KeyStatus::LEFT_DOWN || keyStatus == KeyStatus::RIGHT_DOWN) && numReleases < MAX_RELEASES) {
    dp(currentPiece, KeyStatus::NEITHER, numReleases+1, 0, numFrames, numRotations);
  }

  if (numRotations < MAX_ROTATIONS && currentPiece.canRotate(RotateDirection::ROTATE_AC)) {
    dp(currentPiece.rotate(RotateDirection::ROTATE_AC), keyStatus, numReleases, das, numFrames, numRotations+1);
  }
  if (numRotations < MAX_ROTATIONS && currentPiece.canRotate(RotateDirection::ROTATE_C)) {
    dp(currentPiece.rotate(RotateDirection::ROTATE_C), keyStatus, numReleases, das, numFrames, numRotations+1);
  }


  switch(keyStatus) {
    case KeyStatus::LEFT_DOWN: 
    case KeyStatus::RIGHT_DOWN: {
      das++;
      if (das >= 16) {
        auto direction = keyStatus == KeyStatus::LEFT_DOWN ? MoveDirection::LEFT : MoveDirection::RIGHT;
        if (!currentPiece.canMove(direction)) das = 15;
        else { currentPiece = currentPiece.move(direction); das = 10; }
      }
    } break;
    default: break;
  }
  dp(currentPiece, keyStatus, numReleases, das, numFrames+1, numRotations);
}