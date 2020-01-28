//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/VacancyChecker.hpp"

#include "src/common/common.hpp"
#include <cassert>



class MoveEvaluatorTetrisReady {
 public:
  static const int NUM_FACTORS = MoveEvaluator::NUM_FACTORS + 1;
  static const int TETRIS_READY = MoveEvaluator::NUM_FACTORS;

  constexpr int getNumFactors() { return NUM_FACTORS; }

  MoveEvaluatorTetrisReady(const Weighting &w): me_{MoveEvaluator(), w}, w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    VacancyChecker vac(b);

    int colHeights[NUM_COLUMNS];
    double eval = me_.evaluate(b, p);
    for (int c = 0; c < NUM_COLUMNS; ++c) colHeights[c] = 0;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      for (int r = 0; r < NUM_ROWS; r++) {
        if (!vac.is_vacant({r, c})) {
          colHeights[c] = NUM_ROWS - r;
          break;
        }
      }
    }

    int mc = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (colHeights[c] <= colHeights[mc]) { mc = c; }
    }
    bool fourDiff = true;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (c == mc) continue;
      if (colHeights[c] - colHeights[mc] < 4) fourDiff = false;
    }
    bool noHoles = true;
    if (fourDiff) {
      for (int c = 0; c < NUM_COLUMNS; ++c) {
        if (c == mc) continue;
        int nr = NUM_ROWS - colHeights[mc] - 4;
        for (int r = nr; r < nr + 4; ++r) {
          if (vac.is_vacant({r, c})) noHoles = false;
        }
      }
    }
    int tetrisReady = noHoles && fourDiff;
    eval += w_[TETRIS_READY] * tetrisReady;
    return eval;
  }

 private:
  const MoveEvaluatorAdapter me_;
  const Weighting &w_;
};
