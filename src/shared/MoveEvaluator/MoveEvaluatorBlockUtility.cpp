#include <queue>
#include <vector>
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoard.h"

static constexpr int MAX_CLEAR_HEIGHTS[NUM_COLUMNS] = {
  0, // unused
  8,
  11,
  14,
  17,
  0, // unused
  17,
  14,
  11,
  0 // unused
};

static constexpr int MAX_CLEAR_HEIGHTS18[NUM_COLUMNS] = {
  0, // unused
  12,
  14,
  16,
  18,
  0, // unused
  18,
  16,
  14,
  0 // unused
};

using PairT = std::pair<int, int>;

bool isDeepWell(int smallestHeight, int secondSmallestHeight) {
  assert(smallestHeight <= secondSmallestHeight);
  return secondSmallestHeight - smallestHeight >= 3;
}

bool wellIsTooHigh(PairT lowestColumn) {
  auto [bottomColumnHeight, bottomColumn] = lowestColumn;
  return bottomColumnHeight > MAX_CLEAR_HEIGHTS[bottomColumn];
}

// todo: use std::array to enforce array size.
std::pair<bool, int> getMinBlock(int *colHeights, int level) {
  // min heap
  std::priority_queue<PairT, std::vector<PairT>, std::greater<PairT>> pq;
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    pq.push({colHeights[c], c});
  }

  auto lowestColumn = pq.top(); pq.pop();
  auto [bottomColumnHeight, bottomColumn] = lowestColumn;
  auto [secondColumnHeight, secondColumn] = pq.top(); pq.pop();

  if (!isDeepWell(bottomColumnHeight, secondColumnHeight)) return {false, 0};
  if (bottomColumn == 5) return {false, 0};
  if (wellIsTooHigh(lowestColumn)) return {false, 0}; // you're screwed here haha

  bool isRight = bottomColumn > 5;
  int direction = isRight ? 1 : -1;

  assert(level == 18 || level == 19);
  const int *maxClearHeights = level == 18 ? MAX_CLEAR_HEIGHTS18 : MAX_CLEAR_HEIGHTS;

  int minBlock = -20;
  if (isRight) {
    for (int c = 6; c < bottomColumn; ++c) {
      minBlock = std::max(minBlock, colHeights[c] - maxClearHeights[c]);
    }
  } else {
    for (int c = 4; c > bottomColumn; --c) {
      minBlock = std::max(minBlock, colHeights[c] - maxClearHeights[c]);
    }
  }
  return {true, -minBlock};
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
