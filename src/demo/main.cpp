#include "src/shared/NewGetNextMove.tpp"
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator.hpp"
#include "src/demo/PlayDemo.tpp"
#include "src/demo/PlayBestDemo.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/pso/PieceSetGetter.hpp"


int main() {
  Weighting w = WeightingFn::readFromString(basic_weighting_best);
  auto moveEvaluator = MoveEvaluatorAdapter(MoveEvaluator(), w);
  MoveFinder mf;
  NewGetNextMove getNextMove(moveEvaluator, mf);
  PlayDemo playDemo(getNextMove);
  PlayBestDemo playBestDemo(playDemo);

  PieceSetGetter ps;
  auto pieceSets = ps.getPieceSets();
  playBestDemo.playBestDemo(pieceSets, w);

}