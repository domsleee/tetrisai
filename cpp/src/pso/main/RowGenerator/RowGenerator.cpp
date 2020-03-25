#include "src/pso/main/RowGenerator/RowGenerator.h"
#include "src/common/common.hpp"
#include "src/pso/stats.h"
#include <chrono>

void RowGenerator::applyHeader() {
  for (auto feature: features_) {
    table_ << fort::header << feature->getHeader();
  }
  table_ << "time(s)" << fort::endr;
}

void RowGenerator::calculateAndApplyFeatures() {
  if (fns_.size() == 0) {
    throw std::runtime_error("no fns");
  }
  std::vector<std::string> results;
  for (auto fn: fns_) {
    auto startTime = std::chrono::system_clock::now();
    auto summaryResult = fn();
    auto endTime = std::chrono::system_clock::now();
    bool firstSummaryResult = results.empty();

    for (int i = 0; i < static_cast<int>(features_.size()); ++i) {
      const auto &feature = features_[i];
      if (firstSummaryResult) {
        results.push_back(feature->getValue(summaryResult));
      } else if (feature->considerMultiple()) {
        results[i] += "," + feature->getValue(summaryResult);
      }
    }

    auto timeTaken = toFixed((double)getMs(endTime-startTime)/1000, 1);
    if (firstSummaryResult) {
      results.push_back(timeTaken);
    } else {
      results[results.size()-1] += "," + timeTaken;
    }
  }
  
  for (auto str: results) {
    table_ << str;
  }
  table_ << fort::endr;
}