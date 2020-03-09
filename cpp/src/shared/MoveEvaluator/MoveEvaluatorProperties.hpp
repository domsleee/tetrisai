#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinear.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBurns.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorEdgeWell.hpp"

template <typename T>
struct MoveEvaluatorProperties {
  static std::vector<int> getAllFeatures() {
    throw std::runtime_error("MoveEvaluatorProperties: not implemented");
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
