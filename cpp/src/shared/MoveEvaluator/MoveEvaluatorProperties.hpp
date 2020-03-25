#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/Evaluators/AllMoveEvaluators.h"

// note: we don't need this anymore. We can just use numbers [0, NUM_FACTORS)

template <typename T, size_t S = sizeof(T)>
struct MoveEvaluatorProperties {
  static std::vector<int> getAllFeatures() {
    std::vector<int> res;
    for (int i = 0; i < T::NUM_FACTORS; ++i) res.push_back(i);
    return res;
  }
};
