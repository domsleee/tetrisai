#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include <iostream>
#include <fstream>

BitBoard readBoard(const std::string &filePath) {
  std::ifstream fin{filePath};
  auto vs = std::vector<std::vector<int>>(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
  char t;
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLUMNS; c++) {
      fin >> t;
      vs[r][c] = t-'0';
    }
  }
  return {vs};
}

Weighting getExpectedWeights(const std::string &filepath) {
  std::ifstream fin{filepath};
  Weighting w;
  double d;
  while (fin >> d) w.push_back(d);
  printf("filepath: %s\n", filepath.c_str());
  printf("size: %lu\n", w.size());
  assert(w.size() > 0);
  return w;
}


BitBoard getBoardFromPartialStringVector(std::vector<std::string> strings) {
  auto vs = std::vector<std::vector<int>>(NUM_ROWS, std::vector<int>(NUM_COLUMNS, 0));
  int rOffset = NUM_ROWS - strings.size();
  int r = rOffset;
  for (;r < NUM_ROWS; ++r) {
    const auto &s = strings[r-rOffset];
    assert(s.size() <= NUM_COLUMNS);
    for (int c = 0; c < static_cast<int>(s.size()); ++c) {
      if (s[c] == '1') {
        vs[r][c] = 1;
      }
    }
  }
  return {vs};
}



std::vector<std::vector<int>> leftWell(int height) {
  std::vector<std::vector<int>> vs;
  for (int i = 0; i < NUM_ROWS-height; i++) {
    vs.push_back(std::vector<int>(NUM_COLUMNS, 0));
  }
  std::vector<int> leftFree = std::vector<int>(NUM_COLUMNS, 1);
  leftFree[0] = 0;
  for (int i = 0; i < height; i++) {
    vs.push_back(leftFree);
  }
  return vs;
}
