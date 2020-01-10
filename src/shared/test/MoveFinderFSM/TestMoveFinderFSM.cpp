
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>

#include "src/shared/test/MoveEvaluatorUtility.hpp"


SCENARIO("I-PIECE has moves on empty board") {
  GIVEN("an empty board") {
    BitBoard b;
    WHEN("we search for moves") {
      MoveFinderFSM mf;
      auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
      THEN("there are a few moves") {
        REQUIRE(moves.size() > 0);
      }
      AND_WHEN("we search for the previous path of the first move") {
        auto piece = moves[0];
        auto path = mf.getShortestPath(piece);
      }
    }
  }
}

SCENARIO("I-PIECE can clear 8 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(8);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        MoveFinderFSM mf;
        mf.setMaxDropRem(2);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        auto it = std::find(moves.begin(), moves.end(), exp);
        REQUIRE(it != moves.end());
        auto strs = mf.getShortestPath(*it);
      }
    }
  }
}

/**
Currently, we can't clear 9 high because of the move cooldown.
This can be improved if we can trust that das is loaded
*/
SCENARIO("I-PIECE can't clear 9 high") {
  GIVEN("a board with a slightly open left side") {
    auto vs = leftWell(9);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move is available") {
        //printBoard(b);
        MoveFinderFSM mf;
        mf.setMaxDropRem(2);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        auto it = std::find(moves.begin(), moves.end(), exp);
        REQUIRE(it == moves.end());
        //auto strs = mf.getShortestPath(*it);
        //for (auto st: strs) std::cout << st;
      }
    }
  }
}
