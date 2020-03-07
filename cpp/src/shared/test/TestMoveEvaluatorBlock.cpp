
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlock.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>


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
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorBlock>(b);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQ_DELTA(w[MoveEvaluatorBlock::IS_MORE_THAN_FOUR_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_FOUR_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_THREE_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_TWO_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_ONE_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_ABOUT_TO_BLOCK], 1);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_ONE], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_TWO], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_MORE_THAN_TWO], 0);
      }
    }
  }
  AND_GIVEN("a tetris ready board") {
    // cover UP RIGHT
    const std::vector<std::string> board = { // 9 rows
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110"
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorBlock>(b);

    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQ_DELTA(w[MoveEvaluatorBlock::IS_MORE_THAN_FOUR_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_FOUR_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_THREE_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_TWO_AWAY_FROM_BLOCK], 1);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_ONE_AWAY_FROM_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_ABOUT_TO_BLOCK], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_ONE], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_TWO], 0);
        REQ_DELTA(w[MoveEvaluatorBlock::IS_BLOCK_BY_MORE_THAN_TWO], 0);
      }
    }
  }
}
