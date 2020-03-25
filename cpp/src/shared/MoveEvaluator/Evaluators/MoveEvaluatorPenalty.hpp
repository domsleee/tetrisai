//
// Created by Dom on 2019-11-25.
//

#ifndef TETRISAI_MOVEEVALUATORPENALTY_HPP
#define TETRISAI_MOVEEVALUATORPENALTY_HPP
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"
#include <cassert>


int reachableHeight(int c, int framesPerDrop=2);

class MoveEvaluatorPenalty {
 public:
  static const int NUM_FACTORS = 2;
  static const int DAS_BLOCKED_LEFT = 0;
  static const int DAS_BLOCKED_RIGHT = 1;

  MoveEvaluatorPenalty(const Weighting &w): w_{w} {
    assert(w.size() == NUM_FACTORS);
  }

  double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const {
    int colHeights[NUM_COLUMNS];
    double eval = 0;
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      colHeights[c] = 0;
      for (int r = 0; r < NUM_ROWS; ++r) {
        if (!b.vacant({r, c})) {
          colHeights[c] = NUM_ROWS - r;
          break;
        }
      }
    }

    // number of blocked cells?
    // starting at row index 5.
    for (int c = 5; c >= 0; --c) {
      if (colHeights[c] > reachableHeight(c)) {
        int m = *std::min_element(colHeights, colHeights+c);
        int r = colHeights[c] - m;
        eval += w_[DAS_BLOCKED_LEFT] * r;
        break;
      }
    }

    return eval;

    // number of blocked cells?
    // starting at row index 5.
    for (int c = 5; c < NUM_ROWS; ++c) {
      if (colHeights[c] > reachableHeight(c)) {
        int m = *std::min_element(colHeights+c, colHeights+NUM_ROWS);
        int r = colHeights[c] - m;
        eval += w_[DAS_BLOCKED_RIGHT] * r;
        break;
      }
    }


    return eval;
  }

 private:
  const Weighting &w_;
};



#endif //TETRISAI_MOVEEVALUATORPENALTY_HPP
