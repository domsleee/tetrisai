#include "catch.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAugmentFactory.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <memory>


SCENARIO("Using MoveEvaluatorAugmentFactory") {
  auto me1 = MoveEvaluatorGroup {
    {
      allFeatures(MoveEvaluatorTetrisReady)
    }
  };

  auto me2 = MoveEvaluatorGroup {
    {
      { std::make_shared<MoveEvaluatorAugmentFactory>(me1, Weighting({2})), {} }
    }
  };

  auto me3 = MoveEvaluatorGroup {
    {
      { std::make_shared<MoveEvaluatorAugmentFactory>(me1, Weighting({2})), {} },
      allFeatures(MoveEvaluatorTetrisReady)
    }
  };

  WHEN("we evaluate using me2") {
    auto b = BitBoard(getWell(4, 0));
    me2.setWeights({});
    REQ_DELTA(2, me2.evaluate(b, b.getEmptyPiece(), 19));
  }
  AND_WHEN("we evaluate using me3") {
    auto b = BitBoard(getWell(4, 0));
    me3.setWeights({5});
    REQ_DELTA(7, me3.evaluate(b, b.getEmptyPiece(), 19));
  }


}