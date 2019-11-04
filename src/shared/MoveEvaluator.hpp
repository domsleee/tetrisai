#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/Weighting.hpp"
#include <algorithm>
#include <numeric>

class VacancyChecker {
 public:
  VacancyChecker(const BitBoard &b): b_(b) {};
  bool is_vacant(const Coord &coord) {
    int ind = coord.r * NUM_COLUMNS + coord.c;
    if (vacant_[ind] == UNDEF_) vacant_[ind] = b_.vacant(coord);
    return vacant_[ind];
  }
 private:
  const char UNDEF_ = 'c';
  const BitBoard &b_;
  std::vector<char> vacant_ = decltype(vacant_)(NUM_ROWS * NUM_COLUMNS, UNDEF_);
};

class MoveEvaluator {
  const int INF = 30; // clearly not infinitely lmao

 public:
  static const int TOTAL_LINES_CLEARED = 0;
  static const int TOTAL_LOCK_HEIGHT   = 1;
  static const int TOTAL_WELL_CELLS    = 2;
  static const int TOTAL_DEEP_WELLS    = 3;
  static const int TOTAL_COLUMN_HOLES  = 4;
  static const int TOTAL_WEIGHTED_COLUMN_HOLES = 5;
  static const int TOTAL_COLUMN_HOLE_DEPTH = 6;
  static const int MIN_COLUMN_HOLE_DEPTH = 7;
  static const int MAX_COLUMN_HOLE_DEPTH = 8;
  static const int TOTAL_COLUMN_TRANSITIONS = 9;
  static const int TOTAL_ROW_TRANSITIONS = 10;
  static const int TOTAL_COLUMN_HEIGHTS = 11;
  static const int PILE_HEIGHT = 12;
  static const int COLUMN_HEIGHT_SPREAD = 13;
  static const int TOTAL_SOLID_CELLS = 14;
  static const int TOTAL_WEIGHTED_SOLID_CELLS = 15;
  static const int COLUMN_HEIGHT_VARIANCE = 16;

  double evaluate(const BitBoard& b, const BitPieceInfo& p, const Weighting &w, const ScoreManager &m) {
    auto nxBoard = b;
    auto deltaLines = nxBoard.applyPieceInfo(p);
    return my_evaluate(nxBoard, p, w, m, deltaLines);
  }
  
  double my_evaluate(const BitBoard &b, const BitPieceInfo& p, const Weighting &w, const ScoreManager &m, int deltaLines) {
    if (deltaLines == 4) {
      return -1e9;
    }
    double eval = 0;
    VacancyChecker vac(b);
    eval += w[TOTAL_LINES_CLEARED] * deltaLines;
    eval += w[TOTAL_LOCK_HEIGHT] * (NUM_ROWS - p.getPosition().maxR - 1);

    int lockHeights[NUM_COLUMNS];
    int minR = NUM_ROWS;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      lockHeights[c] = NUM_ROWS;
      for (int r = 0; r < NUM_ROWS; r++) {
        if (!vac.is_vacant({r, c})) {
          lockHeights[c] = r;
          minR = std::min(minR, r);
          break;
        }
      }
    }
    //int lockHeight = std::accumulate(lockHeights, lockHeights + NUM_COLUMNS, 0);
    //eval += w[TOTAL_LOCK_HEIGHT] * lockHeight;

    int totalWellCells = 0, totalDeepWells = 0, totalWeightedColumnHoles = 0, totalColumnHeights = 0;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      int l = c == 0 ? INF : lockHeights[c]-lockHeights[c-1];
      int r = c == NUM_COLUMNS-1 ? INF : lockHeights[c]-lockHeights[c+1];
      int wellCells = std::min(l, r);
      if (wellCells > 0) totalWellCells += wellCells;
      if (wellCells >= 3) totalDeepWells++;
      totalColumnHeights += NUM_ROWS - lockHeights[c];
    }
    eval += w[TOTAL_COLUMN_HEIGHTS] * totalColumnHeights;
    eval += w[TOTAL_WELL_CELLS] * totalWellCells;
    eval += w[TOTAL_DEEP_WELLS] * totalDeepWells;
  
    int totalColumnHoles = 0, totalColumnHolesDepth = 0, minColumnHoleDepth = NUM_ROWS, maxColumnHoleDepth = 0;
    int totalColumnTransitions = 0, totalRowTransitions = 0;
    int totalSolidCells = 0, totalWeightedSolidCells = 0;
    for (int r = 0; r < NUM_ROWS; ++r) {
      for (int c = 0; c < NUM_COLUMNS; ++c) {
        bool takenUp = r == 0 || !vac.is_vacant({r-1, c});
        if (r > 0 && takenUp && vac.is_vacant({r,c})) {
          totalColumnHoles++;
          totalWeightedColumnHoles += r+1;
          int holeDepth = r - lockHeights[c];
          totalColumnHolesDepth += holeDepth;
          minColumnHoleDepth = std::min(minColumnHoleDepth, holeDepth);
          maxColumnHoleDepth = std::max(maxColumnHoleDepth, holeDepth);
        }
        if (r > 0 && r != lockHeights[c]) {
          totalColumnTransitions += vac.is_vacant({r,c}) != vac.is_vacant({r-1,c});
        }
        if (!vac.is_vacant({r, c})) {
          totalSolidCells++;
          totalWeightedSolidCells += (NUM_ROWS-r);
        }
      }
    }
    totalRowTransitions = calculateRowTransitions(vac, minR);

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
        columnHeightVariance += abs(lockHeights[c]-lockHeights[c+1]);
      }
      mColumnHeight = std::min(mColumnHeight, lockHeights[c]);
      MColumnHeight = std::max(MColumnHeight, lockHeights[c]);
    }
    int columnHeightSpread = MColumnHeight-mColumnHeight;
    int pileHeight = (NUM_ROWS - mColumnHeight);
    eval += w[COLUMN_HEIGHT_VARIANCE] * columnHeightVariance;
    eval += w[PILE_HEIGHT] * pileHeight;
    eval += w[COLUMN_HEIGHT_SPREAD] * columnHeightSpread;

    return eval;
  }

 private:
  int calculateRowTransitions(VacancyChecker &vac, int minR) {
    int res = 0;
    for (int r = minR; r < NUM_ROWS; r++) {
      if (vac.is_vacant(Coord{r, 0})) res++;
      if (vac.is_vacant({r, NUM_COLUMNS-1})) res++;
      for (int c = 0; c < NUM_COLUMNS-1; c++) {
        if (vac.is_vacant({r,c}) != vac.is_vacant({r, c+1})) res++;
      }
    }    return res;

  }
};

