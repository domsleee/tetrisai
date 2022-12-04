
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/ScoreManager.hpp"
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
  AND_GIVEN("a tetris ready board because of height") {
    const auto vs = leftWell(8);
    BitBoard b(vs);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 1);
      }
    }
  }
  AND_GIVEN("a NOT tetris ready board because of height") {
    const auto vs = leftWell(9);
    BitBoard b(vs);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 0);
      }
    }
  }
  AND_GIVEN("a tetris ready board because of height") {
    std::vector<std::string> vs;
    const std::string emptyRow = "0000000000";
    const std::string centerRow = "1111101111";
    for (int i = 0; i < 4; ++i) vs.push_back(emptyRow);
    for (int i = 0; i < 16; ++i) vs.push_back(centerRow);
    BitBoard b = getBoardFromPartialStringVector(vs);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    WHEN("we evaluate") {
      THEN("we get the expected value") {
        REQUIRE(w[MoveEvaluatorTetrisReady::TETRIS_READY] == 1);
      }
    }
  }
}

SCENARIO("TetrisReady matches MoveFinderFSM opinion") {
  //REQUIRE(isColAccessible(colHeights, 19) == false);
  for (int level = 18; level <= 19; ++level) {
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      auto move = Move({{R(3), c}, {R(2), c}, {R(1), c}, {R(0), c}});
      auto piece = BitBoard().getPiece(move);
      for (int height = 4; height < NUM_ROWS-2; ++height) {
        BitBoard b = {getWell(height, c)};
        auto tetrisReady = MoveEvaluatorTetrisReady({1}).EVAL(b, b.getEmptyPiece(), level);
        MoveFinderFSM mf;
        mf.setMaxDropRem(level == 19 ? 2 : 3);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto moveFinder = std::find(moves.begin(), moves.end(), piece) != moves.end() ? 1.0 : 0.0;
        // printf("level: %d, c: %d, height: %d, tetrisReady: %d, moveFinder: %d\n", level, c, height, tetrisReady, moveFinder);
        if (fabs(tetrisReady - moveFinder) > 0.01f) {
          printf("level: %d\n", level);
          printBoard(b);
          assert(false);
        }
        REQ_DELTA(tetrisReady, moveFinder);
      }
    }
  }
}

SCENARIO("TetrisReady works AFTER the piece is applied") {
  GIVEN("a clearing piece") {
    BitBoard b = {getWell(4, 0)};
    auto p = b.getPiece(Move({{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}));
    WHEN("the board is cleared") {
      auto clearBoard = MoveEvaluatorTetrisReady({1}).EVAL(b, p, 19);
      auto tetrisReadyBoard = MoveEvaluatorTetrisReady({1}).EVAL(b, b.getEmptyPiece(), 19);
      REQ_DELTA(0, clearBoard);
      REQ_DELTA(1, tetrisReadyBoard);
    }
  }
  AND_GIVEN("a piece that makes the board tetris ready") {
    std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
    for (int r = R(3); r <= R(0); ++r) {
      for (int c = 2; c < NUM_COLUMNS; ++c) vs[r][c] = 1;
    }
    BitBoard b = {vs};
    auto p = b.getPiece(Move({{R(3), 1}, {R(2), 1}, {R(1), 1}, {R(0), 1}}));
    WHEN("the piece is applied") {
      auto notTetrisREady = MoveEvaluatorTetrisReady({1}).EVAL(b, b.getEmptyPiece(), 19);
      auto tetrisReadyBoard = MoveEvaluatorTetrisReady({1}).EVAL(b, p, 19);
      REQ_DELTA(0, notTetrisREady);
      REQ_DELTA(1, tetrisReadyBoard);
    }
  }
}
