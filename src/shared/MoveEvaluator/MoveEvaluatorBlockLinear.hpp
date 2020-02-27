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

class MoveEvaluatorBlockLinear {
 public:
  static const int NUM_FACTORS = MoveEvaluatorTetrisReady::NUM_FACTORS + 2;
  static const int LINEAR_A = MoveEvaluatorTetrisReady::NUM_FACTORS;
  static const int LINEAR_B = MoveEvaluatorTetrisReady::NUM_FACTORS + 1;
   
  constexpr int getNumFactors() { return NUM_FACTORS; }

  MoveEvaluatorBlockLinear(const Weighting &w): me_{w}, w_{w} {
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluate(const BitBoard b, const BitPieceInfo p) const {
    auto colHeights = getColHeights(b);
    auto eval = me_.evaluateGivenColHeights(b, p, colHeights.data());

    auto [valid, minBlock] = getMinBlock(colHeights.data());
    if (valid) eval += w_[LINEAR_A] * minBlock + w_[LINEAR_B];
    return eval;
  }

 private:
  const MoveEvaluatorTetrisReady me_;
  const Weighting w_;
};
