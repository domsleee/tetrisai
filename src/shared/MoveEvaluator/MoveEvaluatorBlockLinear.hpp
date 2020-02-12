//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>

class MoveEvaluatorBlockLinear {
 public:
  static const int NUM_FACTORS = MoveEvaluatorTetrisReady::NUM_FACTORS + 2;
  static const int LINEAR_A = MoveEvaluatorTetrisReady::NUM_FACTORS;
  static const int LINEAR_B = MoveEvaluatorTetrisReady::NUM_FACTORS + 1;
   
  constexpr int getNumFactors() { return NUM_FACTORS; }

  MoveEvaluatorBlockLinear(const Weighting &w): me_{w}, w_{w} {
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluate(const BitBoard b, const BitPieceInfo p) const {
    auto eval = me_.evaluate(b, p);
    VacancyChecker vac(b);
    int colHeights[NUM_COLUMNS];
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      for (int r = 0; r < NUM_ROWS; ++r) {
        if (!vac.is_vacant({r, c})) {
          colHeights[c] = NUM_ROWS - r;
          break;
        }
      }
    }
    //int* colHeights = me_.getColHeights();

    auto [valid, minBlock] = getMinBlock(colHeights);
    if (valid) eval += w_[LINEAR_A] * minBlock + w_[LINEAR_B];
    return eval;
  }

 private:
  const MoveEvaluatorTetrisReady me_;
  const Weighting w_;
};
