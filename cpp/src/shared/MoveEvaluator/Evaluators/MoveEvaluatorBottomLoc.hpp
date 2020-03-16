#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

#include "src/common/common.hpp"
#include <cassert>
#include <cstring>

class MoveEvaluatorBottomLoc: public IEvaluator {
 public:
  static const int NUM_FACTORS = NUM_COLUMNS;

  MoveEvaluatorBottomLoc(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return evaluateMineGivenColHeights(evaluatorInfo.getAppliedBoard(), p, evaluatorInfo.getMyColHeights());
  }

  double evaluateMineGivenColHeights(const BitBoard b, const BitPieceInfo p, const int *colHeights) const {
    auto [bottomColumn, secondColumn] = getMinColumns(colHeights);
    if (colHeights[bottomColumn] == colHeights[secondColumn]) return 0;
    return w_[bottomColumn];
  }

 private:
  const Weighting w_;
};
