#pragma once
#include "src/shared/ScoreManager.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"

struct EvaluatorInfo {
  EvaluatorInfo(const BitBoard &b, const BitPieceInfo &p, const ScoreManager &sm);

  int getMaxDropRem() const {
    if (newSm_.getLevel() <= 18) return 3;
    else if (newSm_.getLevel() < 29) return 2;
    return 1;
  }

  const int* getMyColHeights() const {
    return colHeights_.data();
  }

  int getLevel() const {
    return newSm_.getLevel();
  }

  const BitBoard &getAppliedBoard() const {
    return newB_;
  }

  const BitBoard &getOldBoard() const {
    return oldB_;
  }

 private:
  std::array<int, NUM_COLUMNS> colHeights_;
  const BitBoard &oldB_;
  const BitPieceInfo &p_;
  const ScoreManager &oldSm_;
  ScoreManager newSm_;
  BitBoard newB_;
  int lineClears_;
};

