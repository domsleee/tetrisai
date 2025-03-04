#pragma once

#include <string>

#define DISABLE_TEST return

#ifndef WORKSPACE_DIR
#define WORKSPACE_DIR "NO_WORKSPACE_DIR"
#endif
const std::string TEST_FOLDER = std::string(WORKSPACE_DIR) + std::string("/src/shared/test/data");

//#define MY_DEBUG 1

#ifdef MY_DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

// todo: get rid of this
#define DEFAULT_LEVEL 19

const int NUM_ROWS = 22;
const int NUM_COLUMNS = 10;

const int NUM_GAMES = 100;

// the best for 6 HIGH clears
const std::string basic_weighting_best = "[-1.2423329732739021 9.882528354462208 10.026907431261453 0.763487560179372 -1.8342832202658315 4.6853627614536695 4.146888881587456 -0.9576377660403121 1.085749501319249 3.3613335999923937 11.803244854482699 -1.8226403408651448 -5.815748199178253 -11.8709211557212 -5.814356944759763 1.2293384348879324 -2.679788083005798]";

// average
template<typename T>
double average(T it1, T it2);

static inline int R(int r) { return NUM_ROWS - r - 1; }

#include "src/common/common.tpp"