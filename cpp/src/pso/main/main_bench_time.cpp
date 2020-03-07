#include "src/pso/ClientApi.hpp"
#include "src/shared/get_moves_utils.hpp"
#include "src/shared/di/di.h"
#include "src/shared/MoveRecorder/MoveRecorder.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include <chrono>


int main(int argc, char ** argv) {
  Di::setMoveRecorder(MoveRecorder());

  auto startTime = std::chrono::system_clock::now();
  auto me = getBestMoveEvaluatorBlockLinear150_group(true);
  Config cfg;
  cfg.startingLines = 130;
  cfg.numGames = 100;
  cfg.seed = 55;
  cfg.maxDropRem = 2;
  cfg.print();
  auto score = get_score_regular(me, cfg);
  auto endTime = std::chrono::system_clock::now();
  printf("evaluate time: %ldms (%0.2f)\n", getMs(endTime-startTime), score);
}
