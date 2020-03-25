#pragma once

#include "src/shared/MoveFinder/MoveFinderFSM.h"

using FirstMoveConstraintT = char;
const FirstMoveConstraintT NO_CONSTRAINT = '.';
const FirstMoveConstraintT FIRST_MOVE_DIRECTION_NEITHER = 'N';
const FirstMoveConstraintT FIRST_MOVE_DIRECTION_LEFT = 'L';
const FirstMoveConstraintT FIRST_MOVE_DIRECTION_RIGHT = 'R';


template<typename MyMoveFinder>
struct MoveFinderConstraintResolver {
  static FirstMoveConstraintT getConstraint(MyMoveFinder &mf, const BitPieceInfo &chosenPieceInfo) {
    return NO_CONSTRAINT;
  }
};

template<>
struct MoveFinderConstraintResolver<MoveFinderFSM> {
  static FirstMoveConstraintT getConstraint(MoveFinderFSM &mf, const BitPieceInfo &chosenPieceInfo) {
    mf.findAllMoves(chosenPieceInfo.getBoard(), chosenPieceInfo.getBlockType());
    auto actionPairs = mf.getShortestPathActions(chosenPieceInfo);
    if (actionPairs.size() == 0) return NO_CONSTRAINT;
    auto [frameEntered, action] = actionPairs[0];
    switch(action) {
      case Action::LEFT: return FIRST_MOVE_DIRECTION_LEFT;
      case Action::RIGHT: return FIRST_MOVE_DIRECTION_RIGHT;
      default: return FIRST_MOVE_DIRECTION_NEITHER;
    }
  }
};
