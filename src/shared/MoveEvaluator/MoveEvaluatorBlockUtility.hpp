#pragma once
#include "src/board/bitboard/BitBoard.h"

std::pair<bool, int> getMinBlock(int *colHeights, int level=18);
std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b);
