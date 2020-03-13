//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

#include "src/common/common.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

class MoveEvaluatorWellVeryLeft: public IEvaluator {
 public:
  static const int NUM_FACTORS = 1;
  static const int WELL_VERY_LEFT = 0; // in columns 0,1,2
   
  MoveEvaluatorWellVeryLeft(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    BitBoard b2 = b;
    b2.applyPieceInfo(p);
    auto colHeights = getColHeights(b2);
    return evaluateMineGivenColHeights(b, p, colHeights.data(), evaluatorInfo.level);
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const BitPieceInfo &p, int *colHeights, int level) const {
    auto [bottomColumn, secondColumn] = getMinColumns(colHeights);
    if (!isDeepWell(colHeights[bottomColumn], colHeights[secondColumn])) return 0;
    if (bottomColumn <= 2) return w_[WELL_VERY_LEFT];
    return -w_[WELL_VERY_LEFT];
  }

 private:
  const Weighting w_;
};
