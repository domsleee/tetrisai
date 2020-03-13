//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/VacancyChecker.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>

class MoveEvaluatorBottomLoc: public IEvaluator {
 public:
  static const int NUM_FACTORS = NUM_COLUMNS;

  MoveEvaluatorBottomLoc(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    BitBoard b2(b);
    b2.applyPieceInfo(p);
    return evaluateMineGivenColHeights(b2, p, getColHeights(b).data(), evaluatorInfo.level);
  }

  double evaluateMineGivenColHeights(const BitBoard b, const BitPieceInfo p, int *colHeights, int level) const {
    auto [bottomColumn, secondColumn] = getMinColumns(colHeights);
    if (colHeights[bottomColumn] == colHeights[secondColumn]) return 0;
    return w_[bottomColumn];
  }

 private:
  const Weighting w_;
};
