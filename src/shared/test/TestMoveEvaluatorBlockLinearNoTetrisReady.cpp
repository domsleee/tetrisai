
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinear.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinearNoTetrisReady.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

#define REQ_DELTA(a, b) REQUIRE_THAT(a, Catch::WithinAbs(b, 0.01f));



SCENARIO("is nearly blocking") {
  GIVEN("a tetris ready board") {
    // cover UP left
    const std::vector<std::string> board = {
      "0111111111",
      "0111111111",
      "0111111111",
      "0111111111",
      "0111111111",
      "0111111111",
      "0111111111",
      "0111111111"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorBlockLinearNoTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("tetris_ready is zero (it uses block linear)") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
      }
    }
  }
  AND_GIVEN("a large board") {
    GIVEN("a tetris ready board") {
    const auto b = leftWell(18);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorBlockLinear>(b);
    for (int i = 0; i < w.size(); ++i) {
      printf("%d: %d\n", i, w[i]);
    }


    WHEN("we evaluate") {
      THEN("LIENAR_B is 1") {
        REQUIRE(w[MoveEvaluatorBlockLinear::LINEAR_B-1] == 1);
      }
    }
  }
  }
}
