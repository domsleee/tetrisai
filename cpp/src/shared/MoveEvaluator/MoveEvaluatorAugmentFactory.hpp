#pragma once
#include "src/shared/MoveEvaluator/IEvaluatorFactory.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAugment.hpp"
#include "src/common/common.hpp"
#include <vector>
#include <memory>

class MoveEvaluatorAugmentFactory: public IEvaluatorFactory {
 public:
  MoveEvaluatorAugmentFactory(MoveEvaluatorGroup me, const Weighting &w):
    me_(me.setWeights(w)),
    numFactors_(me.NUM_FACTORS)
  {}

  int getNumFactors() override {
    return numFactors_;
  }

  std::unique_ptr<IEvaluator> createNew(Weighting &w) override {
    return std::make_unique<MoveEvaluatorAugment>(me_);
  }

 private:
  const MoveEvaluatorGroup me_;
  const int numFactors_;
};
