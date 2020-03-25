//
// Created by Dom on 2019-11-25.
//

#pragma once
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
  static const int NUM_FACTORS = 2;
  static const int LINEAR_A = 0;
  static const int LINEAR_B = 1;
   
  MoveEvaluatorBlockLinear(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return evaluateMineGivenColHeights(evaluatorInfo.getAppliedBoard(), evaluatorInfo.getMyColHeights(), evaluatorInfo.getMaxDropRem());
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const int *colHeights, int maxDropRem) const {
    double eval = 0;
    auto [valid, minBlock] = getMinBlock(colHeights, maxDropRem);
    if (valid) eval += w_[LINEAR_A] * minBlock + w_[LINEAR_B];
    return eval;
  }

 private:
  const Weighting w_;
};
