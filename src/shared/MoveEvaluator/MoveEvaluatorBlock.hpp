//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>

class MoveEvaluatorBlock {
 public:
  static const int NUM_FACTORS = MoveEvaluatorTetrisReady::NUM_FACTORS + 9;
  static const int IS_MORE_THAN_FOUR_AWAY_FROM_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS;
  static const int IS_FOUR_AWAY_FROM_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS + 1;
  static const int IS_THREE_AWAY_FROM_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS + 2;
  static const int IS_TWO_AWAY_FROM_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS + 3;
  static const int IS_ONE_AWAY_FROM_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS + 4;
  static const int IS_ABOUT_TO_BLOCK = MoveEvaluatorTetrisReady::NUM_FACTORS + 5;
  static const int IS_BLOCK_BY_ONE = MoveEvaluatorTetrisReady::NUM_FACTORS + 6;
  static const int IS_BLOCK_BY_TWO = MoveEvaluatorTetrisReady::NUM_FACTORS + 7;
  static const int IS_BLOCK_BY_MORE_THAN_TWO = MoveEvaluatorTetrisReady::NUM_FACTORS + 8;

  constexpr int getNumFactors() { return NUM_FACTORS; }

  MoveEvaluatorBlock(const Weighting &w): me_{w}, w_{w} {
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    auto eval = me_.evaluate(b, p);
    int* colHeights = me_.getColHeights();

    auto [valid, minBlock] = getMinBlock(colHeights);
    if (!valid) return eval;
    eval += (minBlock > 4) * w_[IS_MORE_THAN_FOUR_AWAY_FROM_BLOCK];
    eval += (minBlock == 4) * w_[IS_FOUR_AWAY_FROM_BLOCK];
    eval += (minBlock == 3) * w_[IS_THREE_AWAY_FROM_BLOCK];
    eval += (minBlock == 2) * w_[IS_TWO_AWAY_FROM_BLOCK];
    eval += (minBlock == 1) * w_[IS_ONE_AWAY_FROM_BLOCK];
    eval += (minBlock == 0) * w_[IS_ABOUT_TO_BLOCK];
    eval += (minBlock == -1) * w_[IS_BLOCK_BY_ONE];
    eval += (minBlock == -2) * w_[IS_BLOCK_BY_TWO];
    eval += (minBlock <  -2) * w_[IS_BLOCK_BY_MORE_THAN_TWO];

    return eval;
  }

 private:
  const MoveEvaluatorTetrisReady me_;
  const Weighting &w_;
};
