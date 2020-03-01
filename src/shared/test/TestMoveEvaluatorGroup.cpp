
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorTetrisReady.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MoveEvaluator/IEvaluatorFactory.h"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>


SCENARIO("tetris ready when its ready") {
  MoveEvaluatorGroup me{
    {
      {
        std::make_shared<EvaluatorFactory<MoveEvaluatorTetrisReady>>(), { MoveEvaluatorTetrisReady::TETRIS_READY },
      },
      {
        std::make_shared<EvaluatorFactory<MoveEvaluatorAdapter>>(), { MoveEvaluator::PILE_HEIGHT }
      }
    }
  };
  me.setWeights({1, 0});

  GIVEN("not tetris ready board") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);

    auto eval = me.evaluate(b, b.getEmptyPiece());
    REQ_DELTA(0, eval);
    me.setWeights({0, 1});
    eval = me.evaluate(b, b.getEmptyPiece());
    REQ_DELTA(3, eval);
  }
  AND_GIVEN("a tetris ready board") {
    const std::vector<std::string> board = {
        "1111111110",
        "1111111110",
        "1111111110",
        "1111111110",
    };
    const auto b = getBoardFromPartialStringVector(board);
    auto w = getWeightsFromEmptyPieceT<MoveEvaluatorTetrisReady>(b);
    
    auto eval = me.evaluate(b, b.getEmptyPiece());
    REQ_DELTA(1, eval);
    me.setWeights({0, 1});
    eval = me.evaluate(b, b.getEmptyPiece());
    REQ_DELTA(4, eval);
  }
  
}
