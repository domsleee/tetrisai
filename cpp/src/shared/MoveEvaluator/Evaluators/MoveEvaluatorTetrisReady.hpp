//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/VacancyChecker.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>

class MoveEvaluatorTetrisReady: public IEvaluator {
 public:
  static const int NUM_FACTORS = 1;
  static const int TETRIS_READY = 0;

  MoveEvaluatorTetrisReady(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return evaluateMineGivenColHeights(evaluatorInfo.getAppliedBoard(), p, evaluatorInfo.getMyColHeights(), evaluatorInfo.getMaxDropRem(), evaluatorInfo.getAppliedBoardVac());
  }

  double evaluateMineGivenColHeights(const BitBoard b, const BitPieceInfo p, const int *colHeights, int dropRem, const VacancyChecker &vac) const {
    double eval = 0;

    int colWithMinHeight = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (colHeights[c] <= colHeights[colWithMinHeight]) { colWithMinHeight = c; }
    }
    bool fourDiff = true;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (c == colWithMinHeight) continue;
      if (colHeights[c] - colHeights[colWithMinHeight] < 4) fourDiff = false;
    }
    bool noHoles = true;
    if (fourDiff) {
      for (int c = 0; c < NUM_COLUMNS; ++c) {
        if (c == colWithMinHeight) continue;
        int nr = NUM_ROWS - colHeights[colWithMinHeight] - 4; // suspicious?
        for (int r = nr; r < nr + 4; ++r) {
          if (vac.is_vacant({r, c})) noHoles = false;
        }
      }
    }
    //printf("noHoles: %d, fourDiff: %d, isBotttomColAccessible: %d\n", noHoles, fourDiff, isBottomColAccessible(colHeights, dropRem));
    int tetrisReady = noHoles && fourDiff && isBottomColAccessible(colHeights, dropRem);
    eval += w_[TETRIS_READY] * tetrisReady;
    return eval;
  }

 private:
  const Weighting w_;
};
