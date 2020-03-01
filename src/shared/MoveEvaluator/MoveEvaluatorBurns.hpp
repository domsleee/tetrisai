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
    if (w.size() < NUM_FACTORS) {
      printf("Bad weight vector size. Needed at least %d, got %lu", NUM_FACTORS, w.size());
      throw std::runtime_error("bad weight vector size");
    }
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p) const override {
    BitBoard b2 = b;
    int burns = b2.applyPieceInfo(p);
    return w_[BURNS] * (burns == 4 ? 0 : burns);
  }

 private:
  const Weighting w_;
};
