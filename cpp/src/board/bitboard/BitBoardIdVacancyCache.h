#pragma once

#include <cassert>
#include "src/board/bitboard/BitBoardPre.h"

class BitBoardIdVacancyCache {
 public:
  char get(int id) const;
  char set(int id, char val);
  static const char UNDEF;
 private:
  std::vector<char> cache_ = std::vector<char>(BitBoardPre::NUM_INDEXES, UNDEF);
};
