#include "src/pso/stats.h"
#include "src/common/common.hpp"

double getTopAverage(std::vector<int> scores, int percentage) {
  int amount = scores.size() * percentage / 100;
  return average(scores.end() - amount, scores.end());
}

double getPercentile(std::vector<int> scores, int percentile) {
  int amount = scores.size() * percentile / 100;
  return scores.at(amount-1);
}