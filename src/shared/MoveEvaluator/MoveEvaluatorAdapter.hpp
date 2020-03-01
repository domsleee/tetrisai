#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/Weighting.hpp"
#include <algorithm>
#include <numeric>
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"


class MoveEvaluatorAdapter: public IEvaluator {
 public:
  MoveEvaluatorAdapter(const MoveEvaluator &mv, const Weighting &w): mv_(mv), w_(w) {}
  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    return mv_.evaluate(b, p, w_);
  }
  double evaluateMine(const BitBoard &b, const BitPieceInfo &p) const override {
    return evaluate(b, p);
  }

 private:
  const MoveEvaluator mv_;
  const Weighting w_;
};

