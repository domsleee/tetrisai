//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"

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
    int colHeights[NUM_COLUMNS];
    double eval = me_.evaluate(b, p);

    int tetrisReady = 0, totalEmpty = 0, maxColEmpty = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      int r = 0, colEmpty = 0;
      for (r = NUM_ROWS-1; r >= NUM_ROWS-4; --r) {
        if (b.vacant({r, c})) {
          totalEmpty++;
          colEmpty++;
        }
      }
      maxColEmpty = std::max(maxColEmpty, colEmpty);
    }
    if (maxColEmpty == 4 && totalEmpty == 4) {
      tetrisReady = 1;
    }
    eval += w_[TETRIS_READY] * tetrisReady;
    return eval;
  }

 private:
  const MoveEvaluatorAdapter me_;
  const Weighting &w_;
};
