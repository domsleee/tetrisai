
#include "src/pso/ClientApi.hpp"
#include "src/pso/main/get_moves_utils.hpp

int main(int argc, char ** argv) {
  auto me = getBestMoveEvaluatorBlockLinear(true);
  Config cfg;
  cfg.numGames = 100;
  cfg.seed = 55;
  cfg.print();
  get_score_regular(me, cfg);
}
