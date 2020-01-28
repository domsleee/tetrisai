
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>


SCENARIO("tetris ready when its ready") {
  GIVEN("not tetris ready board") {
    // cover UP left
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
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
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 1);
      }
    }
  }
  AND_GIVEN("a tetris ready board ONE row up") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "0101010111"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 1);
      }
    }
  }
  AND_GIVEN("a NOT tetris ready board ONE row up (holes)") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111011110",
        "1111111110",
        "1111111110",
        "0101010111"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
      }
    }
  }
  AND_GIVEN("a NOT tetris ready board ONE row up (hole on TOP)") {
    const std::vector<std::string> board = {
        "1111011110",
        "1111111110",
        "1111111110",
        "1111111110",
        "0101010111"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
      }
    }
  }
  AND_GIVEN("a NOT tetris ready board ONE row up (hole on BOTTOM)") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
        "0111111110",
        "0101010111"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
      }
    }
  }
}
