#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/IEvaluator.h"


class MoveEvaluatorAugment: public IEvaluator {
 public:
  MoveEvaluatorAugment(const MoveEvaluatorGroup &me):
    me_{me}
  {}

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return me_.evaluateMine(b, p, evaluatorInfo);
  }

 private:
  MoveEvaluatorGroup me_;
};
