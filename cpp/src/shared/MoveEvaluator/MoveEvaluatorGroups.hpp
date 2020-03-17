#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/Evaluators/AllMoveEvaluators.h"

#include "src/shared/MoveEvaluator/MoveEvaluatorProperties.hpp"

#include <map>

#define allFeatures(T) { std::make_shared<EvaluatorFactory<T>>(), MoveEvaluatorProperties<T>::getAllFeatures() }

const std::string MOVE_EVALUATOR_GROUP_NORMAL = "Normal";
const std::string MOVE_EVALUATOR_GROUP_LINEAR = "Linear";
const std::string MOVE_EVALUATOR_GROUP_EDGE = "Edge";
const std::string MOVE_EVALUATOR_GROUP_WELL_VERY_LEFT = "WellVeryLeft";
const std::string MOVE_EVALUATOR_AUGMENTED = "Augmented";
const std::string MOVE_EVALUATOR_AUGMENTED_LOC = "AugmentedLoc";
const std::string MOVE_EVALUATOR_AUGMENTED_BOTTOM_LOC = "AugmentedBottomLoc";
const std::string MOVE_EVALUATOR_GROUP_BOTH_LINEAR = "BothLinear";
const std::string MOVE_EVALUATOR_GROUP_BOTH_LINEAR_ADV = "BothLinearAdv";
const std::string MOVE_EVALUATOR_GROUP_BOTH_LINEAR_ADV_VAR = "BothLinearAdvVar";


std::map<std::string, MoveEvaluatorGroup> getMoveEvaluatorGroups();
