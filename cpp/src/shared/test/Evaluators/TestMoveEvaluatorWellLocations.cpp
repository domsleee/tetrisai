
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorWellLocations.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

SCENARIO("MoveEvaluatorWellLocations") {
  MoveEvaluatorWellLocations me({11, 22});
  GIVEN("wells on the left edge") {
    for (int c = 0; c <= 4; ++c) {
      auto well = getWell(3, c);
      REQ_DELTA(11, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  } AND_GIVEN("wells on the right") {
    for (int c = 5; c < NUM_COLUMNS; ++c) {
      auto well = getWell(3, c);
      REQ_DELTA(22, evaluateWithEmptyPiece(me, BitBoard(well)))
    }
  }
  AND_GIVEN("a well on the left and the right") {
    std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
    for (int r = 16; r < NUM_ROWS; r++) {
      for (int c = 1; c < NUM_COLUMNS-1; ++c) {
        vs[r][c] = 1;
      }
    }
    REQ_DELTA(33, evaluateWithEmptyPiece(me, BitBoard(vs)));
  }
  AND_GIVEN("a small well") {
    REQ_DELTA(0, evaluateWithEmptyPiece(me, BitBoard(getWell(2, 0))));
    REQ_DELTA(0, evaluateWithEmptyPiece(me, BitBoard(getWell(2, 8))));
  }
}
