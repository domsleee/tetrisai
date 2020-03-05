#pragma once
#include "src/board/bitboard/BitBoard.h"
#include <memory>
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"

class IEvaluator {
 public:
  virtual ~IEvaluator() {};
  virtual double evaluateMine(const BitBoard &b, const BitPieceInfo &p, const EvaluatorInfo &evaluatorInfo) const = 0;
};