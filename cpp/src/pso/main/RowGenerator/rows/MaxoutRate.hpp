#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct MaxoutRate: public IRow {
  std::string getHeader() const override {
    return "maxout%";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    auto numMaxouts = 0;
    for (auto score: summaryResult.getScores()) numMaxouts += score >= 999999;
    auto maxoutRate = 100 * (double)numMaxouts / summaryResult.getScores().size();
    return toFixed(maxoutRate, 1);
  }
};
