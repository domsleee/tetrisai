#include "src/pso/PieceSetGetter.hpp"
#include <iostream>

int main() {
  PieceSetGetter ps;
  auto sets = ps.getPieceSets(100);
  for (const auto &game: sets) {
    for (int i = 0; i < static_cast<int>(game.size()); ++i) {
      std::cout << game[i];
      if (i != static_cast<int>(game.size())-1) std::cout << ' ';
    }
    std::cout << '\n';
  }
}
