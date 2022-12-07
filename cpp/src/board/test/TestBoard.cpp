
#include "catch.h"
#include "src/board/SimpleBoard.h"
#include "src/common/BlockType.hpp"
#include <vector>


SCENARIO("simple moving tests") {
  WHEN("two pieces are put next to each other") {
    SimpleBoard b;
    Move m1 = {{{0,0}, {0,1}, {0,2}, {0,3}}};
    Move m2 = {{{1,0}, {1,1}, {1,2}, {1,3}}};
    Move m3 = {{{2,0}, {2,1}, {2,2}, {2,3}}};
    auto p1 = b.getPiece(BlockType::I_PIECE, m1);
    auto p2 = b.getPiece(BlockType::I_PIECE, m2);
    auto p3 = b.getPiece(BlockType::I_PIECE, m3);
    b.applyPieceInfo(p3);
    THEN("pieces can't move into the board") {
      REQUIRE(p2.canMove(MoveDirection::DOWN) == false);
    }
    AND_THEN("pieces can't move out of bounds") {
      REQUIRE(p1.canMove(MoveDirection::UP) == false);
      REQUIRE(p1.canMove(MoveDirection::LEFT) == false);
      REQUIRE(p1.canMove(MoveDirection::LEFT) == false);
    }
    AND_THEN("pieces can make valid moves") {
      REQUIRE(p1.canMove(MoveDirection::RIGHT));
      REQUIRE(p2.canMove(MoveDirection::RIGHT));
    }
  }
}

SCENARIO("simple rotation tests") {
  GIVEN("there is a piece") {
    SimpleBoard b;
    Move m1 = {{{2,0}, {2,1}, {2,2}, {2,3}}};
    auto p1 = b.getPiece(BlockType::I_PIECE, m1);
    WHEN("we rotate the piece") {
      REQUIRE(p1.canRotate(RotateDirection::ROTATE_AC));
      auto p2 = p1.rotate(RotateDirection::ROTATE_AC);
      THEN("it rotates as expected") {
        std::vector<Coord> exp = {{0, 2}, {1, 2}, {2, 2}, {3, 2}};
        REQUIRE(p2.getPosition().coords_ == exp);
      }
    }
  }
}

SCENARIO("line clears... top 4 rows") {
  GIVEN("there are lines to be cleared") {
    std::vector<std::vector<int>> vs(NUM_ROWS, std::vector<int>(10, 0));
    for (int r = 2; r < 6; r++) {
      for (int c = 0; c < 9; c++) {
        vs[r][c] = 1;
      }
    }
    Move m1 = {{{2, 9}, {3, 9}, {4, 9}, {5, 9}}};
    SimpleBoard b{vs};
    WHEN("we apply some line clear") {
      THEN("we get the correct number") {
        REQUIRE(!b.vacant({2,0}));
        REQUIRE(!b.vacant({3,0}));
        REQUIRE(!b.vacant({2,8}));
        REQUIRE(!b.vacant({3,8}));
        int lineClears = b.applyMove(m1);
        REQUIRE(lineClears == 4);
        REQUIRE(b.vacant({2,0}));
        REQUIRE(b.vacant({3,0}));
        REQUIRE(b.vacant({4,0}));
        REQUIRE(b.vacant({5,0}));
        REQUIRE(b.vacant({2,9}));
        REQUIRE(b.vacant({3,9}));
        REQUIRE(b.vacant({4,9}));
        REQUIRE(b.vacant({5,9}));
      }
    }

  }
}

