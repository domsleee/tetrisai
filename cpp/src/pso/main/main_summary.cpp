#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"
#include "third_party/libfort/fort.hpp"
#include "src/pso/stats.h"
#include <iomanip>
#include <sstream>
#include "src/pso/defs.h"
#include "src/pso/main/RowGenerator/RowGenerator.h"
#include "src/pso/main/RowGenerator/rows/AllRows.h"


fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getTransitionLinesComparison(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names);

int main() {
  SummaryApi s(DATA_FOLDER);
  auto names = s.getAllNames();
    printf("num_files: %lu\n", names.size());
  std::sort(names.begin(), names.end());

  
  // std::cout << getData(s, names).to_string() << '\n';
  // std::cout << getTransitionLinesComparison(s, names).to_string() << '\n';
 //  std::cout << getPairsData(s, names).to_string() << '\n';
  std::cout << getLookaheadData(s, names).to_string() << '\n';  
}

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "STRAIGHT DATA\n";
  fort::char_table table;

  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50)
  });
  r.applyHeader();

  for (auto name: names) {
    eprintf("processing: %s\n", name.c_str());
    r.setFn([&]{ return s.getSummary(name); });
    r.calculateAndApplyFeatures();
  }
  return r.getTable();
}

fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "FSM vs Rewrite TRANSITION DATA\n";
  fort::char_table table;

  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        eprintf("processing: %s\n", name.c_str());
        r.setFns({
          [&]() { return s.getSummaryFSM(name, otherName, 90); },
          [&]() { return s.getSummary(name, otherName, 90); }
        });
        r.calculateAndApplyFeatures();
      }
    }
  }
  return r.getTable();
}

fort::char_table getTransitionLinesComparison(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "Transition lines COMPARISON\n";
  fort::char_table table;

  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        eprintf("processing: %s\n", name.c_str());
        for (int i = 0; i <= 230; i += 5) {
          eprintf("processing: %d\n", i);
          r.setFn([&]() { return s.getSummary(name, otherName, i); });
          r.calculateAndApplyFeatures();
        }
      }
    }
  }
  return r.getTable();
}

fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "LOOKAHEAD DATA\n";
  fort::char_table table;
  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  for (auto name: names) {
    if (name[1] == '8') {
      auto otherName = name;
      otherName[1] = '9';
      if (std::find(names.begin(), names.end(), otherName) != names.end()) {
        eprintf("processing: %s\n", name.c_str());
        r.setFns({
          [&]() { return s.getSummaryLookahead(name, otherName, 90); },
        });
        r.calculateAndApplyFeatures();
      }
    }
  }
  return r.getTable();
}
