#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/Weighting.hpp"
#include <algorithm>
#include <numeric>
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"

template<typename MyMoveEvaluator>
class MoveEvaluatorAdapter: public IEvaluator {
 public:
  static const int NUM_FACTORS = MyMoveEvaluator::NUM_FACTORS;
  MoveEvaluatorAdapter(const Weighting &w): w_(w) {}
  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    return me_.evaluate(b, p, w_);
  }
  // todo: deprecated
  double evaluate(const BitBoard &b, const BitPieceInfo &p, const int level) const {
    return evaluate(b, p);
  }
  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaulatorInfo) const override {
    return evaluate(b, p);
  }

 private:
  const MyMoveEvaluator me_;
  const Weighting w_;
};

