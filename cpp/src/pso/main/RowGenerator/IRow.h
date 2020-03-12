#pragma once
#include <string>
#include "src/pso/summary/SummaryResult.hpp"

struct IRow {
  virtual ~IRow() {};

  virtual bool considerMultiple() const { return true; }
  virtual std::string getHeader() const = 0;
  virtual std::string getValue(const SummaryResult &summaryResult) const = 0;
};
