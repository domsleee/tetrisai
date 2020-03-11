#include <queue>
#include <vector>
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoard.h"

static constexpr int MAX_CLEAR_HEIGHTS[NUM_COLUMNS] = {
  0, // unused
  8, // for 0
  11, // for 1
  15, // for 2
  17, // for 3
  17, // for 4/6
  17, // for 7
  15, // for 8
  11, // for 9
  0 // unused
};

static constexpr int MAX_CLEAR_HEIGHTS18[NUM_COLUMNS] = {
  0, // unused
  12,
  14,
  17,
  17,
  17, // for 4/6
  17,
  17,
  14,
  0 // unused
};

// prototypes
using PairT = std::pair<int, int>;
int getMaxColHeightMinusColLimit(int *colHeights, int level, int bottomColumn);
bool isColAccessible(int *colHeights, int level, int bottomColumn);
bool wellIsTooHigh(PairT lowestColumn);


int getMinColumn(int *colHeights) {
  int index = 0, value = colHeights[0];
  for (int c = 1; c < NUM_COLUMNS; ++c) {
    if (colHeights[c] < value) {
      value = colHeights[c];
      index = c;
    }
  }
  return index;
}

std::pair<int, int> getMinColumns(int *colHeights) {
  std::priority_queue<PairT, std::vector<PairT>, std::greater<PairT>> pq;
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    pq.push({colHeights[c], c});
  }
  auto [bottomColumnHeight, bottomColumn] = pq.top(); pq.pop();
  auto [secondColumnHeight, secondColumn] = pq.top(); pq.pop();
  return {bottomColumn, secondColumn};
}

// todo: use std::array to enforce array size.
std::pair<bool, int> getMinBlock(int *colHeights, int level) {
  auto [bottomColumn, secondColumn] = getMinColumns(colHeights);

  auto bottomColumnHeight = colHeights[bottomColumn];
  auto secondColumnHeight = colHeights[secondColumn];

  if (!isDeepWell(bottomColumnHeight, secondColumnHeight)) return {false, 0};
  if (bottomColumn == 5) return {false, 0};
  if (wellIsTooHigh({bottomColumnHeight, bottomColumn})) return {false, 0}; // you're screwed here haha

  return {true, -getMaxColHeightMinusColLimit(colHeights, level, bottomColumn)};
}

bool isDeepWell(int smallestHeight, int secondSmallestHeight) {
  assert(smallestHeight <= secondSmallestHeight);
  return secondSmallestHeight - smallestHeight >= 3;
}

bool wellIsTooHigh(PairT lowestColumn) {
  auto [bottomColumnHeight, bottomColumn] = lowestColumn;
  return bottomColumnHeight > MAX_CLEAR_HEIGHTS[bottomColumn];
}

bool isColAccessible(int *colHeights, int level) {
  auto bottomColumn = getMinColumn(colHeights);
  return isColAccessible(colHeights, level, bottomColumn);
}

bool isColAccessible(int *colHeights, int level, int bottomColumn) {
  return getMaxColHeightMinusColLimit(colHeights, level, bottomColumn) <= 0;
}

int getMaxColHeightMinusColLimit(int *colHeights, int level, int bottomColumn) {
  bool isRight = bottomColumn > 5;
  const int *maxClearHeights = level == 18 ? MAX_CLEAR_HEIGHTS18 : MAX_CLEAR_HEIGHTS;

  int maxColAboveLimit = -20;
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
