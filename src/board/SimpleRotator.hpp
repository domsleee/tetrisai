
#pragma once
#include "src/board/SimpleBoard.h"
#include <vector>
#include "src/common/Coord.hpp"
#include "src/common/Move.hpp"
#include "src/board/generated.hpp"
#include <cassert>


struct SimpleRotator {
  static Move getRotatePosition(Move position, BlockType blockType, int rotateState, RotateDirection rotateDirection) {
    auto tl = position.coords_[0];
    int newRotateState = rotateState + (rotateDirection == RotateDirection::ROTATE_C ? 1 : -1);
    if (newRotateState == 4) newRotateState = 0;
    else if (newRotateState < 0) newRotateState = 3;
    auto offset1 = getOffset(blockType, rotateState);
    auto offset2 = getOffset(blockType, newRotateState);
    auto tl2 = (tl-offset1)+offset2;
    auto nextMove = getPointsFromRotateState(blockType, newRotateState);
    for (auto &coord: nextMove.coords_) {
      coord = tl2 + (coord-offset2);
    }
    return nextMove;
  }

  static Coord getOffset(BlockType blockType, int rotateState) {
    return getPointsFromRotateState(blockType, rotateState).coords_[0];
  }

  static Move getPointsFromRotateState(BlockType blockType, int rotateState) {
    assert(rotateState >= 0 && rotateState < 4);
    auto blocks = blockToOffsetMap[blockType][rotateState];
    std::vector<Coord> coords;
    for (int r = 0; r < blocks.size(); r++) {
      for (int c = 0; c < blocks[r].size(); c++) {
        if (blocks[r][c]) coords.push_back({r, c});
      }
    }
    return Move(coords);
  }
};
