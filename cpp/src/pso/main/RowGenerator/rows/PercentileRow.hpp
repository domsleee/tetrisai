#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct PercentileRow: public IRow {
  PercentileRow(int perc): perc_{perc} {}

  std::string getHeader() const override {
    return "perc"+std::to_string(perc_);
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    double val = getPercentile(summaryResult.getScores(), perc_);
    return toFixed(val, 0);
  }

 private:
  int perc_;
};
