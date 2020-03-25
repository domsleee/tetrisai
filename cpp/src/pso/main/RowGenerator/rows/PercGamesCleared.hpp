#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct PercGamesCleared: public IRow {
  PercGamesCleared(int forced=UNDEF):
    forced_{forced}
    {}

  std::string getHeader() const override {
    if (forced_ != UNDEF) {
      return "%clrd_" + std::to_string(forced_);
    }
    return "%clrd";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    auto passLines = summaryResult.getConfig().startingLevel == 19 ? 230 : 130;
    if (forced_ != UNDEF) {
      passLines = forced_;
    }
    int res = 0;
    for (auto sm: summaryResult.getScoreManagers()) {
      res += (sm.getTotalLines() >= passLines);
    }
    return toFixed(100 * (double)res / static_cast<int>(summaryResult.getScoreManagers().size()), 1);
  }
 private:
  static const int UNDEF = -1;
  int forced_ = UNDEF;
};
