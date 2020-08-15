#pragma once
#include <string>
#include "src/pso/summary/SummaryResult.hpp"
#include "src/pso/defs.h"

struct LogFileResult {
  Config config;
  std::string group;
  Weighting weights;
};

class SummaryApi {
 public:
  SummaryApi(const std::string &logFolder=DATA_FOLDER): 
    logFolder_{logFolder}
  {}
  std::vector<std::string> getAllNames() const;
  SummaryResult getSummary(const std::string &name) const;
  SummaryResult getSummary(const std::string &name1, const std::string &name2, int transitionLines) const;
  SummaryResult getSummaryFSM(const std::string &name) const;
  SummaryResult getSummaryFSM(const std::string &name1, const std::string &name2, int transitionLines) const;
  SummaryResult getSummaryAllMoveFinder(const std::string &name) const;
  SummaryResult getSummaryAllMoveFinderLookahead(const std::string &name) const;
  SummaryResult getSummaryLookahead(const std::string &name1, const std::string &name2, int transitionLines) const;

  // exposed for testing
  LogFileResult readLogFile(const std::string &name) const;
 private:
  template<typename MyMoveFinder>
  SummaryResult getSummaryHelper(const std::string &name) const;

  template<typename MyMoveFinder>
  SummaryResult getSummaryHelper(const std::string &name1, const std::string &name2, int transitionLines) const;

  const std::string logFolder_;
};

#include "src/pso/summary/SummaryApi.tpp"


