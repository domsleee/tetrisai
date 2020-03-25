
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorEdgeWell.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

SCENARIO("Edge well cases") {
  MoveEvaluatorEdgeWell me({2});
  GIVEN("wells on the edge") {
    auto well1 = getWell(3, 0);
    auto well2 = getWell(3, 9);

    WHEN("we evaluate") {
      THEN("we expected the answer to be 2") {
        REQ_DELTA(2, evaluateWithEmptyPiece(me, BitBoard(well1)));
        REQ_DELTA(2, evaluateWithEmptyPiece(me, BitBoard(well2)));
      }
    }
  }
  AND_GIVEN("wells not on the edge") {
    for (int c = 1; c <= 8; ++c) {
      auto well = getWell(3, c);
      REQ_DELTA(-2, evaluateWithEmptyPiece(me, BitBoard(well)));
    }
  }
  AND_GIVEN("small wells") {
    for (int c = 0; c < 9; ++c) {
      auto well = getWell(2, c);
      REQ_DELTA(0, evaluateWithEmptyPiece(me, BitBoard(well)));
    }
  }
}

SCENARIO("well after tetris is played") {
  MoveEvaluatorEdgeWell me({2});
  BitBoard b(getWell(4, 0));
  auto p = b.getPiece(Move({{16, 0}, {17, 0}, {18, 0}, {19, 0}}));
  auto clearBoard = me.EVAL(b, p, 19);
  REQ_DELTA(0, clearBoard);
}
