#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include <algorithm>
#include <numeric>

using Weighting = std::vector<double>; //todo: move somewhere else?

class MoveEvaluator {
  const int INF = 30; // clearly not infinitely lmao
  const int TOTAL_LINES_CLEARED = 0;
  const int TOTAL_LOCK_HEIGHT   = 1;
  const int TOTAL_WELL_CELLS    = 2;
  const int TOTAL_DEEP_WELLS    = 3;
  const int TOTAL_COLUMN_HOLES  = 4;
  const int TOTAL_WEIGHTED_COLUMN_HOLES = 5;
  const int TOTAL_COLUMN_HOLE_DEPTH = 6;
  const int MIN_COLUMN_HOLE_DEPTH = 7;
  const int MAX_COLUMN_HOLE_DEPTH = 8;
  const int TOTAL_COLUMN_TRANSITIONS = 9;
  const int TOTAL_ROW_TRANSITIONS = 10;
  const int TOTAL_COLUMN_HEIGHTS = 11;
  const int PILE_HEIGHT = 12;
  const int COLUMN_HEIGHT_SPREAD = 13;
  const int TOTAL_SOLID_CELLS = 14;
  const int TOTAL_WEIGHTED_SOLID_CELLS = 15;
  const int COLUMN_HEIGHT_VARIANCE = 16;

 public:
  double evaluate(const BitBoard& b, const BitPieceInfo& p, const Weighting &w, const ScoreManager &m) {
    auto nxBoard = b;
    auto deltaLines = nxBoard.applyPieceInfo(p);

    // printf("==========\n");
    // for (int i = 0; i < w.size(); i++) {
    //   printf("%0.8f\n", w[i]);
    // }

    double eval = 0;
    eval += w[TOTAL_LINES_CLEARED] * (m.totalLines_ + deltaLines);


    int lockHeights[NUM_COLUMNS];
    for (int c = 0; c < NUM_COLUMNS; c++) {
      bool broken = false;
      for (int r = 0; r < NUM_ROWS; r++) {
        if (!b.vacant({r, c})) {
          lockHeights[c] = r;
          broken = true;
          break;
        }
      }
      if (broken) lockHeights[c] = NUM_ROWS;
    }
    int lockHeight = std::accumulate(lockHeights, lockHeights + NUM_COLUMNS, 0);
    //eval += w[TOTAL_LOCK_HEIGHT] * lockHeight;


    int totalWellCells = 0, totalDeepWells = 0, totalWeightedColumnHoles = 0, totalColumnHeights = 0;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      int l = c == 0 ? INF : lockHeights[c]-lockHeights[c-1];
      int r = c == NUM_COLUMNS-1 ? INF : lockHeights[c]-lockHeights[c+1];
      int wellCells = std::min(l, r);
      if (wellCells > 0) totalWellCells += wellCells;
      if (wellCells >= 3) totalDeepWells++;
      totalColumnHeights += lockHeights[c];
    }
    eval += w[TOTAL_COLUMN_HEIGHTS] * totalColumnHeights;
    eval += w[TOTAL_WELL_CELLS] * totalWellCells;
    eval += w[TOTAL_DEEP_WELLS] * totalDeepWells;
  
    int totalColumnHoles = 0, totalColumnHolesDepth = 0, minColumnHoleDepth = 20, maxColumnHoleDepth = 0;
    int totalColumnTransitions = 0, totalRowTransitions = 0;
    int totalSolidCells = 0, totalWeightedSolidCells = 0;
    for (int r = 0; r < NUM_ROWS; r++) {
      for (int c = 0; c < NUM_COLUMNS; c++) {
        bool takenLeft = c == 0 || !b.vacant({r, c-1});
        bool takenRight = c == NUM_COLUMNS-1 || !b.vacant({r, c+1});
        bool takenUp = r == 0 || !b.vacant({r-1, c});
        bool takenDown = r == NUM_ROWS-1 || !b.vacant({r+1, c});
        if (takenLeft && takenRight && takenUp && takenDown) {
          totalColumnHoles++;
          totalWeightedColumnHoles += r;
          int holeDepth = r - lockHeights[c];
          totalColumnHolesDepth += holeDepth;
          minColumnHoleDepth = std::min(minColumnHoleDepth, holeDepth);
          maxColumnHoleDepth = std::min(maxColumnHoleDepth, holeDepth);
        }
        if (takenUp && takenDown) {
          totalColumnTransitions++;
        }
        if ((takenLeft && takenRight) || (c == 0 && !b.vacant({r,c})) || (c == NUM_COLUMNS-1 && !b.vacant({r,c}))) {
          totalRowTransitions++;
        }
        if (!b.vacant({r, c})) {
          totalSolidCells++;
          totalWeightedSolidCells += (NUM_ROWS-r+1);
        }
      }
    }
    eval += w[TOTAL_WEIGHTED_COLUMN_HOLES] * totalWeightedColumnHoles;
    eval += w[TOTAL_COLUMN_HOLES] * totalColumnHoles;
    eval += w[TOTAL_COLUMN_HOLE_DEPTH] * totalColumnHolesDepth;
    eval += w[MIN_COLUMN_HOLE_DEPTH] * minColumnHoleDepth;
    eval += w[MAX_COLUMN_HOLE_DEPTH] * maxColumnHoleDepth;
    eval += w[TOTAL_ROW_TRANSITIONS] * totalRowTransitions;
    eval += w[TOTAL_COLUMN_TRANSITIONS] * totalColumnTransitions;
    eval += w[TOTAL_SOLID_CELLS] * totalSolidCells;
    eval += w[TOTAL_WEIGHTED_SOLID_CELLS] * totalWeightedSolidCells;


    int columnHeightVariance = 0;
    int mColumnHeight = lockHeights[0], MColumnHeight = lockHeights[0];
    for (int c = 0; c < NUM_COLUMNS; c++) {
      if (c < NUM_COLUMNS-1) {
        columnHeightVariance = std::max(columnHeightVariance, abs(lockHeights[c]-lockHeights[c+1]));
      }
      mColumnHeight = std::min(mColumnHeight, lockHeights[c]);
      MColumnHeight = std::max(MColumnHeight, lockHeights[c]);
    }
    int columnHeightSpread = MColumnHeight-mColumnHeight;
    int pileHeight = MColumnHeight;
    eval += w[COLUMN_HEIGHT_VARIANCE] * columnHeightVariance;
    eval += w[PILE_HEIGHT] * pileHeight;
    eval += w[COLUMN_HEIGHT_SPREAD] * columnHeightSpread;

    return eval;
  }
};

