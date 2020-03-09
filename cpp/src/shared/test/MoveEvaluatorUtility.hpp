#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"
#include "src/shared/ScoreManager.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])
#define REQ_DELTA(a, b) REQUIRE_THAT(a, Catch::WithinAbs(b, 0.01f));

BitBoard readBoard(const std::string &filePath);

Weighting getExpectedWeights(const std::string &filepath);

BitBoard getBoardFromPartialStringVector(std::vector<std::string> strings);

std::vector<std::vector<int>> getWell(int height, int column);

std::vector<std::vector<int>> leftWell(int height);

template<typename MyMoveEvaluator>
Weighting getWeightsTemp(const BitBoard &b, const BitPieceInfo &piece) {
  int num_factors = MyMoveEvaluator::NUM_FACTORS;
  Weighting w(num_factors, 0);
  Weighting res(num_factors, 0);
  for (int i = 0; i < num_factors; i++) {
    if (i > 0) w[i-1] = 0;
    w[i] = 1;
    MyMoveEvaluator me(w);
    res[i] = me.evaluateMine(b, piece, EvaluatorInfo(19));
  }
  return res;
}

template<typename MyMoveEvaluator>
auto getWeightsFromEmptyPieceT(const BitBoard &b) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeightsTemp<MyMoveEvaluator>(b, piece);
}

