#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct Seed: public IRow {
  std::string getHeader() const override {
    return "seed";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    return toFixed(summaryResult.getConfig().seed, 0);
  }
};
