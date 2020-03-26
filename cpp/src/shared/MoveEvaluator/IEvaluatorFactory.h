#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluator.hpp"


class IEvaluatorFactory {
 public:
  virtual int getNumFactors() = 0;
  virtual ~IEvaluatorFactory() {}
  virtual std::unique_ptr<IEvaluator> createNew(Weighting &w) = 0;
};

template <typename T>
class EvaluatorFactory: public IEvaluatorFactory {
 public:
  std::unique_ptr<IEvaluator> createNew(Weighting &w) override {
    return std::make_unique<T>(w);
  }
  int getNumFactors() override {
    return T::NUM_FACTORS;
  }
};

// MoveEvaluatorAugmenter(MoveEvaluatorGroup, weights)

