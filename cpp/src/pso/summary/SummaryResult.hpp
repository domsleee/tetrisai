#pragma once
#include "src/shared/Config.hpp"
#include "src/common/Weighting.hpp"
#include <vector>


class SummaryResult {
 public:
  SummaryResult(const std::string &name, const std::string &group, const std::vector<int> &scores, const Config &config):
    name_{name},
    group_{group},
    scores_{scores},
    config_{config}
    {}
  std::string getName() const { return name_; };
  std::string getGroup() const { return group_; };
  const std::vector<int>& getScores() const { return scores_; };
  Config getConfig() const { return config_; };
 private:
  std::string name_;
  std::string group_;
  std::vector<int> scores_;
  Config config_;
};
