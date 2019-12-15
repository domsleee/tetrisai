
#include "src/shared/MoveFinder/MoveFinder.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"
#include "src/pso/PieceSetGetter.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveFinder/CacheMoveFinder.tpp"
#include "src/demo/PlayBestDemoContainer.tpp"


std::string leftBest = "[7.201090035050697 0.6803263246170981 2.5309091534808084 -2.567191361633152 -8.477823863408982 2.861254658996007 -0.10704817057340765 1.2507849947092708 9.55394214238503 10.736517682805502 10.788586037271989 3.9636334720923596 -3.82475000633373 -13.675637895451793 -7.804330248402439 0.8399842387266787 1.1998470700878294 20.588708640318544]";


int main() {
  PieceSetGetter ps;
  auto pieceSets = ps.getPieceSets(3);
  const auto w = WeightingFn::readFromString(leftBest);

  auto cont = PlayBestDemoContainer(
    MoveEvaluatorPenalty(w),
    CacheMoveFinder<MoveFinder>()
  );
  auto myPlayBestDemo = cont.getInstance();
  myPlayBestDemo.playBestDemo(pieceSets);
}
