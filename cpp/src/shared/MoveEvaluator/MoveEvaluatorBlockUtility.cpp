#include <queue>
#include <vector>
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoard.h"

static constexpr int MAX_CLEAR_HEIGHTS19[NUM_COLUMNS] = {
  0, // unused
  8, // for 0
  11, // for 1
  15, // for 2
  18, // for 3
  19, // for 4/6
  18, // for 7
  15, // for 8
  11, // for 9
  0 // unused
};

static constexpr int MAX_CLEAR_HEIGHTS18[NUM_COLUMNS] = {
  0, // unused
  12,
  14,
  17,
  18,
  19, // for 4/6
  18,
  17,
  14,
  0 // unused
};

static constexpr int MAX_CLEAR_HEIGHTS29[NUM_COLUMNS] = {
  0, // unused
  0, // for 0
  4,
  11,
  16,
  19, // for 4/6
  16,
  11,
  4,
  0 // unused
};

const int* getMaxClearHeights(int dropRem) {
  switch(dropRem) {
    case 3: return MAX_CLEAR_HEIGHTS18;
    case 2: return MAX_CLEAR_HEIGHTS19;
    case 1: return MAX_CLEAR_HEIGHTS29;
    default: throw std::runtime_error("unknown droprem");
  }
}

// prototypes
using PairT = std::pair<int, int>;
int getMaxColHeightMinusColLimit(const int *colHeights, int dropRem, int bottomColumn);
bool isBottomColAccessible(const int *colHeights, int dropRem, int bottomColumn);
bool wellIsTooHigh(PairT lowestColumn);


int getMinColumn(const int *colHeights) {
  int index = 0, value = colHeights[0];
  for (int c = 1; c < NUM_COLUMNS; ++c) {
    if (colHeights[c] < value) {
      value = colHeights[c];
      index = c;
    }
  }
  return index;
}

std::pair<int, int> getMinColumns(const int *colHeights) {
  int firstC = -1, secondC = -1;
  if (colHeights[0] < colHeights[1]) {
    firstC = 0, secondC = 1;
  } else {
    firstC = 1, secondC = 0;
  }

  for (int c = 2; c < NUM_COLUMNS; ++c) {
    if (colHeights[c] <= colHeights[firstC]) {
      secondC = firstC;
      firstC = c;
    }
    else if (colHeights[c] <= colHeights[secondC]) {
      secondC = c;
    }
  }
  return {firstC, secondC};
}

// todo: use std::array to enforce array size.
std::pair<bool, int> getMinBlock(const int *colHeights, int level) {
  auto [bottomColumn, secondColumn] = getMinColumns(colHeights);

  auto bottomColumnHeight = colHeights[bottomColumn];
  auto secondColumnHeight = colHeights[secondColumn];

  if (!isDeepWell(bottomColumnHeight, secondColumnHeight)) return {false, 0};
  if (bottomColumn == 5) return {false, 0};
  if (wellIsTooHigh({bottomColumnHeight, bottomColumn})) return {false, 0}; // you're screwed here haha

  return {true, -getMaxColHeightMinusColLimit(colHeights, level, bottomColumn)};
}

int getMaxColHeightsMinusClearHeightsRange(const int *colHeights, int dropRem, int l, int r) {
  int M = -20;
  const int *maxClearHeights = getMaxClearHeights(dropRem);
  for (int c = l; c <= r; ++c) {
    M = std::max(M, colHeights[c] - maxClearHeights[c]);
  }
  return M;
}

std::array<bool, NUM_COLUMNS> getIsColAccessible(const int *colHeights, int dropRem) {
  const int *maxClearHeights = getMaxClearHeights(dropRem);
  std::array<bool, NUM_COLUMNS> res;
  if (colHeights[5] > maxClearHeights[5]) {
    for (int i = 0; i < NUM_COLUMNS; ++i) res[i] = false;
    return res;
  }
  res[5] = true;
  bool leftOk = true;
  for (int c = 4; c >= 0; --c) {
    if (colHeights[c] > maxClearHeights[c]) {
      leftOk = false;
    }
    res[c] = leftOk;
  }

  bool rightOk = true;
  for (int c = 6; c < NUM_COLUMNS; ++c) {
    if (colHeights[c] > maxClearHeights[c]) {
      rightOk = false;
    }
    res[c] = rightOk;
  }
  return res;
}

int getMaxColHeightsMinusClearHeightsAll(const int *colHeights, int dropRem) {
  return getMaxColHeightsMinusClearHeightsRange(colHeights, dropRem, 1, NUM_COLUMNS-2);
}

bool isDeepWell(int smallestHeight, int secondSmallestHeight) {
  assert(smallestHeight <= secondSmallestHeight);
  return secondSmallestHeight - smallestHeight >= 3;
}

bool wellIsTooHigh(PairT lowestColumn) {
  auto [bottomColumnHeight, bottomColumn] = lowestColumn;
  return bottomColumnHeight > MAX_CLEAR_HEIGHTS19[bottomColumn];
}

bool isBottomColAccessible(const int *colHeights, int dropRem) {
  auto bottomColumn = getMinColumn(colHeights);
  return isBottomColAccessible(colHeights, dropRem, bottomColumn);
}

bool isBottomColAccessible(const int *colHeights, int dropRem, int bottomColumn) {
  // printf("dropRem (isBottomColAccessible): %d (result: %d)\n", dropRem, getMaxColHeightMinusColLimit(colHeights, dropRem, bottomColumn));
  if (bottomColumn == 5) {
    for (int c = 3; c <= 6; ++c) {
      //int fallDistBeforeRot = dropRem == 1 ? 3 : (dropRem == 2 ? 1 : 0);
      int fallDistBeforeRot = 2;
      int failHeight = NUM_ROWS - fallDistBeforeRot;
      if (colHeights[c] >= failHeight) return false;
    }
    return true;
  }

  return getMaxColHeightMinusColLimit(colHeights, dropRem, bottomColumn) <= 0;
}

int getMaxColHeightMinusColLimit(const int *colHeights, int dropRem, int bottomColumn) {
  bool isRight = bottomColumn > 5;
  const int *maxClearHeights = getMaxClearHeights(dropRem);

  int maxColAboveLimit = -1e9;
  if (isRight) {
    for (int c = 5; c < bottomColumn; ++c) {
      maxColAboveLimit = std::max(maxColAboveLimit, colHeights[c] - maxClearHeights[c]);
    }
  } else {
    for (int c = 5; c > bottomColumn; --c) {
      maxColAboveLimit = std::max(maxColAboveLimit, colHeights[c] - maxClearHeights[c]);
    }
  }
  return maxColAboveLimit;
}

std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b) {
  std::array<int, NUM_COLUMNS> colHeights;
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    colHeights[c] = 0;
    for (int r = 0; r < NUM_ROWS; ++r) {
      if (!b.vacant({r, c})) {
        colHeights[c] = NUM_ROWS - r;
        break;
      }
    }
  }
  return colHeights;
}

std::array<int, NUM_COLUMNS> getColHeights(const VacancyChecker &vac) {
  std::array<int, NUM_COLUMNS> colHeights;
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    colHeights[c] = 0;
    for (int r = 0; r < NUM_ROWS; ++r) {
      if (!vac.is_vacant({r, c})) {
        colHeights[c] = NUM_ROWS - r;
        break;
      }
    }
  }
  return colHeights;
}

