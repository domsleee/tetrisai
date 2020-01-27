
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/pso/main/get_moves_utils.hpp"
#include <vector>
#include <iostream>
#include <algorithm>


SCENARIO("Metrics line up") {
  const auto testFile = TEST_FOLDER + "/test1.in";
  const auto weightFile = TEST_FOLDER + "/test1.exp";
  auto b = readBoard(testFile);
  auto w = getWeightsFromEmptyPiece(b, MoveEvaluator());
  auto wExp = getExpectedWeights(weightFile);


  REQ_IND(w, wExp, MoveEvaluator::TOTAL_LINES_CLEARED);
  //REQ_IND(w, wExp, MoveEvaluator::TOTAL_LOCK_HEIGHT);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WELL_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_DEEP_WELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HOLES);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WEIGHTED_COLUMN_HOLES);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::MIN_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::MAX_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_TRANSITIONS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_ROW_TRANSITIONS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HEIGHTS);
  REQ_IND(w, wExp, MoveEvaluator::PILE_HEIGHT);
  REQ_IND(w, wExp, MoveEvaluator::COLUMN_HEIGHT_SPREAD);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_SOLID_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WEIGHTED_SOLID_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::COLUMN_HEIGHT_VARIANCE);
  
  //REQUIRE(w == wExp);

}




SCENARIO("Metrics 2 line up") {
  const auto testFile = TEST_FOLDER + "/test2.in";
  const auto weightFile = TEST_FOLDER + "/test2.exp";
  auto b = readBoard(testFile);
  auto w = getWeightsFromEmptyPiece(b, MoveEvaluator());
  auto wExp = getExpectedWeights(weightFile);


  REQ_IND(w, wExp, MoveEvaluator::TOTAL_LINES_CLEARED);
  //REQ_IND(w, wExp, MoveEvaluator::TOTAL_LOCK_HEIGHT);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WELL_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_DEEP_WELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HOLES);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WEIGHTED_COLUMN_HOLES);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::MIN_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::MAX_COLUMN_HOLE_DEPTH);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_TRANSITIONS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_ROW_TRANSITIONS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_COLUMN_HEIGHTS);
  REQ_IND(w, wExp, MoveEvaluator::PILE_HEIGHT);
  REQ_IND(w, wExp, MoveEvaluator::COLUMN_HEIGHT_SPREAD);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_SOLID_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::TOTAL_WEIGHTED_SOLID_CELLS);
  REQ_IND(w, wExp, MoveEvaluator::COLUMN_HEIGHT_VARIANCE);
  
  //REQUIRE(w == wExp);
}

SCENARIO("lock height") {
  auto b = BitBoard();
  Move m = Move({{5, 5}, {5, 6}, {5, 7}, {6,5}});
  auto piece = b.getPiece(m);
  // 19 ==> 0
  // 6 ==> 13
  auto w = getWeights(b, piece, MoveEvaluator());
  REQUIRE(w[MoveEvaluator::TOTAL_LOCK_HEIGHT] == 13);

}

// todo: move to utils
SCENARIO("strange O2 edge case") {
  const auto testFile = TEST_FOLDER + "/test3.in";
  auto b = readBoard(testFile);
  auto me = MoveEvaluatorAdapter(MoveEvaluator(), w1);
  // (18, 8), (19, 7), (19, 8), (19, 9)
  auto move = Move({{18, 8}, {19, 7}, {19, 8}, {19, 9}});
  auto pieceInfo = b.getPiece(move);
  
  auto eval = me.evaluate(b, pieceInfo);
  auto [me2, mf] = getMeMfPair(0);
  auto eval2 = me2.evaluate(b, pieceInfo);
  REQUIRE(eval == eval2);
  REQUIRE_THAT(eval, Catch::WithinAbs(-20.39f, 0.01f));
}