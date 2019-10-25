#include "src/pso/EvaluateWeightings.h"
#include "src/board/bitboard/BitBoardPre.h"
#include <chrono>
#include <iostream>

int main() {
  EvaluateWeightings ew;

  auto startTime1 = std::chrono::system_clock::now();
  BitBoardPre::precompute();
  auto endTime1 = std::chrono::system_clock::now();
  std::chrono::duration<double> duration1 = endTime1-startTime1;
  std::cout << "time taken for precompute: " << duration1.count() << "s\n";

  auto startTime = std::chrono::system_clock::now();
  std::vector<double> w(NUM_FACTORS, 0);
  ew.rankWeightings({w});
  auto endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = endTime-startTime;
  std::cout << "time taken for one evaluation: " << duration.count() << "s\n";
}

