#include "src/pso/summary/SummaryApi.h"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include "src/common/Weighting.hpp"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveFinder/MoveFinderBfs.tpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/pso/summary/SummaryApiUtility.tpp"
#include <filesystem>
namespace fs = std::filesystem;

std::vector<std::string> SummaryApi::getAllNames() const {
  std::vector<std::string> res;
  for (const auto & entry : fs::directory_iterator(logFolder_)) {
    if (!entry.is_regular_file()) continue;
    std::string path = entry.path();
    std::vector<std::string> spl;
    boost::split(spl, path, boost::is_any_of("/"));
    res.push_back(spl.back());
  }
  return res;
}

SummaryResult SummaryApi::getSummary(const std::string &name) const {
  return getSummaryHelper<MoveFinderRewrite>(name);
}

SummaryResult SummaryApi::getSummaryFSM(const std::string &name) const {
  return getSummaryHelper<MoveFinderFSM>(name);
}


SummaryResult SummaryApi::getSummary(const std::string &name1, const std::string &name2, int transitionLines) const {
  return getSummaryHelper<MoveFinderRewrite>(name1, name2, transitionLines);
}


SummaryResult SummaryApi::getSummaryFSM(const std::string &name1, const std::string &name2, int transitionLines) const {
  return getSummaryHelper<MoveFinderFSM>(name1, name2, transitionLines);
}

SummaryResult SummaryApi::getSummaryLookahead(const std::string &name1, const std::string &name2, int transitionLines) const {
  auto info1 = readLogFile(name1);
  auto info2 = readLogFile(name2);
  auto config = info1.config;
  config.setupForLongPlay();
  config.numGames = 12;
  config.seed = 205;
  config.numLines = 230;
  config.startingLevel = 19;
  auto me1 = getMoveEvaluatorGroups().at(info1.group).setWeights(info1.weights);
  auto me2 = getMoveEvaluatorGroups().at(info2.group).setWeights(info2.weights);
  auto scoreManagers = getScoresLookahead<MoveFinderBfs>(config, me1, me2, transitionLines);
  return {
    name1 + "_" + name2 + "_Lookahead",
    info1.group + "_" + info2.group,
    scoreManagers,
    config
  };
}

LogFileResult SummaryApi::readLogFile(const std::string &name) const {
  auto filepath = logFolder_ + '/' + name;
  std::ifstream fin(filepath);
  std::string line;
  Config config;
  std::string group = "";
  std::string weights = "";
  while (std::getline(fin, line)) {
    if (line == "FEATURE EVALUATION CONFIG") {
      std::getline(fin, line);
      if (line[0] != '=') throw std::runtime_error("parsing error on feature evaluation config");
      std::getline(fin, line);
      while (line[0] != '=') {
        std::vector<std::string> result;
        boost::split(result, line, boost::is_any_of(":"));
        for (auto str: result) boost::trim(str);
        auto key = result[0];
        auto val = result[1];
        config.setField(key, val);
        if (!std::getline(fin, line)) break;
      }
      if (line[0] != '=') throw std::runtime_error("parsing error on feature evaluation config (2)");
    }
    if (line.find("MoveEvaluatorGroup used: ") == 0) {
      std::vector<std::string> spl1, spl2;
      boost::algorithm::split_regex(spl1, line, boost::regex("MoveEvaluatorGroup used: "));
      boost::split(spl2, spl1[1], boost::is_any_of(" "));
      group = spl2[0];
    }

    std::vector<std::string> spl;
    boost::split(spl, line, boost::is_any_of(","));
    if (spl.size() > 1) {
      weights = spl.back();
    }
  }

  if (config.numLines == Config::UNDEF) {
    throw std::runtime_error("error parsing config (numLines)");
  }
  if (group == "") {
    throw std::runtime_error("error parsing group");
  }
  if (weights == "") {
    throw std::runtime_error("error parsing weights");
  }

  Weighting w = WeightingFn::readFromString(weights);

  return {config, group, w};
}
