#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/AllMoveEvaluators.h"

// note: we don't need this anymore. We can just use numbers [0, NUM_FACTORS)

template <typename T, size_t S = sizeof(T)>
struct MoveEvaluatorProperties {
  static std::vector<int> getAllFeatures() {
    static_assert(sizeof(T) == -1, "You have to have a specialization for Helper!");
    throw std::runtime_error("compiler");
  }
};

template <>
struct MoveEvaluatorProperties<MoveEvaluatorWellVeryLeft> {
  static std::vector<int> getAllFeatures() {
    return { MoveEvaluatorWellVeryLeft::WELL_VERY_LEFT };
  }
};

template <>
struct MoveEvaluatorProperties<MoveEvaluatorEdgeWell> {
  static std::vector<int> getAllFeatures() {
    return { MoveEvaluatorEdgeWell::EDGE_WELL };
  }
};
template <>
struct MoveEvaluatorProperties<MoveEvaluatorTetrisReady> {
  static std::vector<int> getAllFeatures() {
    return { MoveEvaluatorTetrisReady::TETRIS_READY };
  }
};
template <>
struct MoveEvaluatorProperties<MoveEvaluatorBurns> {
  static std::vector<int> getAllFeatures() {
    return { MoveEvaluatorBurns::BURNS };
  }
};
template <>
struct MoveEvaluatorProperties<MoveEvaluatorBlockLinear> {
  static std::vector<int> getAllFeatures() {
    return { MoveEvaluatorBlockLinear::LINEAR_A, MoveEvaluatorBlockLinear::LINEAR_B };
  }
};

template <>
struct MoveEvaluatorProperties<MoveEvaluatorAdapter<MoveEvaluator>> {
  static std::vector<int> getAllFeatures() {
    std::vector<int> res;
    for (int i = 0; i <= 16; ++i) res.push_back(i);
    return res;
  }
};
