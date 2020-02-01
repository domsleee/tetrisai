#pragma once
#include <iostream>
#include "src/pso/NewEvaluateWeightings.tpp"

struct Config {
  static const int UNDEF = -1;
  int seed = UNDEF;
  int numGames = UNDEF;
  int numLines = UNDEF;
  int maxDropRem = UNDEF; // todo.
  int startingLines = UNDEF;

  void print() const {
    printf("CONFIG\n");
    printf("========\n");
    printf("seed: %d\n", seed);
    printf("numGames: %d\n", numGames);
    printf("numLines: %d\n", numLines);
    printf("maxDropRem (todo): %d\n", maxDropRem);
    printf("startingLines: %d\n", startingLines);
    printf("========\n");
  }

  template <typename MyRunPieceSet>
  void applyConfig(NewEvaluateWeightings<MyRunPieceSet> &ew) const {
    if (seed != Config::UNDEF) ew.setSeed(seed);
    if (numGames != Config::UNDEF) ew.setNumGames(numGames);
    if (numLines != Config::UNDEF) ew.setNumLines(numLines);
    if (startingLines != Config::UNDEF) ew.setStartingLines(startingLines);
  }
};