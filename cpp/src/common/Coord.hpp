#pragma once
struct Coord {
  int r, c;
  Coord(int r, int c) : r(r), c(c) {}
  Coord& operator+=(const Coord &coord) {
    r += coord.r;
    c += coord.c;
    return *this;
  }
  friend Coord operator+(Coord lhs, const Coord& rhs) {
    lhs += rhs;
    return lhs;
  }
  Coord& operator-=(const Coord &coord) {
    r -= coord.r;
    c -= coord.c;
    return *this;
  }
  friend Coord operator-(Coord lhs, const Coord& rhs) {
    lhs -= rhs;
    return lhs;
  }
  friend bool operator==(const Coord& lhs, const Coord &rhs) {
    return lhs.r == rhs.r && lhs.c == rhs.c;
  }
  void print() const {
    printf("Coord: (%d, %d)\n", r, c);
  }
};

