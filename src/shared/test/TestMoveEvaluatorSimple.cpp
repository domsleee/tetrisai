
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorSimple.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>


SCENARIO("Metrics line up") {
  const auto testFile = TEST_FOLDER + "/test_simple1.in";
  const auto weightFile = TEST_FOLDER + "/test_simple1.exp";
  auto b = readBoard(testFile);
  auto w = getWeightsFromEmptyPiece(b, MoveEvaluatorSimple());
  auto wExp = getExpectedWeights(weightFile);

  REQ_IND(w, wExp, MoveEvaluatorSimple::AGGREGATE_HEIGHT);
  REQ_IND(w, wExp, MoveEvaluatorSimple::HOLES);
  REQ_IND(w, wExp, MoveEvaluatorSimple::COMPLETE_LINES);
  REQ_IND(w, wExp, MoveEvaluatorSimple::BUMPINESS);
}
