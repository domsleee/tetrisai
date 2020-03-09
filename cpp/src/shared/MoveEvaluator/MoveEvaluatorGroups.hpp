#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorEdgeWell.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockLinear.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBurns.hpp"

#include "src/shared/MoveEvaluator/MoveEvaluatorProperties.hpp"

#include <map>

#define allFeatures(T) { std::make_shared<EvaluatorFactory<T>>(), MoveEvaluatorProperties<T>::getAllFeatures() }

const std::string MOVE_EVALUATOR_GROUP_NORMAL = "Normal";
const std::string MOVE_EVALUATOR_GROUP_BURNS = "Burns";
const std::string MOVE_EVALUATOR_GROUP_LINEAR = "Linear";
const std::string MOVE_EVALUATOR_GROUP_EDGE = "Edge";

std::map<std::string, MoveEvaluatorGroup> getMoveEvaluatorGroups();
