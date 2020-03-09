#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"
#include "third_party/libfort/fort.hpp"
#include "src/pso/stats.h"

const std::string DATA_FOLDER = WORKSPACE_DIR + std::string("/../python/named_logs");

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names);

int main() {
  SummaryApi s(DATA_FOLDER);
  auto names = s.getAllNames();
    printf("num_files: %lu\n", names.size());
  std::sort(names.begin(), names.end());

  
  std::cout << getData(s, names).to_string() << std::endl;
  std::cout << "PAIRS DATA\n";
  std::cout << getPairsData(s, names).to_string() << '\n';
  
}

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names) {
  fort::char_table table;
  table << fort::header << "name" << "group" << "top50Average" << "median" << fort::endr;
  for (auto name: names) {
    printf("processing: %s\n", name.c_str());
    auto summary = s.getSummary(name);
    auto top50Avg = getTopAverage(summary.getScores(), 50);
    auto median = getPercentile(summary.getScores(), 50);
    table << name << summary.getGroup() << top50Avg << median << fort::endr;
  }
  return table;
}

fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names) {
  fort::char_table table;
  table << fort::header << "name" << "group" << "top50Average" << "median" << fort::endr;

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        auto summary = s.getSummaryLookahead(name, otherName, 90);
        auto top50Avg = getTopAverage(summary.getScores(), 50);
        auto median = getPercentile(summary.getScores(), 50);
        table << name << summary.getGroup() << top50Avg << median << fort::endr;
      }
    }
  }
  return table;
}