#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "src/common/Coord.hpp"

struct Move {
  int maxR = 0;
  std::vector<Coord> coords_;
  Move(const std::vector<Coord>& coords) {
    if (coords.size() != 4) {
      throw new std::runtime_error{"require vector of size 4"};
    }
    for (auto coord: coords) {
      maxR = std::max(maxR, coord.r);
    }
    coords_ = coords;
  }
  Move() {}
  
  Move& operator+=(Coord c) {
    for (auto& coord: coords_) {
      coord += c;
      maxR = std::max(maxR, coord.r);
    }
    return *this;
  }
  friend bool operator==(const Move& m1, const Move &m2) {
    return m1.coords_ == m2.coords_;
  }
  void print() const {
    printf("Move: ");
    for (const auto &coord: coords_) {
      printf("(%d, %d), ", coord.r, coord.c);
    }
    printf("\n");
  }
};

namespace std {
  template <>
  struct hash<Move> {
    std::size_t operator()(const Move& k) const {
      long long mult = 1;
      std::size_t h = 0;
      for (const auto& coord: k.coords_) {
        int v = coord.c*20+coord.r;
        h += mult*v;
        mult *= 200;
      }
      return h;
    }
  };

}