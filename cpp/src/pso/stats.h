#pragma once
#include <vector>
#include <sstream>

double getTopAverage(std::vector<int> scores, int percentage);
double getPercentile(std::vector<int> scores, int percentile);
std::string toFixed(double val, int dp);
