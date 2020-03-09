#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"
#include "third_party/libfort/fort.hpp"
#include "src/pso/stats.h"
#include <iomanip>
#include <sstream>
#include "src/pso/defs.h"

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names);
std::string toFixed(double val, int dp);

int main() {
  SummaryApi s(DATA_FOLDER);
  auto names = s.getAllNames();
    printf("num_files: %lu\n", names.size());
  std::sort(names.begin(), names.end());

  
  std::cout << getData(s, names).to_string() << std::endl;
  std::cout << getPairsData(s, names).to_string() << '\n';
  std::cout << getLookaheadData(s, names).to_string() << '\n';
  
}

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "STRAIGHT DATA\n";
  fort::char_table table;
  table << fort::header << "name" << "group" << "top50Average" << "70th perc" << "median" << fort::endr;
  for (auto name: names) {
    eprintf("processing: %s\n", name.c_str());
    auto summary = s.getSummary(name);
    auto top50Avg = getTopAverage(summary.getScores(), 50);
    auto median = getPercentile(summary.getScores(), 50);
    auto perc70 = getPercentile(summary.getScores(), 70);
    table << name << summary.getGroup() << top50Avg << perc70 << median << fort::endr;
  }
  return table;
}

fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "FSM DATA\n";
  fort::char_table table;
  table << fort::header << "name" << "group" << "top50Average" << "70th perc" << "median" << "maxout %" << fort::endr;

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        eprintf("processing: %s\n", name.c_str());
        auto summary = s.getSummaryFSM(name, otherName, 90);
        auto summary2 = s.getSummary(name, otherName, 90);

        auto top50Avg = getTopAverage(summary.getScores(), 50);
        auto top50Avg2 = getTopAverage(summary2.getScores(), 50);

        auto median = getPercentile(summary.getScores(), 50);
        auto median2 = getPercentile(summary2.getScores(), 50);

        auto perc70 = getPercentile(summary.getScores(), 70);
        auto perc702 = getPercentile(summary2.getScores(), 70);

        auto numMaxouts = 0;
        for (auto score: summary.getScores()) numMaxouts += score >= 999999;
        auto maxoutRate = 100 * (double)numMaxouts / summary.getScores().size();
        table << name << summary.getGroup()
          << (toFixed(top50Avg, 0) + "," + toFixed(top50Avg2, 0))
          << (toFixed(perc70, 0) + "," + toFixed(perc702, 0))
          << (toFixed(median, 0) + "," + toFixed(median2, 0))
          << toFixed(maxoutRate, 2) << fort::endr;
      }
    }
  }
  return table;
}

fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "LOOKAHEAD DATA\n";
  fort::char_table table;
  table << fort::header << "name" << "group" << "top50Average" << "70th perc" << "median" << "maxout %" << fort::endr;

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        eprintf("processing: %s\n", name.c_str());
        auto summary = s.getSummaryLookahead(name, otherName, 90);
        auto top50Avg = getTopAverage(summary.getScores(), 50);
        auto median = getPercentile(summary.getScores(), 50);
        auto perc70 = getPercentile(summary.getScores(), 70);
        auto numMaxouts = 0;
        for (auto score: summary.getScores()) numMaxouts += score >= 999999;
        auto maxoutRate = 100 * (double)numMaxouts / summary.getScores().size();
        table << name << summary.getGroup() << top50Avg << perc70 << median << toFixed(maxoutRate, 2) << fort::endr;
      }
    }
  }
  return table;
}

std::string toFixed(double val, int dp) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(dp) << val;
  return ss.str();
}