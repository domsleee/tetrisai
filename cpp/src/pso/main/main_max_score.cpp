#include "src/shared/ScoreManager.hpp"
#include <iostream>

int main() {
  ScoreManager sm(19);
  while (sm.getTotalLines() < 230) {
    sm.addLineClears(4);
  }
  std::cout << sm.getScore() << '\n';
}