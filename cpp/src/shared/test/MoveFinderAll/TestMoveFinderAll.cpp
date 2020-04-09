
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderAll.h"
#include <vector>
#include <iostream>
#include <algorithm>

#include "src/shared/test/MoveEvaluatorUtility.hpp"


SCENARIO("I-PIECE has moves on empty board") {
  GIVEN("an empty board") {
    BitBoard b;
    WHEN("we search for moves") {
      MoveFinderAll mf;
      auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
      THEN("there are a few moves") {
        REQUIRE(moves.size() > 0);
      }
      AND_WHEN("we search for the previous path of the first move") {
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
        auto path = mf.getShortestPath(exp);
        REQUIRE(path.size() > 0);
        REQUIRE("2 LEFT\n" == path[0]);
      }
    }
  }
}