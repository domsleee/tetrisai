
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder.h"
#include <vector>
#include <iostream>
#include <algorithm>


SCENARIO("I-PIECE all the way to the end baby") {
  GIVEN("a board with a slightly open left side") {
    std::vector<std::vector<int>> vs;
    for (int i = 0; i < 16; i++) {
      vs.push_back(std::vector<int>(10, 0));
    }
    std::vector<int> leftFree = std::vector<int>(10, 1);
    leftFree[0] = 0;
    for (int i = 0; i < 4; i++) {
      vs.push_back(leftFree);
    }

    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        MoveFinder mf;
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        printf("num moves: %lu\n", moves.size());

        auto exp = b.getPiece(BlockType::O_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        for (auto move: moves) {
          move.print();
        }
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
        REQUIRE(moves.size() == 17);
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


