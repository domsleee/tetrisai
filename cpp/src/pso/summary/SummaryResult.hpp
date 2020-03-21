#pragma once
#include "src/shared/Config.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/ScoreManager.hpp"
#include <vector>


class SummaryResult {
 public:
  SummaryResult(const std::string &name, const std::string &group, const std::vector<ScoreManager> &sms, const Config &config):
    name_{name},
    group_{group},
    scoreManagers_{sms},
    config_{config}
    {
      scores_ = {};
      for (auto sm: scoreManagers_) {
        scores_.push_back(sm.getScore());
      }
    }
  std::string getName() const { return name_; };
  std::string getGroup() const { return group_; };
  const std::vector<int>& getScores() const { return scores_; };
  const std::vector<ScoreManager>& getScoreManagers() const { return scoreManagers_; }
  Config getConfig() const { return config_; };
 private:
  std::string name_;
  std::string group_;
  std::vector<int> scores_;
  std::vector<ScoreManager> scoreManagers_;
  Config config_;
};
