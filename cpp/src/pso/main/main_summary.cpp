#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"
#include "third_party/libfort/fort.hpp"
#include "src/pso/stats.h"
#include <iomanip>
#include <sstream>
#include "src/pso/defs.h"
#include "src/pso/main/RowGenerator/RowGenerator.h"
#include "src/pso/main/RowGenerator/rows/AllRows.h"
#include "src/shared/BranchSearcher/defs.h"
#include <chrono>
#include <ctime>
#include <fstream>      // std::ofstream

fort::char_table getNoroData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getNoroLookaheadData(const SummaryApi &s, const std::vector<std::string> &names, int depthWithKnownBlock);
fort::char_table getData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getTransitionLinesComparison(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getPairsData(const SummaryApi &s, const std::vector<std::string> &names);
fort::char_table getLookaheadData(const SummaryApi &s, const std::vector<std::string> &names);

int main() {
  auto t = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(t);
  std::cout << "Current time: " << std::ctime(&tt);

  SummaryApi s(DATA_FOLDER);
  auto names = s.getAllNames();
  printf("num_files: %lu\n", names.size());
  std::sort(names.begin(), names.end());
  std::cout << getNoroData(s, names).to_string() << '\n' << std::flush;
  std::cout << getNoroLookaheadData(s, names, 1).to_string() << std::endl;
  std::cout << getNoroLookaheadData(s, names, 2).to_string() << std::endl;
  std::cout << getNoroLookaheadData(s, names, 3).to_string() << std::endl;
  std::cout << getNoroLookaheadData(s, names, 4).to_string() << std::endl;
  std::cout << getNoroLookaheadData(s, names, 5).to_string() << std::endl;

  // std::cout << getData(s, names).to_string() << '\n';
  // std::cout << getTransitionLinesComparison(s, names).to_string() << '\n';
  // std::cout << getPairsData(s, names).to_string() << '\n';
  // std::cout << getLookaheadData(s, names).to_string() << '\n';  
  auto endTime = std::chrono::system_clock::now();

  std::cout << "Time taken: " << toFixed((double)getMs(endTime-t)/1000, 1) << '\n';
}

fort::char_table getNoroData(const SummaryApi &s, const std::vector<std::string> &names) {
  std::cout << "Noro DATA\n";
  fort::char_table table;

  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercGamesCleared>()
  });
  r.applyHeader();

  for (auto name: names) {
    //if (strcmp(name.c_str(), "19_BothLinearGen.log") != 0) continue;
    eprintf("processing: %s\n", name.c_str());
    r.setFn([&]{ return s.getSummaryAllMoveFinder(name); });
    r.calculateAndApplyFeatures();
  }
  return r.getTable();
}

void getBranchSearchDefs(std::ostream &fout) {
  fout << "\nBranchSearchDefs:\n--------\n";
  fout << "TOP_N_BLOCKS: " << BranchSearcherDefs::TOP_N_BLOCKS << '\n';
  fout << "INIT_TOP_N_BLOCKS: " << BranchSearcherDefs::INIT_TOP_N_BLOCKS << '\n';
  fout << "DEPTH: " << BranchSearcherDefs::DEPTH << '\n';
  fout << "DEPTH_WITH_KNOWN_BLOCK: " << BranchSearcherDefs::DEPTH_WITH_KNOWN_BLOCK << '\n';
  fout << '\n';
}

void outputResults(const std::vector<SummaryResult> &res, const std::string &filename) {
  std::cout << "OUTPUTTING RESULTS to " << filename << "...\n";
  std::ofstream ofs(filename, std::ofstream::out);
  getBranchSearchDefs(ofs);
  ofs << "score, level, totalLines\n";
  for (auto r: res) {
    ofs << "NEW RES\n";
    for (auto sm: r.getScoreManagers()) {
      ofs << sm.getTotalLines() << '\n';
    }
  }
  ofs.close();
}

fort::char_table getNoroLookaheadData(const SummaryApi &s, const std::vector<std::string> &names, int depthWithKnownBlock) {
  std::cout << "Noro lookahead DATA\n";
  fort::char_table table;

  BranchSearcherDefs::DEPTH_WITH_KNOWN_BLOCK = depthWithKnownBlock;
  getBranchSearchDefs(std::cout);

  auto r = RowGenerator(table, {
    std::make_shared<Name>(),
    std::make_shared<Group>(),
    std::make_shared<Seed>(),
    std::make_shared<NumGames>(),
    std::make_shared<TopAverage>(50),
    std::make_shared<PercentileRow>(70),
    std::make_shared<PercentileRow>(50),
    std::make_shared<PercGamesCleared>()
  });
  r.applyHeader();

  for (auto name: names) {
    //if (strcmp(name.c_str(), "19_BothLinearGen.log") != 0) continue;
    eprintf("processing: %s\n", name.c_str());
    r.setFn([&]{ return s.getSummaryAllMoveFinderLookahead(name); });
    r.calculateAndApplyFeatures();
    outputResults(r.getResult(), std::string(WORKSPACE_DIR) + "/summary_reports/" + name + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
  }
  return r.getTable();
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
    std::make_shared<PercentileRow>(50),
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
    std::make_shared<PercGamesCleared>(130),
    std::make_shared<PercGamesCleared>(230),
    std::make_shared<MaxoutRate>()
  });
  r.applyHeader();

  /*for (auto name: names) {
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
  }*/
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
