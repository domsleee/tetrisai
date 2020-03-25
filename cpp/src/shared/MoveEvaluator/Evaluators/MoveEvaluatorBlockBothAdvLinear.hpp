//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorBlockBothAdvUtility.h"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>


class MoveEvaluatorBlockBothAdvLinear: public IEvaluator {
 public:
  static const int NUM_FACTORS = 6;
  static const int BLOCKS_UNTIL_CLEARED_WELL_SIDE_LINEAR_A = 0;
  static const int BLOCKS_UNTIL_CLEARED_WELL_SIDE_LINEAR_B = 1;
  static const int BLOCKS_UNTIL_CLEARED_NONWELL_SIDE_LINEAR_A = 2;
  static const int BLOCKS_UNTIL_CLEARED_NONWELL_SIDE_LINEAR_B = 3;
  static const int NUM_BLOCKED_COLS = 4;
  static const int NUM_BLOCKED_ROWS = 5;
   
  MoveEvaluatorBlockBothAdvLinear(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double wellSideCalc(int maxDiff) const {
    return w_[BLOCKS_UNTIL_CLEARED_WELL_SIDE_LINEAR_A] * maxDiff + w_[BLOCKS_UNTIL_CLEARED_WELL_SIDE_LINEAR_B];
  }

  double nonWellSideCalc(int maxDiff) const {
    return w_[BLOCKS_UNTIL_CLEARED_NONWELL_SIDE_LINEAR_A] * maxDiff + w_[BLOCKS_UNTIL_CLEARED_NONWELL_SIDE_LINEAR_B];
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return evaluateMineGivenColHeights(evaluatorInfo.getAppliedBoard(), evaluatorInfo.getMyColHeights(), evaluatorInfo.getMaxDropRem());
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const int *colHeights, int maxDropRem) const {
    double eval = 0;
    auto [bottomColumn, secondColumn] = getMinColumns(colHeights);
    bool wellIsRight = bottomColumn >= 5;
    auto maxDiffLeft = getMaxColHeightsMinusClearHeightsRange(colHeights, maxDropRem, 1, 4);
    auto maxDiffRight = getMaxColHeightsMinusClearHeightsRange(colHeights, maxDropRem, 5, 8);

    if (wellIsRight) {
      eval += wellSideCalc(maxDiffRight) + nonWellSideCalc(maxDiffLeft);
    } else {
      eval += wellSideCalc(maxDiffLeft) + nonWellSideCalc(maxDiffRight);
    }

    auto isColAccessible = getIsColAccessible(colHeights, maxDropRem);
    int numBlockedCols = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) numBlockedCols += !isColAccessible[c];
    eval += w_[NUM_BLOCKED_COLS] * numBlockedCols;

    int numBlockedRows = getNumBlockedRows(colHeights, isColAccessible);
    eval += w_[NUM_BLOCKED_ROWS] * numBlockedRows;
    return eval;
  }

 private:
  const Weighting w_;
};
