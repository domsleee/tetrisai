
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>


SCENARIO("I-PIECE can clear 6 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(6);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        MoveFinderRewrite mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
      }
    }
  }
}

SCENARIO("I-PIECE can clear 7 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(7);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        //printBoard(b);
        MoveFinderRewrite mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
      }
    }
  }
}

SCENARIO("O_PIECE location") {
  GIVEN("a board with an O_PIECE on it") {
    std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
    vs[R(1)][8] = vs[R(1)][9] = vs[R(0)][8] = vs[R(0)][9] = 1;
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("a move that covers the covered squares isn't in the list") {
        MoveFinderRewrite mf;
        auto moves = mf.findAllMoves(b, BlockType::O_PIECE);
        auto check = b.getPiece(BlockType::O_PIECE, {{{R(1), 8}, {R(1), 9}, {R(0),8}, {R(0),9}}});
        REQUIRE(std::find(moves.begin(), moves.end(), check) == moves.end());

        auto check2 = b.getPiece(BlockType::O_PIECE, {{{R(1),3}, {R(1),4}, {R(0),3}, {R(0),4}}});
        REQUIRE(std::find(moves.begin(), moves.end(), check2) != moves.end());

      }
    }
  }
}


