#pragma once
#include "src/pso/main/RowGenerator/IRow.h"
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/stats.h"


struct Name: public IRow {
  std::string getHeader() const override {
    return "name";
  }

  std::string getValue(const SummaryResult &summaryResult) const override {
    return summaryResult.getName();
  }

  bool considerMultiple() const override {
    return false;
  }
};
