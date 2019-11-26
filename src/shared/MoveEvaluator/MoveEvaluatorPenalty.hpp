//
// Created by Dom on 2019-11-25.
//

#ifndef TETRISAI_MOVEEVALUATORPENALTY_HPP
#define TETRISAI_MOVEEVALUATORPENALTY_HPP
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/common/common.hpp"
#include <cassert>


int reachableHeight(int c, int framesPerDrop=2);

class MoveEvaluatorPenalty {
 public:
  static const int NUM_FACTORS_ = 18;
  static const int DAS_BLOCKED_LEFT = 17;

  static constexpr int getNumFactors() { return NUM_FACTORS_; }

  MoveEvaluatorPenalty(const Weighting &w): me_{MoveEvaluator(), w}, w_{w} {
    assert(w.size() == NUM_FACTORS_);
  }


  double evaluate(const BitBoard &b, const BitPieceInfo &p) const {
    int colHeights[NUM_COLUMNS];
    double eval = me_.evaluate(b, p);
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
  }

 private:
  const MoveEvaluatorAdapter me_;
  const Weighting &w_;
};



#endif //TETRISAI_MOVEEVALUATORPENALTY_HPP
