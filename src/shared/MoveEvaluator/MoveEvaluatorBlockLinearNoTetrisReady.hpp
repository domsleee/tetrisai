//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinear.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

#include "src/common/common.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>

Weighting transformW(const Weighting &w) {
  Weighting w2;
  for (int i = 0; i < w.size(); ++i) {
    if (i == MoveEvaluatorTetrisReady::TETRIS_READY) w2.push_back(0);
    w2.push_back(w[i]);
  }
  return w2;
}

class MoveEvaluatorBlockLinearNoTetrisReady {
 public:
  static const int NUM_FACTORS = MoveEvaluatorBlockLinear::NUM_FACTORS - 1;
  
  MoveEvaluatorBlockLinearNoTetrisReady(const Weighting &w): me_{transformW(w)} {  }

  double evaluate(const BitBoard b, const BitPieceInfo p) const {
    return me_.evaluate(b, p);
  }

 private:
  MoveEvaluatorBlockLinear me_;
};
