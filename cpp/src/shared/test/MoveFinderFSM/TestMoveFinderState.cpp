
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>

MoveFinderState setupSample() {
  BitBoard b;
  auto piece = b.getPiece(BlockType::I_PIECE);
  auto state = MoveFinderState(piece, false, 0);
  state.setFsmState(FSMState::RELEASED);
  state.frameEntered_ = 54;
  return state;
}

SCENARIO("equality works correctly") {
  GIVEN("two equivalent instances") {
    auto s1 = setupSample();
    auto s2 = setupSample();
    WHEN("we test equality") {
      THEN("they are equal") {
        REQUIRE(s1 == s2);
      }
    }
    AND_WHEN("we put it in an ordered map and use COUNT") {
      std::unordered_map<MoveFinderState, int> mp;
      mp.insert({s1, 1});
      THEN("we get count() == 1") {
        REQUIRE(mp.count(s2) == 1);
      }
    }
  }

}
