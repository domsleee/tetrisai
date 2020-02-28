#include "src/pso/ClientApi.hpp"
#include "src/shared/get_moves_utils.hpp"
#include "src/shared/di/di.h"
#include "src/shared/MoveRecorder/MoveRecorder.h"
#include <chrono>


int main(int argc, char ** argv) {
  Di::setMoveRecorder(MoveRecorder());

  auto startTime = std::chrono::system_clock::now();
  auto me = getBestMoveEvaluatorBlockLinear(true);
  Config cfg;
  cfg.numGames = 100;
  cfg.seed = 55;
  cfg.maxDropRem = 2;
  cfg.print();
  get_score_regular(me, cfg);
  auto endTime = std::chrono::system_clock::now();
  printf("evaluate time: %ldms\n", getMs(endTime-startTime));
}
