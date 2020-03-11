
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

std::vector<std::string> getLefts(std::vector<std::string> &vs) {
  std::vector<std::string> lefts;
  for (const auto str: vs) {
    if (str.size() > 3 && str[str.size()-3] == 'F') lefts.push_back(str);
  }
  return lefts;
}

SCENARIO("Demo Recording - prefer no quicktap (long)") {
  GIVEN("a board with a 6 high well on the LHS") {
    auto vs = leftWell(6);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move does not involve a quicktap") {
        MoveFinderFSM mf;
        mf.setMaxDropRem(2);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        auto it = std::find(moves.begin(), moves.end(), exp);
        REQUIRE(it != moves.end());
        auto strs = mf.getShortestPath(*it);
        auto leftStrs = getLefts(strs);

        REQUIRE(5 == leftStrs.size());
        REQUIRE("2 LEFT\n" == leftStrs[0]);
        REQUIRE("7 LEFT\n" == leftStrs[1]);
        REQUIRE("13 LEFT\n" == leftStrs[2]);
        REQUIRE("19 LEFT\n" == leftStrs[3]);
        REQUIRE("25 LEFT\n" == leftStrs[4]);
      }
    }
  }
}

SCENARIO("Demo Recording - prefer no quicktap (short)") {
  GIVEN("an empty board") {
    BitBoard b;
    WHEN("we search for moves") {
      THEN("the LHS move does not involve a quicktap") {
        MoveFinderFSM mf;
        mf.setMaxDropRem(2);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{19, 2}, {19, 3}, {19, 4}, {19, 5}}});
        auto it = std::find(moves.begin(), moves.end(), exp);
        REQUIRE(it != moves.end());
        auto strs = mf.getShortestPath(*it);
        auto leftStrs = getLefts(strs);

        REQUIRE(1 == leftStrs.size());
        REQUIRE("2 LEFT\n" == leftStrs[0]);
      }
    }
  }
}



SCENARIO("Demo Recording - prefer early quicktap") {
  GIVEN("a board with a 7 high well on the LHS") {
    auto vs = leftWell(7);
    BitBoard b(vs);
    WHEN("we search for moves") {
      THEN("the LHS move does not involve a quicktap") {
        MoveFinderFSM mf;
        mf.setMaxDropRem(2);
        auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
        auto exp = b.getPiece(BlockType::I_PIECE, {{{16, 0}, {17, 0}, {18, 0}, {19, 0}}});
        auto it = std::find(moves.begin(), moves.end(), exp);
        REQUIRE(it != moves.end());
        auto strs = mf.getShortestPath(*it);
        auto leftStrs = getLefts(strs);
        REQUIRE(5 == leftStrs.size());
        REQUIRE("2 LEFT\n" == leftStrs[0]);
        REQUIRE("7 LEFT\n" == leftStrs[1]);
        REQUIRE("13 LEFT\n" == leftStrs[2]);
        REQUIRE("19 LEFT\n" == leftStrs[3]);
        REQUIRE("23 LEFT\n" != leftStrs[4]);
      }
    }
  }
}



SCENARIO("reee") {
  BitBoard b("00000000000000000000000001100000000100000001111000000011111011001111111111111110110111111011111111101111111110111111111011111111101111111110111111111011111111101111111110111111111011111111101111111110");
  MoveFinderFSM mf;
  auto moves = mf.findAllMoves(b, BlockType::Z_PIECE);
  REQUIRE(moves.size() != 8);
}
