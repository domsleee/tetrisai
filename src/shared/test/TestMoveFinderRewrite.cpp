
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>


std::vector<std::vector<int>> leftWell(int height) {
  std::vector<std::vector<int>> vs;
  for (int i = 0; i < NUM_ROWS-height; i++) {
    vs.push_back(std::vector<int>(NUM_COLUMNS, 0));
  }
  std::vector<int> leftFree = std::vector<int>(NUM_COLUMNS, 1);
  leftFree[0] = 0;
  for (int i = 0; i < height; i++) {
    vs.push_back(leftFree);
  }
  return vs;
}

SCENARIO("I-PIECE can clear 6 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(6);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        MoveFinderRewrite mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
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
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
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
        MoveFinderRewrite mf;
        auto moves = mf.findAllMoves(b, BlockType::O_PIECE);
        auto check = b.getPiece(BlockType::O_PIECE, {{{18, 8}, {18, 9}, {19,8}, {19,9}}});
        REQUIRE(std::find(moves.begin(), moves.end(), check) == moves.end());

        auto check2 = b.getPiece(BlockType::O_PIECE, {{{18,3}, {18,4}, {19,3}, {19,4}}});
        REQUIRE(std::find(moves.begin(), moves.end(), check2) != moves.end());

      }
    }
  }
}


