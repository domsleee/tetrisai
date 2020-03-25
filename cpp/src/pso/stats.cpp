#include "src/pso/stats.h"
#include "src/common/common.hpp"
#include <iomanip>

double getTopAverage(std::vector<int> scores, int percentage) {
  int amount = scores.size() * percentage / 100;
  return average(scores.end() - amount, scores.end());
}

double getPercentile(std::vector<int> scores, int percentile) {
  int amount = scores.size() * percentile / 100;
  if (amount == 0) return -1;
  return scores.at(amount-1);
}

std::string toFixed(double val, int dp) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(dp) << val;
  return ss.str();
}