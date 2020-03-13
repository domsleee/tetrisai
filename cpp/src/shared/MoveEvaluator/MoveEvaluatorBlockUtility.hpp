#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"

std::pair<bool, int> getMinBlock(int *colHeights, int level=DEFAULT_LEVEL);
std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b);
bool isColAccessible(int *colHeights, int level);
std::pair<int, int> getMinColumns(int *colHeights);
bool isDeepWell(int smallestHeight, int secondSmallestHeight);

template <typename T>
double evaluateWithEmptyPiece(const T& me, const BitBoard &b) {
  return me.evaluateMine(b, b.getEmptyPiece(), EvaluatorInfo(19));
}
