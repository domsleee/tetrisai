
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/demo/PlayBestDemoContainer.tpp"


#include <memory>


int main() {
  PieceSetGetter ps;
  auto pieceSets = ps.getPieceSets(3);
  const auto w = WeightingFn::readFromString(basic_weighting_best);


  PlayBestDemoContainer cont(
    MoveEvaluatorAdapter(MoveEvaluator(), w),
    MoveFinder()
  );
  auto myPlayBestDemo = cont.getInstance();
  myPlayBestDemo.playBestDemo(pieceSets);
}
