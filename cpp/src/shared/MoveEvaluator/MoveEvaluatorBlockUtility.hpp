#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"
#include "src/shared/MoveEvaluator/VacancyChecker.hpp"

#define EVAL(b, p, level) evaluateMine(b, p, EvaluatorInfo(b, p, ScoreManager(level)))


std::pair<bool, int> getMinBlock(const int *colHeights, int maxDropRem=2);
int getMaxColHeightsMinusClearHeightsAll(const int *colHeights, int maxDropRem);
int getMaxColHeightsMinusClearHeightsRange(const int *colHeights, int dropRem, int l, int r);
std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b);
std::array<int, NUM_COLUMNS> getColHeights(const VacancyChecker &vac);

std::array<bool, NUM_COLUMNS> getIsColAccessible(const int *colHeights, int dropRem);
bool isBottomColAccessible(const int *colHeights, int maxDropRem);
std::pair<int, int> getMinColumns(const int *colHeights);
bool isDeepWell(int smallestHeight, int secondSmallestHeight);

template <typename T>
double evaluateWithEmptyPiece(const T& me, const BitBoard &b) {
  return me.evaluateMine(b, b.getEmptyPiece(), {b, b.getEmptyPiece(), ScoreManager(19)});
}
