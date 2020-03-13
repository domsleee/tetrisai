
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorWellLoc.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

SCENARIO("Edge loc cases") {
  Weighting w(10, 5);
  w[0] = 2;
  w[1] = 3;
  MoveEvaluatorWellLoc me(w);
  GIVEN("wells on the left edge") {
    REQ_DELTA(2, evaluateWithEmptyPiece(me, BitBoard(getWell(3, 0))));
    REQ_DELTA(3, evaluateWithEmptyPiece(me, BitBoard(getWell(3, 1))));
    for (int c = 2; c < NUM_COLUMNS; ++c) {
      REQ_DELTA(5, evaluateWithEmptyPiece(me, BitBoard(getWell(3, c))));
    }
  }
  AND_GIVEN("small wells") {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      auto well = getWell(2, c);
      REQ_DELTA(0, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  }
}
