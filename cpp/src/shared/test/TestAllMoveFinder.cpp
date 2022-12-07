
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/AllMoveFinder.tpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

SCENARIO("I-PIECE all the way to the end baby") {
  GIVEN("a board with a slightly open left side") {
    std::vector<std::vector<int>> vs;
    for (int i = 0; i < 18; i++) {
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
        AllMoveFinder mf = AllMoveFinder();
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        printf("num moves: %lu\n", moves.size());

        auto exp = b.getPiece(BlockType::O_PIECE, {{{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}});
        for (auto move: moves) {
          move.print();
        }
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
        REQUIRE(moves.size() == 17);
      }
    }
  }
}

