#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Coord.hpp"

#include <vector>

class VacancyChecker {
 public:
  VacancyChecker(const BitBoard &b): b_(b) {};
  bool is_vacant(const Coord &coord) const {
    int ind = coord.r * NUM_COLUMNS + coord.c;
    if (vacant_[ind] == UNDEF_) vacant_[ind] = b_.vacant(coord);
    return vacant_[ind];
  }
 private:
  static const char UNDEF_;
  BitBoard b_;
  mutable std::vector<char> vacant_ = decltype(vacant_)(NUM_ROWS * NUM_COLUMNS, UNDEF_);
};
