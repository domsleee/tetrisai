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
  static const int NUM_FACTORS = 2;
  static const int LINEAR_A = 0;
  static const int LINEAR_B = 1;
   
  MoveEvaluatorBlockLinear(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    BitBoard b2 = b;
    b2.applyPieceInfo(p);
    auto colHeights = getColHeights(b2);
    return evaluateMineGivenColHeights(b, p, colHeights.data(), evaluatorInfo.level);
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const BitPieceInfo &p, int *colHeights, int level) const {
    double eval = 0;
    auto [valid, minBlock] = getMinBlock(colHeights, level);
    if (valid) eval += w_[LINEAR_A] * minBlock + w_[LINEAR_B];
    return eval;
  }

 private:
  const Weighting w_;
};
