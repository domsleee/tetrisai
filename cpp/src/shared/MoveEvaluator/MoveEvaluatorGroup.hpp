#pragma once
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/IEvaluatorFactory.h"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"
#include <vector>
#include <memory>

namespace MoveEvaluatorGroupNs {
  using FeatureGroupT = std::pair<std::shared_ptr<IEvaluatorFactory>, std::vector<int>>;
}

class MoveEvaluatorGroup: public IEvaluator {
 public:
  const int NUM_FACTORS;

  
  MoveEvaluatorGroup(std::vector<MoveEvaluatorGroupNs::FeatureGroupT> features):
    NUM_FACTORS(std::accumulate(features.begin(), features.end(), 0, [](const int a, const MoveEvaluatorGroupNs::FeatureGroupT &feature) {
      return a + feature.second.size();
    })),
    features_(features)
      {}


  MoveEvaluatorGroup& setWeights(const Weighting &w) {
    setWeightsCalled_ = true;
    if (static_cast<int>(w.size()) != NUM_FACTORS) {
      printf("Given %lu, expected %d\n", w.size(), NUM_FACTORS);
      throw std::runtime_error("Unexpected weight size");
    }
    evaluators_.clear();
    int i = 0;
    for (const auto &pair: features_) {
      const auto &vec = pair.second;
      Weighting w2(pair.first->getNumFactors());
      for (int feature: vec) {
        w2[feature] = w[i++];
      }
      evaluators_.push_back(pair.first->createNew(w2));
    }
    return *this;
  }

  double evaluate(const BitBoard &b, const BitPieceInfo &p, const ScoreManager &sm) const {
    return evaluateMine(b, p, {b, p, sm});
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    if (!setWeightsCalled_) throw std::runtime_error("setWeights must be called before evaluate");
    double res = 0;
    for (const auto &evaluator: evaluators_) {
      res += evaluator->evaluateMine(b, p, evaluatorInfo);
    }
    return res;
  }

private:
  bool setWeightsCalled_ = false;
  std::vector<std::pair<std::shared_ptr<IEvaluatorFactory>, std::vector<int>>> features_;
  std::vector<std::shared_ptr<IEvaluator>> evaluators_ = {};
};
