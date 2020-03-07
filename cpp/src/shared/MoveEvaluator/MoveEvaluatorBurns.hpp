#pragma once
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/common/Weighting.hpp"

#include "src/common/common.hpp"
#include <vector>

class MoveEvaluatorBurns: public IEvaluator {
 public:
  static const int BURNS = 0;
  static const int NUM_FACTORS = 1;
   
  MoveEvaluatorBurns(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    BitBoard b2 = b;
    int burns = b2.applyPieceInfo(p);
    return w_[BURNS] * (burns == 4 ? 0 : burns);
  }

 private:
  const Weighting w_;
};
