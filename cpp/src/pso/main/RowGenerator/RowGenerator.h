#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "third_party/libfort/fort.hpp"
#include "src/pso/main/RowGenerator/IRow.h"

class RowGenerator {
 public:
  RowGenerator(const fort::char_table &table, const std::vector<std::shared_ptr<IRow>> &features):
    table_{table},
    features_{features} {}
  void applyHeader();
  void calculateAndApplyFeatures();
  void setFn(const std::function<SummaryResult()> &fn) {
    setFns({fn});
  }
  void setFns(const std::vector<std::function<SummaryResult()>> &fns) {
    fns_ = fns;
  }
  fort::char_table getTable() { return table_; }
  std::vector<SummaryResult> getResult() { return result_; }

 private:
  fort::char_table table_;
  std::vector<SummaryResult> result_;
  std::vector<std::function<SummaryResult()>> fns_;
  std::vector<std::shared_ptr<IRow>> features_;
};
