#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <iostream>
#include <fstream>


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
Weighting getWeights(const BitBoard &b, const BitPieceInfo &piece, const MyMoveEvaluator &me, int num_factors) {
  Weighting w(num_factors, 0);
  Weighting res(num_factors, 0);
  for (int i = 0; i < num_factors; i++) {
    if (i > 0) w[i-1] = 0;
    w[i] = 1;
    // todo: strange??
    MyMoveEvaluator me;

    res[i] = me.evaluate(b, piece, w);
  }
  return res;
}

template<typename MyMoveEvaluator>
Weighting getWeightsFromEmptyPiece(const BitBoard &b, const MyMoveEvaluator &me) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeights(b, piece, me);
}


Weighting getWeights2(const BitBoard &b, const BitPieceInfo &piece) {
  int num_factors = MoveEvaluatorPenalty::getNumFactors();
  Weighting w(num_factors, 0);
  Weighting res(num_factors, 0);
  for (int i = 0; i < num_factors; i++) {
    if (i > 0) w[i-1] = 0;
    w[i] = 1;
    MoveEvaluatorPenalty me(w);
    res[i] = me.evaluate(b, piece);
  }
  return res;
}

// todo: fix this up
Weighting getWeightsFromEmptyPiece2(const BitBoard &b) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeights2(b, piece);
}


BitBoard getBoardFromPartialStringVector(std::vector<std::string> strings) {
  auto vs = std::vector<std::vector<int>>(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
  int rOffset = NUM_ROWS - strings.size();
  int r = rOffset;
  for (;r < NUM_ROWS; ++r) {
    const auto &s = strings[r-rOffset];
    assert(s.size() <= NUM_COLUMNS);
    for (int c = 0; c < s.size(); ++c) {
      if (s[c] == '1') {
        vs[r][c] = 1;
      }
    }
  }
  return {vs};
}



#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])
