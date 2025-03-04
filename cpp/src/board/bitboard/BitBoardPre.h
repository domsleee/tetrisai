#pragma once
#include "src/common/RotateDirection.hpp"
#include "src/common/MoveDirection.hpp"
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include "src/common/Action.h"
#include "src/common/common.hpp"
#include <bitset>
#include <unordered_map>

// avoid circular deps
class BitPieceInfo;

namespace BitBoardPre {
  const int MAX_IND = 5000; // NUM_BLOCKS * NUM_ROWS * NUM_COLUMNS * NUM_ROTATIONS
  const int NUM_INDEXES = MAX_IND;  

  void precompute();

  int getStartingPieceId(BlockType blockType);

  int getRotate(int id, RotateDirection rotateDirection);

  int getMove(int id, MoveDirection moveDirection);

  const std::bitset<NUM_ROWS * NUM_COLUMNS>& idToBitset(int id);

  const Move& idToMove(int id);

  int getMoveFromId(const Move& move); 

  int getRepIdFromId(const int id);

  int getEmptyMoveId();

  int getMoveHeight(int id);

  const std::vector<int>& getOpenRotN(int id);

  extern std::unordered_map<Move, int> moveToId_;

  BlockType getBlockTypeFromId(int id);

  int doActionOnEmptyBoard(int id, Action action);


  const std::vector<std::pair<std::vector<Action>, int>> &getActions(bool includeDown);
  const std::vector<Action> &bitfieldToActions(int bitfield);


}
