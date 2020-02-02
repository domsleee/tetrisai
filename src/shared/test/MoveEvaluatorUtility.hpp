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


const std::string TEST_FOLDER = "/Users/dom/Documents/git/tetrisAI/src/shared/test/data";

BitBoard readBoard(const std::string &filePath);

Weighting getExpectedWeights(const std::string &filepath);


template<typename MyMoveEvaluator>
Weighting getWeights(const BitBoard &b, const BitPieceInfo &piece, const MyMoveEvaluator &me, int num_factors=17) {
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


Weighting getWeights2(const BitBoard &b, const BitPieceInfo &piece);

// todo: fix this up
Weighting getWeightsFromEmptyPiece2(const BitBoard &b);

BitBoard getBoardFromPartialStringVector(std::vector<std::string> strings);

#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])

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
    res[i] = me.evaluate(b, piece);
  }
  return res;
}

template<typename MyMoveEvaluator>
auto getWeightsFromEmptyPieceT(const BitBoard &b) {
  const auto m = BitBoardPre::idToMove(BitBoardPre::getEmptyMoveId());
  auto piece = b.getPiece(m);
  return getWeightsTemp<MyMoveEvaluator>(b, piece);
}

