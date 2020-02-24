#pragma once
#include "src/board/bitboard/BitBoard.h"

std::pair<bool, int> getMinBlock(int *colHeights);
std::array<int, NUM_COLUMNS> getColHeights(const BitBoard &b);
