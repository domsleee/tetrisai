
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBurns.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/get_moves_utils.hpp"
#include <vector>
#include <iostream>


SCENARIO("burns are correct") {
  auto me = MoveEvaluatorBurns({1});
  Move position = {{{16, 9}, {17, 9}, {18, 9}, {19, 9}}};
  GIVEN("not tetris ready board") {
    // cover UP left
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
    };
    const auto b = getBoardFromPartialStringVector(board);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQ_DELTA(3, me.evaluateMine(b, b.getPiece(position)));

      }
    }
  }
  AND_GIVEN("a tetris ready board") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
    };
    const auto b = getBoardFromPartialStringVector(board);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQ_DELTA(0, me.evaluateMine(b, b.getPiece(position)));
      }
    }
  }

}
