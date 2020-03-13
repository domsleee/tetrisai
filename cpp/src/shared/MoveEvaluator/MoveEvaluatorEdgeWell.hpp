//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

#include "src/common/common.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

class MoveEvaluatorEdgeWell: public IEvaluator {
 public:
  static const int NUM_FACTORS = 1;
  static const int EDGE_WELL = 0;
   
  MoveEvaluatorEdgeWell(const Weighting &w): w_{w} {
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
    if (bottomColumn == 0 || bottomColumn == 9) return w_[EDGE_WELL];
    return -w_[EDGE_WELL];
  }

 private:
  const Weighting w_;
};
