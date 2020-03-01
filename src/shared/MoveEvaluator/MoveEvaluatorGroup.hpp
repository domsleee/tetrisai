#pragma once
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/IEvaluatorFactory.h"
#include "src/common/Weighting.hpp"
#include <vector>
#include <memory>


class MoveEvaluatorGroup {
public:
  const int NUM_FACTORS;

  using FeatureGroupT = std::pair<std::shared_ptr<IEvaluatorFactory>, std::vector<int>>;
  MoveEvaluatorGroup(std::vector<FeatureGroupT> features):
    NUM_FACTORS(std::accumulate(features.begin(), features.end(), 0, [](const int a, const FeatureGroupT &feature) {
      return a + feature.second.size();
    })),
    features_(features)
      {}


  void setWeights(const Weighting &w) {
    if (static_cast<int>(w.size()) != NUM_FACTORS) {
      printf("Given %lu, expected %d\n", w.size(), NUM_FACTORS);
      throw std::runtime_error("Unexpected weight size");
    }
    evaluators_.clear();
    int i = 0;
    for (auto &pair: features_) {
      const auto &vec = pair.second;
      Weighting w2;
      for (int feature: vec) {
        if (static_cast<int>(w2.size()) <= feature) w2.resize(feature+1, 0);
        w2[feature] = w[i++];
      }
      evaluators_.push_back(pair.first->createNew(w2));
    }
  }

  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    double res = 0;
    for (const auto &evaluator: evaluators_) {
      res += evaluator->evaluateMine(b, p);
    }
    return res;
  }

private:
  std::vector<std::pair<std::shared_ptr<IEvaluatorFactory>, std::vector<int>>> features_;
  std::vector<std::shared_ptr<IEvaluator>> evaluators_ = {};
};
