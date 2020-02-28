#pragma once
#include "src/common/RotateDirection.hpp"
#include "src/common/MoveDirection.hpp"
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include <bitset>
#include <unordered_map>

// avoid circular deps
class BitPieceInfo;

namespace BitBoardPre {
  // todo: union of type enums
  enum Action {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_UP,
    ROTATE_AC,
    ROTATE_C
  };

  void precompute();

  int getStartingPieceId(BlockType blockType);

  int getRotate(int id, RotateDirection rotateDirection);

  int getMove(int id, MoveDirection moveDirection);

  const std::bitset<200>& idToBitset(int id);

  const Move& idToMove(int id);

  int getMoveFromId(const Move& move); 

  int getRepIdFromId(const int id);

  int getEmptyMoveId();

  int getMoveHeight(int id);

  const std::vector<int>& getOpenRotN(int id);

  extern std::unordered_map<Move, int> moveToId_;

  // throws exception if there is no path
  const std::vector<Action>& getShortestPath(BitPieceInfo &p1, BitPieceInfo &p2);

  BlockType getBlockTypeFromId(int id);
}
