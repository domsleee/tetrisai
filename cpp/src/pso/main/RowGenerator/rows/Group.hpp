#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct Group: public IRow {
  std::string getHeader() const override {
    return "group";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    return summaryResult.getGroup();
  }
};
