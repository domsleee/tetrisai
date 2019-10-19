
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/AllMoveFinder.tpp"
#include "src/shared/MoveEvaluator.hpp"
#include "src/shared/ScoreManager.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

const std::string TEST_FOLDER = "/Users/dom/Documents/git/tetrisAI/src/shared/test/data";

BitBoard readBoard(const std::string &filePath) {
  std::ifstream fin{filePath};
  auto vs = std::vector<std::vector<int>>(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
  char t;
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLUMNS; c++) {
      fin >> t;
      vs[r][c] = t-'0'; 
    }
  }
  return {vs};
}

Weighting getExpectedWeights(const std::string &filepath) {
  std::ifstream fin{filepath};
  Weighting w;
  double d;
  while (fin >> d) w.push_back(d);
  return w;
}

Weighting getWeights(const BitBoard &b, const BitPieceInfo &piece) {
  printf("NUM_FACTORS: %d\n", NUM_FACTORS);
  Weighting w(NUM_FACTORS, 0);
  Weighting res(NUM_FACTORS, 0);
  for (int i = 0; i < NUM_FACTORS; i++) {
    if (i > 0) w[i-1] = 0;
    w[i] = 1;
    // todo: move this spaghet to a convenience
    MoveEvaluator me;
    ScoreManager sm;
  
    res[i] = me.evaluate(b, piece, w, sm);
  }
  printf("%lu\n", res.size());
  return res;
}

Weighting getWeightsFromEmptyPiece(const BitBoard &b) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeights(b, piece);
}

#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])

SCENARIO("Metrics line up") {
  const auto testFile = TEST_FOLDER + "/test1.in";
  const auto weightFile = TEST_FOLDER + "/test1.exp";
  auto b = readBoard(testFile);
  auto w = getWeightsFromEmptyPiece(b);
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
  printf("maxR: %d\n", piece.getPosition().maxR);
  auto w = getWeights(b, piece);
  REQUIRE(w[MoveEvaluator::TOTAL_LOCK_HEIGHT] == 13);

}