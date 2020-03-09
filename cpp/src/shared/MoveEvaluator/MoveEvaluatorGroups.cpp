#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"

std::map<std::string, MoveEvaluatorGroup> getMoveEvaluatorGroups() {
  std::map<std::string, MoveEvaluatorGroup> res;
  res.emplace(MOVE_EVALUATOR_GROUP_BURNS, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBurns)
    }
  });
  res.emplace(MOVE_EVALUATOR_GROUP_LINEAR, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
    }
  });
  res.emplace(MOVE_EVALUATOR_GROUP_EDGE, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
      allFeatures(MoveEvaluatorEdgeWell)
    }
  });
  res.emplace(MOVE_EVALUATOR_GROUP_NORMAL, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>)
    }
  });
  return res;
}
