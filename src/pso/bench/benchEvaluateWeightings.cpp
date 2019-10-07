#include "src/pso/EvaluateWeightings.h"
#include <chrono>
#include <iostream>

int main() {
  EvaluateWeightings ew;
  auto startTime = std::chrono::system_clock::now();
  ew.rankWeightings({{}});
  auto endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = endTime-startTime;
  std::cout << "time taken for one evaluation: " << duration.count() << "s\n";
}
