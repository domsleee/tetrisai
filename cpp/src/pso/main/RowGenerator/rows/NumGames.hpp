#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct NumGames: public IRow {
  std::string getHeader() const override {
    return "nGames";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    return toFixed(summaryResult.getConfig().numGames, 0);
  }
};
