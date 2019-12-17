
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/common/BlockType.hpp"
#include <vector>


SCENARIO("vacancy check") {
  WHEN("we try to place a block on top of another block") {
    BitBoard b;
    auto p = b.getPiece(BlockType::I_PIECE);
    b.applyPieceInfo(p);
    auto p2 = b.getPiece(BlockType::I_PIECE);
    THEN("it is not vacant") {
      REQUIRE(!b.vacant(p2));
    }
    AND_THEN("pieces cant move out of bounds") {
      REQUIRE(!p2.canMove(MoveDirection::UP));
    }
  }
  AND_WHEN("we try moving the piece down out of bounds") {
    BitBoard b;
    Move m1 = {{{19,0}, {19,1}, {19,2}, {19,3}}};
    auto p = b.getPiece(BlockType::I_PIECE, m1);
    THEN("we cant") {
      REQUIRE(!p.canMove(MoveDirection::DOWN));
    }
  }
}

SCENARIO("O_PIECE check") {
  GIVEN("a board and an O_PIECE") {
    BitBoard b;
    auto p = b.getPiece(BlockType::O_PIECE);
    WHEN("we try to go down") {
      THEN("we can") {
        REQUIRE(p.canMove(MoveDirection::DOWN));
        REQUIRE(b.vacant(p));
      }
    }
  }
}

SCENARIO("simple rotation tests") {
  GIVEN("there is a piece") {
    BitBoard b;
    Move m1 = {{{2,0}, {2,1}, {2,2}, {2,3}}};
    auto p1 = b.getPiece(BlockType::I_PIECE, m1);
    WHEN("we rotate the piece") {
      REQUIRE(p1.canRotate(RotateDirection::ROTATE_AC));
      auto p2 = p1.rotate(RotateDirection::ROTATE_AC);
      THEN("it rotates as expected") {
        auto expMove = Move{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}};
        REQUIRE(p2.getPosition() == expMove);
      }
    }
  }
  AND_GIVEN("a horizontal I-PIECE near the top") {
    BitBoard b;
    Move m1 = {{{2,3}, {2,4}, {2,5}, {2,6}}};
    auto p1 = b.getPiece(BlockType::I_PIECE, m1);
    WHEN("we rotate the piece") {
      REQUIRE(p1.canRotate(RotateDirection::ROTATE_AC));
      auto p2 = p1.rotate(RotateDirection::ROTATE_AC);
      THEN("it rotates as expected") {
        auto expMove = Move{{{0,5}, {1,5}, {2,5}, {3,5}}};
        REQUIRE(p2.getPosition() == expMove);
      }
    }
  }
}

SCENARIO("repeated rotate test") {
  for (auto blockType: allBlockTypes) {
    BitBoard b;
    auto piece = b.getPiece(blockType);
    for (int i = 0; i < 3; ++i) piece = piece.move(MoveDirection::DOWN);

    auto rotPieceAC = piece;
    auto rotPieceC = piece;
    for (int i = 0; i < 4; ++i) rotPieceAC = rotPieceAC.rotate(RotateDirection::ROTATE_AC);
    for (int i = 0; i < 4; ++i) rotPieceC = rotPieceC.rotate(RotateDirection::ROTATE_C);
    REQUIRE(piece == rotPieceAC);
    REQUIRE(piece == rotPieceC);
  }
}

SCENARIO("friends check") {
  GIVEN("some BitPieces") {
    BitBoard b;
    auto p1 = b.getPiece(BlockType::I_PIECE);
    auto p2 = b.getPiece(BlockType::I_PIECE);
    auto p3 = b.getPiece(BlockType::O_PIECE);
    auto p4 = b.getPiece(BlockType::O_PIECE);
    WHEN("we check equality") {
      THEN("they are equal") {
        REQUIRE(p1 == p2);
        REQUIRE(p3 == p4);
      }
    }
  }
}

SCENARIO("line clears... top 4 rows") {
  GIVEN("there are lines to be cleared") {
    std::vector<std::vector<int>> vs(20, std::vector<int>(10, 0));
    for (int r = 1; r < 5; r++) {
      for (int c = 0; c < 9; c++) {
        vs[r][c] = 1;
      }
    }
    Move m1 = {{{1, 9}, {2, 9}, {3, 9}, {4, 9}}};
    Move m2 = {{{0,0}, {0,1}, {0,2}, {0,3}}};
    Move m3 = {{{15,0}, {15,1}, {15,2}, {15,3}}};
    BitBoard b(vs);
    b.applyMove(m2);
    b.applyMove(m3);

    WHEN("we apply some line clear") {
      THEN("we get the correct number") {
        REQUIRE(!b.vacant(Coord(1,0)));
        REQUIRE(!b.vacant(Coord(1,8)));
        int lineClears = b.applyMove(m1);
        REQUIRE(lineClears == 4);
        REQUIRE(b.vacant(Coord(0,0)));
        REQUIRE(b.vacant(Coord(1,0)));
        REQUIRE(b.vacant(Coord(2,0)));
        REQUIRE(b.vacant(Coord(3,0)));
        REQUIRE(b.vacant(Coord(0,9)));
        REQUIRE(b.vacant(Coord(1,9)));
        REQUIRE(b.vacant(Coord(2,9)));
        REQUIRE(b.vacant(Coord(3,9)));


        Move exp1 = {{{4,0}, {4,1}, {4,2}, {4,3}}};
        Move exp2 = m3;
        BitBoard b2;
        b2.applyMove(exp1);
        b2.applyMove(exp2);
        REQUIRE(b == b2);
      }
    }

  }
}

SCENARIO("Getting height") {
  std::vector<std::vector<int>> vs(20, std::vector<int>(10, 0));
  GIVEN("the bottom row only") {
    vs[19][0] = 1;
    BitBoard b(vs);
    THEN("the pile height is 1") {
      REQUIRE(b.getPileHeight() == 1);
    }
  }
  AND_GIVEN("an empty board") {
    BitBoard b(vs);
    WHEN("we check the height") {
      THEN("it is zero") {
        REQUIRE(b.getPileHeight() == 0);
      }
    }
  }
  AND_GIVEN("a board") {
    vs[5][0] = 1;
    vs[8][0] = 1;
    BitBoard b(vs);
    WHEN("we check the height") {
      THEN("it is correct") {
        REQUIRE(b.getPileHeight() == 15);
      }
    }
  }
  AND_GIVEN("a different board") {
    vs[9][0] = 1;
    vs[12][0] = 1;
    BitBoard b(vs);
    WHEN("we check the height") {
      THEN("it is correct" ){
        REQUIRE(b.getPileHeight() == 11);
      }
    }
  }
}

