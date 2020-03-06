
#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/Weighting.hpp"
#include <algorithm>
#include <numeric>

class MoveEvaluatorSimple {
 public:
  static const int AGGREGATE_HEIGHT = 0;
  static const int COMPLETE_LINES = 1;
  static const int HOLES = 2;
  static const int BUMPINESS = 3;

  static const int NUM_FACTORS = 4;

  double evaluate(const BitBoard& b, const BitPieceInfo& p, const Weighting &w) const {
    auto nxBoard = b;
    auto deltaLines = nxBoard.applyPieceInfo(p);
    assert(w.size() >= 4);

    int colHeights[NUM_COLUMNS];
    for (int c = 0; c < NUM_COLUMNS; c++) {
      colHeights[c] = 0;
      for (int r = 0; r < NUM_ROWS; r++) {
        if (!nxBoard.vacant({r,c})) {
          colHeights[c] = std::max(NUM_ROWS - r, colHeights[c]);
        }
      }
    }

    int numHoles = 0;
    for (int r = 0; r < NUM_ROWS; ++r) {
      for (int c = 0; c < NUM_COLUMNS; ++c) {
        bool takenLeft = c == 0 || !nxBoard.vacant({r, c-1});
        bool takenRight = c == NUM_COLUMNS-1 || !nxBoard.vacant({r, c+1});
        if (!nxBoard.vacant({r, c}) && takenLeft && takenRight && NUM_ROWS - r < colHeights[c]) {
          numHoles++;
        }
      }
    }

    int aggregateHeight = 0, bumpiness = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      aggregateHeight += colHeights[c];
      if (c > 0) {
        bumpiness += abs(colHeights[c] - colHeights[c-1]);
      }
    }

    double eval = 0;
    eval += w[AGGREGATE_HEIGHT] * aggregateHeight;
    eval += w[COMPLETE_LINES] * deltaLines;
    eval += w[HOLES] * numHoles;
    eval += w[BUMPINESS] * bumpiness;
    return eval;
  }
  
};