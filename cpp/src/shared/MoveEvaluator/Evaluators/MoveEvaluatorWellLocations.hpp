#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

#include "src/common/common.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

class MoveEvaluatorWellLocations: public IEvaluator {
 public:
  static const int NUM_FACTORS = 2;
  static const int WELL_LEFT = 0; // "left" means <= 4
  static const int WELL_RIGHT = 1; // "left" means <= 4

  static const int WELL_THRESHOLD = 3;
   
  MoveEvaluatorWellLocations(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return evaluateMineGivenColHeights(evaluatorInfo.getAppliedBoard(), evaluatorInfo.getMyColHeights());
  }

  double getWellInColumnVal(int c) const {
    return c <= 4 ? w_[WELL_LEFT] : w_[WELL_RIGHT];
  }

  double evaluateMineGivenColHeights(const BitBoard &b, const int *colHeights) const {
    double eval = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      bool blockedLeft = c == 0 || colHeights[c-1] - colHeights[c] >= WELL_THRESHOLD;
      bool blockedRight = c == NUM_COLUMNS - 1 || colHeights[c+1] - colHeights[c] >= WELL_THRESHOLD;
      if (blockedLeft && blockedRight) {
        eval += getWellInColumnVal(c);
      }
    }
    return eval;
  }

 private:
  const Weighting w_;
};
