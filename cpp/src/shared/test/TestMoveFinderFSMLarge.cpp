#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include <iostream>
#include "catch.h"

const int SKIP_AMOUNT = 100; // one in every n

SCENARIO("FSM large data") {
  using PairT = std::pair<std::string, int>;
  PairT p1 = {TEST_FOLDER + "/FSM19BigData.in", 2};
  PairT p2 = {TEST_FOLDER + "/FSM18BigData.in", 3};
  PairT gp = GENERATE_COPY(Catch::Generators::values({p1, p2}));
  auto m = readFromBigFile(gp.first);
  int ct = 0;
  for (auto [board, p]: m) {
    if (ct++ % SKIP_AMOUNT != 0) continue;
    auto [blockType, expMoveInts] = p;
    auto mf = MoveFinderFSM();
    mf.setMaxDropRem(gp.second);
    auto moves = mf.findAllMoves(board, blockType);
    std::vector<int> moveInts;
    std::transform(moves.begin(), moves.end(), std::back_inserter(moveInts), [](const auto& move) { return move.getId(); });
    std::sort(moveInts.begin(), moveInts.end());
    std::sort(expMoveInts.begin(), expMoveInts.end());
    REQUIRE(moveInts.size() == expMoveInts.size());
    REQUIRE_THAT(moveInts, Catch::Matchers::Equals(expMoveInts));
  }
}