#pragma once
#include "src/board/SimpleBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include <bitset>

namespace BitBoardPre {
  void precompute();

  //inline int pieceInfoToId(const SimplePieceInfo &p);

  //inline int addRotate(int id, int nxId, RotateDirection rotateDirection);

  //inline int addMove(int id, int nxId, MoveDirection moveDirection);

  int getStartingPieceId(BlockType blockType);

  int getRotate(int id, RotateDirection rotateDirection);

  int getMove(int id, MoveDirection moveDirection);

  const std::bitset<200>& idToBitset(int id);

  const Move& idToMove(int id);

  int moveToId(const Move& move); 
}