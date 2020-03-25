#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/Move.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/VacancyChecker.hpp"
#include <algorithm>
#include <numeric>

#include "src/shared/MoveEvaluator/IEvaluator.h"


#define LOCK_HEIGHT(c) (NUM_ROWS - colHeights[c])

class MoveEvaluator: public IEvaluator {
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

  static const int NUM_FACTORS = 17;

  MoveEvaluator(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const override {
    return myEvaluate(evaluatorInfo.getAppliedBoard(), p, w_, evaluatorInfo.getArrColHeights(), evaluatorInfo.getLineClears(), evaluatorInfo.getAppliedBoardVac());
  }
  
  double myEvaluate(const BitBoard &b, const BitPieceInfo& p, const Weighting &w, const std::array<int, NUM_COLUMNS> &colHeights, int deltaLines, const VacancyChecker &vac) const {
    double offset = (deltaLines == 4) ? -1e9 : 0;
    double eval = offset;
    if (true || deltaLines != 4) eval += w[TOTAL_LINES_CLEARED] * deltaLines;
    eval += w[TOTAL_LOCK_HEIGHT] * (NUM_ROWS - p.getPosition().maxR - 1);

    auto [it1, it2] = std::minmax_element(colHeights.begin(), colHeights.end());
    int mColHeight = *it1;
    int MColHeight = *it2;
    int minR = NUM_ROWS - MColHeight;
    int totalWellCells = 0, totalDeepWells = 0, totalWeightedColumnHoles = 0, totalColumnHeights = 0;
    for (int c = 0; c < NUM_COLUMNS; c++) {
      int l = c == 0 ? INF : colHeights[c-1] - colHeights[c];
      int r = c == NUM_COLUMNS-1 ? INF : colHeights[c+1] - colHeights[c];
      int wellCells = std::min(l, r);
      if (wellCells > 0) totalWellCells += wellCells;
      if (wellCells >= 3) totalDeepWells++;
      totalColumnHeights += colHeights[c];
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
          // r - (NUM_COLUMNS - colHeights[c]) = colHeights[c] + r - NUM_COLUMNS
          int holeDepth = r - LOCK_HEIGHT(c);
          totalColumnHolesDepth += holeDepth;
          minColumnHoleDepth = std::min(minColumnHoleDepth, holeDepth);
          maxColumnHoleDepth = std::max(maxColumnHoleDepth, holeDepth);
        }
        if (r > LOCK_HEIGHT(c)) { // r != LOCK_HEIGHT(c)
          totalColumnTransitions += vac.is_vacant({r,c}) != vac.is_vacant({r-1,c});
        }
        if (!vac.is_vacant({r, c})) {
          ++totalSolidCells;
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
    for (int c = 0; c < NUM_COLUMNS-1; c++) {
      columnHeightVariance += abs(colHeights[c]-colHeights[c+1]);
    }

    int columnHeightSpread = MColHeight-mColHeight;
    int pileHeight = MColHeight;
    eval += w[COLUMN_HEIGHT_VARIANCE] * columnHeightVariance;
    eval += w[PILE_HEIGHT] * pileHeight;
    eval += w[COLUMN_HEIGHT_SPREAD] * columnHeightSpread;

    //for (auto v: w) std::cout << v << ' '; std::cout << '\n';

    return eval;
  }

 private:
  int calculateRowTransitions(const VacancyChecker &vac, int minR) const {
    int res = 0;
    for (int r = minR; r < NUM_ROWS; r++) {
      if (vac.is_vacant({r, 0})) res++;
      if (vac.is_vacant({r, NUM_COLUMNS-1})) res++;
      for (int c = 0; c < NUM_COLUMNS-1; c++) {
        if (vac.is_vacant({r,c}) != vac.is_vacant({r, c+1})) res++;
      }
    }
    return res;
  }

  const Weighting w_;
};

