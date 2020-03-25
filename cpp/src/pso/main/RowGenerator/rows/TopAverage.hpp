#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct TopAverage: public IRow {
  TopAverage(int perc): perc_{perc} {}

  std::string getHeader() const override {
    return "top"+toFixed(perc_, 0)+"avg";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    double val = getTopAverage(summaryResult.getScores(), perc_);
    return toFixed(val, 0);
  }

 private:
  int perc_;
};
