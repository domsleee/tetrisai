
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorWellVeryLeft.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

SCENARIO("Edge well cases") {
  MoveEvaluatorWellVeryLeft me({2});
  GIVEN("wells on the left edge") {
    for (int c = 0; c <= 2; ++c) {
      auto well = getWell(3, c);
      REQ_DELTA(2, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  } AND_GIVEN("wells to the right") {
    for (int c = 3; c < NUM_COLUMNS; ++c) {
      auto well = getWell(3, c);
      REQ_DELTA(-2, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  }
  AND_GIVEN("small wells to the right") {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      auto well = getWell(2, c);
      REQ_DELTA(0, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  }
}
