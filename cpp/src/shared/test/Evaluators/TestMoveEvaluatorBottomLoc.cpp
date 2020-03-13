
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBottomLoc.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

SCENARIO("Edge loc cases") {
  Weighting w(10, 0);
  for (int c = 0; c < NUM_COLUMNS; ++c) w[c] = c;
  MoveEvaluatorBottomLoc me(w);
  GIVEN("big wells") {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      REQ_DELTA(c, evaluateWithEmptyPiece(me, BitBoard(getWell(3, c))));
    }
  }
  AND_GIVEN("small wells") {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      auto well = getWell(2, c);
      REQ_DELTA(c, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  }
}
