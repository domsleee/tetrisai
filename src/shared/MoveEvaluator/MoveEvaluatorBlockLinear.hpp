//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>

class MoveEvaluatorBlockLinear: public IEvaluator {
 public:
  static const int NUM_FACTORS = MoveEvaluatorTetrisReady::NUM_FACTORS + 2;
  static const int LINEAR_A = MoveEvaluatorTetrisReady::NUM_FACTORS;
  static const int LINEAR_B = MoveEvaluatorTetrisReady::NUM_FACTORS + 1;
   
  MoveEvaluatorBlockLinear(const Weighting &w): me_{w}, w_{w} {
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluate(const BitBoard b, const BitPieceInfo p) const {
    BitBoard b2 = b;
    b2.applyPieceInfo(p);
    auto colHeights = getColHeights(b2);
    auto eval = me_.evaluateGivenColHeights(b, p, colHeights.data());
    return eval + evaluateMineGivenColHeights(b, p, colHeights.data());
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p) const override {
    BitBoard b2 = b;
    b2.applyPieceInfo(p);
    auto colHeights = getColHeights(b2);
    return evaluateMineGivenColHeights(b, p, colHeights.data());
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const BitPieceInfo &p, int *colHeights) const {
    double eval = 0;
    auto [valid, minBlock] = getMinBlock(colHeights);
    if (valid) eval += w_[LINEAR_A] * minBlock + w_[LINEAR_B];
    return eval;
  }

 private:
  const MoveEvaluatorTetrisReady me_;
  const Weighting w_;
};
