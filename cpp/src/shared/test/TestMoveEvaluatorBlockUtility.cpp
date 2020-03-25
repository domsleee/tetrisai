
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>


SCENARIO("invalid cases") {
  GIVEN("no well") {
    int columns[NUM_COLUMNS] = {};
    columns[0] = 2;
    REQUIRE(false == getMinBlock(columns).first);
  }
  AND_GIVEN("well is beyond saving") {
    int columns[NUM_COLUMNS] = {};
    for (int i = 0; i < NUM_COLUMNS; ++i) columns[i] = 17;
    columns[1] = 16;
    REQUIRE(false == getMinBlock(columns).first);
  }
}

SCENARIO("valid cases") {
  GIVEN("left-side well, blocked by minBlock: 1") {
    int columns[NUM_COLUMNS] = {};
    for (int i = 0; i < NUM_COLUMNS; ++i) columns[i] = 9;
    columns[0] = 0;
    auto minBlock = getMinBlock(columns);
    REQUIRE(true == minBlock.first);
    REQUIRE(-1 == minBlock.second);
  }
}

SCENARIO("getMaxColHeightsMinusClearHeightsAll") {
  GIVEN("left-side well, blocked by minBlock: 1") {
    int columns[NUM_COLUMNS] = {};
    for (int i = 0; i < NUM_COLUMNS; ++i) columns[i] = 9;
    columns[0] = 0;
    REQUIRE(9 == getMaxColHeightsMinusClearHeightsAll(columns, 1));
    REQUIRE(1 == getMaxColHeightsMinusClearHeightsAll(columns, 2));
    REQUIRE(-3 == getMaxColHeightsMinusClearHeightsAll(columns, 3));
  }
}
