#pragma once
#include "src/board/bitboard/BitBoard.h"

std::pair<bool, int> getMinBlock(int *colHeights, int level=DEFAULT_LEVEL);
std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b);
bool isColAccessible(int *colHeights, int level);
