#pragma once
#include "src/shared/MoveEvaluator/IEvaluatorFactory.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/common/common.hpp"
#include <vector>
#include <memory>

class MoveEvaluatorGroupFactory: public IEvaluatorFactory {
 public:
  MoveEvaluatorGroupFactory(const std::vector<MoveEvaluatorGroupNs::FeatureGroupT> &features):
    features_(features)
  {}

  std::unique_ptr<IEvaluator> createNew(const Weighting &w) override {
    auto val = std::make_unique<MoveEvaluatorGroup>(features_);
    val->setWeights(w);
    return val;
  }

 private:
  const std::vector<MoveEvaluatorGroupNs::FeatureGroupT> features_;
};
