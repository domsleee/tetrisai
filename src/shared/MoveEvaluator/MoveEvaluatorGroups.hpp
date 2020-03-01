#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinear.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBurns.hpp"

#include "src/shared/MoveEvaluator/MoveEvaluatorProperties.hpp"

#include <map>

#define allFeatures(T) { std::make_shared<EvaluatorFactory<T>>(), MoveEvaluatorProperties<T>::getAllFeatures() }

const std::string MOVE_EVALUATOR_GROUP_BURNS = "Burns";
const std::string MOVE_EVALUATOR_GROUP_LINEAR = "Linear";

std::map<std::string, MoveEvaluatorGroup> getMoveEvaluatorGroups() {
  std::map<std::string, MoveEvaluatorGroup> res;
  res.emplace(MOVE_EVALUATOR_GROUP_BURNS, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBurns)
    }
  });
  res.emplace(MOVE_EVALUATOR_GROUP_LINEAR, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
    }
  });
  return res;
}

