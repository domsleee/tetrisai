
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>



SCENARIO("move penalty is calculated correctly (2)") {
  GIVEN("some stuff") {
    // cover UP left
    const std::vector<std::string> board = {
        "00111",
        "00111",
        "10111",
        "11111",
        "11111",
        "11111",
        "10111",
        "01111",
        "11111",
        "11111",
    };

    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPiece2(b);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorPenalty::DAS_BLOCKED_LEFT] == 3);
      }
    }
  }
  AND_GIVEN("a different board") {
    const std::vector<std::string> board = {
        "01111",
        "01111",
        "11111",
        "11111",
        "01111",
        "11111",
        "11111",
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPiece2(b);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorPenalty::DAS_BLOCKED_LEFT] == 2);
      }
    }
  }

}
