#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAugmentFactory.hpp"

std::map<std::string, MoveEvaluatorGroup> getMoveEvaluatorGroups() {
  std::map<std::string, MoveEvaluatorGroup> res;
  res.emplace(MOVE_EVALUATOR_GROUP_LINEAR, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
    }
  });

  res.emplace(MOVE_EVALUATOR_GROUP_BOTH_LINEAR, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockBothLinear),
    }
  });

  res.emplace(MOVE_EVALUATOR_GROUP_WELL_LOCATIONS, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockBothLinear),
      allFeatures(MoveEvaluatorWellLocations)
    }
  });

  res.emplace(MOVE_EVALUATOR_GROUP_WELL_LOCATIONS_NOT_BOTH, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
      allFeatures(MoveEvaluatorWellLocations)
    }
  });

  res.emplace(MOVE_EVALUATOR_GROUP_BOTH_LINEAR_ADV, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockBothAdvLinear),
    }
  });

  res.emplace(MOVE_EVALUATOR_GROUP_BOTH_LINEAR_ADV_VAR, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockBothAdvLinearVar),
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
  res.emplace(MOVE_EVALUATOR_GROUP_WELL_VERY_LEFT, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>),
      allFeatures(MoveEvaluatorTetrisReady),
      allFeatures(MoveEvaluatorBlockLinear),
      allFeatures(MoveEvaluatorWellVeryLeft)
    }
  });
  res.emplace(MOVE_EVALUATOR_GROUP_NORMAL, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorAdapter<MoveEvaluator>)
    }
  });

  const std::string b19_linearAgain = "[17.13162690918415 19.953561083931444 13.07767822693484 -3.8384184579204246 5.905180229423406 8.540305126601009 14.096674615381346 1.123364887863541 19.504255279933833 21.184977029726177 19.867257467512484 -7.217501217316558 -16.55603008888561 -18.583921791259886 -17.041402813898483 3.216537351149734 -1.1523936919609405 -10.138334122877396 -0.3843117420886184 2.7435489529345998]";
  
  res.emplace(MOVE_EVALUATOR_AUGMENTED, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorWellVeryLeft),
      {std::make_shared<MoveEvaluatorAugmentFactory>(res.at(MOVE_EVALUATOR_GROUP_LINEAR), WeightingFn::readFromString(b19_linearAgain)), {}}
    }
  });

  res.emplace(MOVE_EVALUATOR_AUGMENTED_LOC, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorWellLoc),
      {std::make_shared<MoveEvaluatorAugmentFactory>(res.at(MOVE_EVALUATOR_GROUP_LINEAR), WeightingFn::readFromString(b19_linearAgain)), {}}
    }
  });

  res.emplace(MOVE_EVALUATOR_AUGMENTED_BOTTOM_LOC, MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorBottomLoc),
      {std::make_shared<MoveEvaluatorAugmentFactory>(res.at(MOVE_EVALUATOR_GROUP_LINEAR), WeightingFn::readFromString(b19_linearAgain)), {}}
    }
  });
  return res;
}
