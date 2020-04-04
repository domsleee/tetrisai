#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include <iostream>
#include "catch.h"

SCENARIO("large data") {
  const auto filename = TEST_FOLDER + "/big_data3.in";
  DISABLE_TEST;

  auto m = readFromBigFile(filename);
  for (auto [board, p]: m) {
    auto [blockType, expMoveInts] = p;
    auto mf = MoveFinderRewrite();
    mf.setMaxDropRem(2);
    auto moves = mf.findAllMoves(board, blockType);
    std::vector<int> moveInts;
    std::transform(moves.begin(), moves.end(), std::back_inserter(moveInts), [](const auto& move) { return move.getId(); });
    std::sort(moveInts.begin(), moveInts.end());
    std::sort(expMoveInts.begin(), expMoveInts.end());
    REQUIRE(moveInts.size() == expMoveInts.size());
    REQUIRE_THAT(moveInts, Catch::Matchers::Equals(expMoveInts));
  }
}
