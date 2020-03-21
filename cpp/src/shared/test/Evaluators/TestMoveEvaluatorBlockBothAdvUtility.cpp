#include "catch.h"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorBlockBothAdvUtility.h"
#include <array>

SCENARIO("both sides aren't measured by getNumBlockRows") {
  int colHeights[NUM_COLUMNS];
  for (int i = 0; i < NUM_COLUMNS; ++i) colHeights[i] = 0;
  colHeights[0] = colHeights[NUM_COLUMNS-1] = NUM_ROWS;
  std::array<bool, NUM_COLUMNS> isColAccessible;
  for (int i = 0; i < NUM_COLUMNS; ++i) isColAccessible[i] = true;
  isColAccessible[0] = isColAccessible[NUM_COLUMNS-1] = false;

  WHEN("getNumBlockRows") {
    THEN("there are 0 blocked rows") {
      REQUIRE(0 == getNumBlockedRows(colHeights, isColAccessible));
    }
  }

}
