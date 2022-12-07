
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
        auto exp = b.getPiece(BlockType::I_PIECE, {{{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}});
        REQUIRE(std::find(moves.begin(), moves.end(), exp) != moves.end());
        auto path = mf.getShortestPath(exp);
        REQUIRE(path.size() > 0);
        REQUIRE("2 LEFT\n" == path[0]);
      }
      AND_WHEN("we look at the path") {
        auto exp = b.getPiece(BlockType::I_PIECE, {{{R(3), 0}, {R(2), 0}, {R(1), 0}, {R(0), 0}}});
        auto path = mf.getShortestPathActions(exp);
        auto iPiece = b.getPiece(BlockType::I_PIECE);
        for (auto [frame, action]: path) {
          //printBoardWithPiece(b, iPiece);
          //printf("frame: %d, nextAction: %s\n", frame, toString(action).c_str());
          if (frame == 57) continue;
          iPiece = iPiece.doAction(action);
        }
        REQUIRE(exp == iPiece);
      }
    }
  }
}