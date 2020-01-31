#include <iostream>

struct Config {
  static const int UNDEF = -1;
  int seed = UNDEF;
  int numGames = UNDEF;
  int numLines = UNDEF;
  int maxDropRem = UNDEF; // todo.
  int startingLines = UNDEF;

  void print() {
    printf("seed: %d\n", seed);
    printf("numGames: %d\n", numGames);
    printf("numLines: %d\n", numLines);
    printf("maxDropRem: %d\n", maxDropRem);
    printf("startingLines: %d\n", startingLines);
  }
};

template <typename MyRunPieceSet>
void applyConfig(NewEvaluateWeightings<MyRunPieceSet> &ew, const Config &config) {
  if (config.seed != Config::UNDEF) ew.setSeed(config.seed);
  if (config.numGames != Config::UNDEF) ew.setNumGames(config.numGames);
  if (config.numLines != Config::UNDEF) ew.setNumLines(config.numLines);
  if (config.startingLines != Config::UNDEF) ew.setStartingLines(config.startingLines);
}
