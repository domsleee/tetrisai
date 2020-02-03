#include <queue>
#include <vector>
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/common/common.hpp"

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

// todo: use std::array to enforce array size.
std::pair<bool, int> getMinBlock(int *colHeights) {
  using PairT = std::pair<int, int>;
  // min heap
  std::priority_queue<PairT, std::vector<PairT>, std::greater<PairT>> pq;
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    pq.push({colHeights[c], c});
  }
  auto [bottomColumnHeight, bottomColumn] = pq.top(); pq.pop();
  auto [secondColumnHeight, secondColumn] = pq.top(); pq.pop();

  if (secondColumnHeight - bottomColumnHeight < 3) return {false, 0};
  if (bottomColumn == 5) return {false, 0};
  if (MAX_CLEAR_HEIGHTS[bottomColumn] > bottomColumnHeight) return {false, 0}; // you're screwed here haha

  bool isRight = bottomColumn > 5;
  int direction = isRight ? 1 : -1;

  int minBlock = 20;
  if (isRight) {
    for (int c = 6; c < bottomColumn; ++c) {
      minBlock = std::min(minBlock, MAX_CLEAR_HEIGHTS[c] - colHeights[c]);
    }
  } else {
    for (int c = 4; c > bottomColumn; --c) {
      minBlock = std::min(minBlock, MAX_CLEAR_HEIGHTS[c] - colHeights[c]);
    }
  }
  return {true, minBlock};
}