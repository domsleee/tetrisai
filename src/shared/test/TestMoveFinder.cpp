
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include "src/shared/test/MoveEvaluatorUtility.hpp"


SCENARIO("I-PIECE can clear 12 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(12);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        MoveFinder mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
      }
    }
  }
}

SCENARIO("I-PIECE can't clear 13 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(13);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is NOT available") {
        //printBoard(b);
        MoveFinder mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) == moves.end());
      }
    }
  }
}

SCENARIO("O_PIECE location") {
  GIVEN("a board with an O_PIECE on it") {
    std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
    vs[18][8] = vs[18][9] = vs[19][8] = vs[19][9] = 1;
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("a move that covers the covered squares isn't in the list") {
        MoveFinder mf;
        auto moves = mf.findAllMoves(b, BlockType::O_PIECE);
        auto check = b.getPiece(BlockType::O_PIECE, {{{18, 8}, {18, 9}, {19,8}, {19,9}}});
        REQUIRE(std::find(moves.begin(), moves.end(), check) == moves.end());
      }
    }
  }
}


