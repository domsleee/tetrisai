#pragma once
#include <iostream>
#include <map>
#include <vector>


#define PAIR(x) { #x, x }
#define SET(x) x = m.at(#x)


// todo: refactor
// best is to use setters/getters
struct Config {
  static const int UNDEF = -1;
  int seed = UNDEF;
  static std::vector<std::string> allFields;
  int numGames = UNDEF;
  int numLines = UNDEF;
  int maxDropRem = UNDEF; // todo.
  int startingLines = UNDEF;
  int startingLevel = UNDEF;
  int averageAmount = UNDEF;

  void print() const {
    printf("FEATURE EVALUATION CONFIG\n");
    printf("========\n");
    auto m = toMap();
    for (auto it = m.begin(); it != m.end(); ++it) {
      printf("%s: %d\n", it->first.c_str(), it->second);
    }
    printf("========\n");
  }

  std::map<std::string, int> toMap() const {
    return {
      PAIR(seed),
      PAIR(numGames),
      PAIR(numLines),
      PAIR(maxDropRem),
      PAIR(startingLines),
      PAIR(startingLevel),
      PAIR(averageAmount)
    };
  }

  void fromMap(const std::map<std::string, int> &m) {
    SET(seed);
    SET(numGames);
    SET(numLines);
    SET(maxDropRem);
    SET(startingLines);
    SET(startingLevel);
    SET(averageAmount);
  }

  void setField(std::string key, std::string val) {
    auto m = toMap();
    m.at(key) = std::stoi(val);
    fromMap(m);
  }

  void setupForLongPlay() {
    startingLevel = Config::UNDEF;
    startingLines = Config::UNDEF;
    maxDropRem = Config::UNDEF;
    numLines = 1000;
  }

  template <typename MyEvaluateWeightings>
  void applyConfig(MyEvaluateWeightings &ew) const {
    if (seed != Config::UNDEF) ew.setSeed(seed);
    if (numGames != Config::UNDEF) ew.setNumGames(numGames);
    if (numLines != Config::UNDEF) ew.setNumLines(numLines);
    if (startingLines != Config::UNDEF) ew.setStartingLines(startingLines);
    // if (maxDropRem != Config::UNDEF) {};
    if (startingLevel != Config::UNDEF) ew.setStartingLevel(startingLevel);
    if (averageAmount != Config::UNDEF) ew.setAverageAmount(averageAmount);
  }
};