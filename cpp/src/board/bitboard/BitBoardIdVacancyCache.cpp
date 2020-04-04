#include "src/board/bitboard/BitBoardIdVacancyCache.h"

char BitBoardIdVacancyCache::get(int id) const {
  assert(0 <= id && id < BitBoardPre::NUM_INDEXES);
  return cache_[id];
}

char BitBoardIdVacancyCache::set(int id, char val) {
  assert(0 <= id && id < BitBoardPre::NUM_INDEXES);
  return cache_[id] = val;
}

const char BitBoardIdVacancyCache::UNDEF = 'z';