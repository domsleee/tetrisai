#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"
#include "third_party/libfort/fort.hpp"
#include "src/pso/stats.h"
#include <iomanip>
#include <sstream>
#include "src/pso/defs.h"
#include "src/pso/main/RowGenerator/RowGenerator.h"
#include "src/pso/main/RowGenerator/rows/AllRows.h"
#include <chrono>
#include <ctime>
#include <fstream>

fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getTransitionLinesComparison(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getBfsData(const SummaryApi &s, const std::vector<std::string> &names);


int main() {
  auto t = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(t);
  std::cout << "Current time: " << std::ctime(&tt);

  SummaryApi s(DATA_FOLDER);
  auto names = s.getAllNames();
  printf("num_files: %lu\n", names.size());
  std::sort(names.begin(), names.end());
  std::cout << getData(s, names).to_string() << '\n';
  // std::cout << getTransitionLinesComparison(s, names).to_string() << '\n';
  // std::cout << getPairsData(s, names).to_string() << '\n';
  // std::cout << getLookaheadData(s, names).to_string() << '\n';  
  std::cout << getBfsData(s, names).to_string() << '\n';
  auto endTime = std::chrono::system_clock::now();

  std::cout << "Time taken: " << toFixed((double)getMs(endTime-t)/1000, 1) << '\n';
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
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercGamesCleared>()
  });
  r.applyHeader();

  for (auto name: names) {
    //if (strcmp(name.c_str(), "19_BothLinearGen.log") != 0) continue;
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
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercentileRow>(70),
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
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercentileRow>(70),
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
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercGamesCleared>(130),
    std::make_shared<PercGamesCleared>(230),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  for (auto name1: names) {
    for (auto name2: names) {
      if (name1 == name2) continue;
      if (name1[1] == '8' && name2[1] == '9') {
        //if (strcmp("18_LinearGen.log", name1.c_str()) != 0) continue;
        //if (strcmp("19_BothLinearGen.log", name2.c_str()) != 0) continue;
        eprintf("processing: %s\n", name1.c_str());
        r.setFns({
          [&]() { return s.getSummaryLookahead(name1, name2, 90); },
        });
        r.calculateAndApplyFeatures();
      }
    }
  }
  return r.getTable();
}


fort::char_table getBfsData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "BFS DATA\n";
  fort::char_table table;
  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercGamesCleared>(130),
    std::make_shared<PercGamesCleared>(230),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  for (auto name1: names) {
    if (strcmp("19_Bfs.log", name1.c_str()) != 0) continue;
    eprintf("processing: %s\n", name1.c_str());
    r.setFns({
      [&]() { return s.getSummaryLookahead(name1, name1, 90); },
    });
    auto results = r.calculateAndApplyFeatures();
    auto result = results[0];
    std::ofstream fout{"/tmp/" + name1 + ".csv"};
    fout << "score,level,totalLines\n";
    for (auto score: result.getScoreManagers()) {
      fout << score.getScore() << ',' << score.getLevel() << ',' << score.getTotalLines() << '\n';
    }
    result.getScoreManagers();
    fout.close();
  }
  return r.getTable();
}

