#include "src/shared/get_moves_utils.hpp"
#include "src/shared/di/di.h"
#include "src/shared/MoveRecorder/MoveRecorder.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/shared/Config.hpp"
#include "src/pso/SimpleApi.tpp"
#include "src/shared/MoveFinder/MoveFinderAll.h"
#include <chrono>


int main(int argc, char ** argv) {
  //Di::setMoveRecorder(MoveRecorder());

  auto startTime = std::chrono::system_clock::now();
  auto me = getBestMoveEvaluatorBlockLinear150_group(true);
  Config cfg;
  cfg.startingLines = 130;
  cfg.numLines = 230;
  cfg.numGames = 1;
  cfg.seed = 55;
  cfg.print();
  auto score = getEvaluateWeightings<MoveFinderRewrite>(me, cfg).runAllPieceSets();
  auto endTime = std::chrono::system_clock::now();
  printf("evaluate time: %ldms (%0.2f)\n", getMs(endTime-startTime), score);
}
