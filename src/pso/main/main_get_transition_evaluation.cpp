
#include "src/pso/ClientApi.hpp"
#include "src/pso/main/get_moves_utils.hpp"


int main() {
  auto me1 = getBestMoveEvaluatorBlockLinear(false);
  auto me2 = getBestMoveEvaluatorBlockLinear(true);
  Config cfg;
  cfg.numGames = 80;
  cfg.seed = 55;
  cfg.print(); 
  auto sms = get_transition_evaluation(me1, me2, cfg);

  int numMaxouts = 0;
  for (auto sm: sms) numMaxouts += sm.getScore() >= 999999;

  printf("score, level, totalLines\n");
  for (auto sm: sms) {
    printf("%d, %d, %d\n", sm.getScore(), sm.getLevel(), sm.getTotalLines());
  }
  printf("\n\nSUMMARY\n");
  printf("===========================\n");
  printf("numMaxouts: %d\n", numMaxouts);
  printf("maxoutRate: %0.2f%%\n", (double)100 * numMaxouts / sms.size());
  printf("--------------\n");
}