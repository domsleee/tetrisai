#include "src/pso/ClientApi.hpp"
#include "src/pso/main/get_moves_utils.hpp"
#include <chrono>


int main(int argc, char ** argv) {
  auto startTime = std::chrono::system_clock::now();
  auto me = getBestMoveEvaluatorBlockLinear(true);
  Config cfg;
  cfg.numGames = 100;
  cfg.seed = 55;
  cfg.print();
  get_score_regular(me, cfg);
  auto endTime = std::chrono::system_clock::now();
  printf("evaluate time: %ldms\n", getMs(endTime-endTimePrecompute));
}
