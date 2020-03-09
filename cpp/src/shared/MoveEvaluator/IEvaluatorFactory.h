#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/IEvaluator.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"


class IEvaluatorFactory {
 public:
  virtual std::unique_ptr<IEvaluator> createNew(const Weighting &w) = 0;
};

template <typename T>
class EvaluatorFactory: public IEvaluatorFactory {
 public:
  std::unique_ptr<IEvaluator> createNew(const Weighting &w) override {
    return std::make_unique<T>(w);
  }
};

template <>
class EvaluatorFactory<MoveEvaluatorAdapter<MoveEvaluator>>: public IEvaluatorFactory {
 public:
  std::unique_ptr<IEvaluator> createNew(const Weighting &w) override {
    return std::make_unique<MoveEvaluatorAdapter<MoveEvaluator>>(w);
  }
};


