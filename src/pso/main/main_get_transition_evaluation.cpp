
#include "src/pso/ClientApi.hpp"
#include "src/shared/get_moves_utils.hpp"
#include <chrono>

#define MY_FN getBestMoveEvaluator_50_2

const int MAXOUT_SCORE = 999999;

template<typename A>
void writeSms(FILE* fp, A sms) {
  fprintf(fp, "score, level, totalLines\n");
  for (auto sm: sms) {
    fprintf(fp, "%d, %d, %d\n", sm.getScore(), sm.getLevel(), sm.getTotalLines());
  }
}

int main(int argc, char *argv[]) {
  auto csvFileName = "/home/dom/tetrisai/hey.csv";
  auto startTime = std::chrono::system_clock::now();
  BitBoardPre::precompute();
  auto endTimePrecompute = std::chrono::system_clock::now();
 
  auto me1 = MY_FN(false);
  auto me2 = MY_FN(true);
  Config cfg;
  cfg.numGames = 120;
  cfg.startingLines = 0;
  cfg.numLines = 130;
  cfg.seed = 55;
  cfg.maxDropRem = 3;
  cfg.print(); 
  auto sms = get_transition_evaluation(me2, me2, cfg);
  auto endTime = std::chrono::system_clock::now();

  int numMaxouts = 0;
  for (auto sm: sms) numMaxouts += sm.getScore() >= MAXOUT_SCORE;

  writeSms(stdout, sms);
  printf("\n\nSUMMARY\n");
  printf("===========================\n");
  printf("numMaxouts: %d\n", numMaxouts);
  printf("maxoutRate: %0.2f%%\n", (double)100 * numMaxouts / sms.size());
  printf("--------------\n");
  printf("precompute: %ldms\n", getMs(endTimePrecompute-startTime));
  printf("evaluate time: %ldms\n", getMs(endTime-endTimePrecompute));
  if (strcmp(csvFileName, "") != 0) {
    printf("written to %s\n", csvFileName);
    auto fp = fopen(csvFileName, "w");
    writeSms(fp, sms);
    fclose(fp);
  }
}

