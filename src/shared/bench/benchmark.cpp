
#include "src/board/SimpleBoard.h"

#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinder.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>


int main() {
  std::vector<std::vector<int>> vs;
  for (int i = 0; i < 16; i++) {
    vs.push_back(std::vector<int>(10, 0));
  }
  std::vector<int> leftFree = std::vector<int>(10, 1);
  leftFree[0] = 0;
  for (int i = 0; i < 4; i++) {
    vs.push_back(leftFree);
  }

  auto startTime = std::chrono::system_clock::now();

  const int NUM_RUNS = 25;
  BitBoardPre::precompute();
  for (int i = 0; i < NUM_RUNS; i++) {
    BitBoard b(vs);
    MoveFinder mf;
    auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
  }
  auto endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = endTime-startTime;
  std::cout << NUM_RUNS << " runs in " << duration.count() << "s\n";
  std::cout << "average: " << duration.count()/NUM_RUNS << "s\n";

}
