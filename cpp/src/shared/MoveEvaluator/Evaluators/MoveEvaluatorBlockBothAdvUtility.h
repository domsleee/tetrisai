//
// Created by Dom on 2019-11-25.
//

#pragma once
#include "src/common/common.hpp"
#include <array>

int getNumBlockedRows(const int* colHeights, const std::array<bool, NUM_COLUMNS> &isColAccessible);
