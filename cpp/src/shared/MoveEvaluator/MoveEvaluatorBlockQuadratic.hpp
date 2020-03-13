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

class MoveEvaluatorBlockQuadratic {
 public:
  static const int NUM_FACTORS = MoveEvaluatorTetrisReady::NUM_FACTORS + 3;
  static const int QUADRATIC_A = MoveEvaluatorTetrisReady::NUM_FACTORS;
  static const int QUADRATIC_B = MoveEvaluatorTetrisReady::NUM_FACTORS + 1;
  static const int QUADRATIC_C = MoveEvaluatorTetrisReady::NUM_FACTORS + 2;

  MoveEvaluatorBlockQuadratic(const Weighting &w): w_{w} {
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, int level) const {
    auto [b2, lineClears] = b.applyPieceInfoCopy(p);
    auto colHeights = getColHeights(b2);
    auto [valid, minBlock] = getMinBlock(colHeights.data());
    double eval = 0;
    if (valid) eval += w_[QUADRATIC_A] * minBlock * minBlock + w_[QUADRATIC_B] * minBlock + w_[QUADRATIC_C];
    return eval;
  }

 private:
  const Weighting w_;
};
