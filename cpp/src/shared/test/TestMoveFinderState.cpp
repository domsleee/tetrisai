#include "catch.h"
#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"
#include "src/board/bitboard/BitBoard.h"

SCENARIO("fields work properly") {
  MoveFinderState st(BitBoard().getEmptyPiece(), false, 3);
  GIVEN("boolean values") {
    auto b = GENERATE(false, true);
    st.setIsLeftHolding(b);
    REQUIRE(b == st.getIsLeftHolding());
  }
  AND_GIVEN("small values") {
    auto v1 = GENERATE(0, 1, 2);
    auto v2 = GENERATE(0, 1, 2);
    auto v3 = GENERATE(0, 1, 2);
    st.setReleaseCooldown(v1);
    st.setDropRem(v2);
    st.setDasRem(v3);
    REQUIRE(v1 == st.getReleaseCooldown());
    REQUIRE(v2 == st.getDropRem());
    REQUIRE(v3 == st.getDasRem());
  }
  AND_GIVEN("multi-layer values") {
    auto v1 = GENERATE(0, 1, 2);
    auto v2 = GENERATE(0, 1, 2);
    st.setMoveCooldown(MoveDirection::LEFT, v1, true);
    st.setMoveCooldown(MoveDirection::RIGHT, v2, true);
  
    REQUIRE(v1 == st.getMoveCooldown(MoveDirection::LEFT));
    REQUIRE(v2 == st.getMoveCooldown(MoveDirection::RIGHT));
    st.setMoveCooldown(MoveDirection::RIGHT, v1, true);
    st.setMoveCooldown(MoveDirection::LEFT, v2, true);
    REQUIRE(v1 == st.getMoveCooldown(MoveDirection::RIGHT));
    REQUIRE(v2 == st.getMoveCooldown(MoveDirection::LEFT));
  }
}