
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
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

template<typename MyMoveEvaluator>
Weighting getWeights(const BitBoard &b, const BitPieceInfo &piece, const MyMoveEvaluator &me, int num_factors=NUM_FACTORS) {
  Weighting w(num_factors, 0);
  Weighting res(num_factors, 0);
  for (int i = 0; i < num_factors; i++) {
    if (i > 0) w[i-1] = 0;
    w[i] = 1;
    MyMoveEvaluator me;
    ScoreManager sm;
  
    res[i] = me.evaluate(b, piece, w, sm);
  }
  return res;
}

template<typename MyMoveEvaluator>
Weighting getWeightsFromEmptyPiece(const BitBoard &b, const MyMoveEvaluator &me) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeights(b, piece, me);
}

#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])
