#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/IEvaluator.h"


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
class EvaluatorFactory<MoveEvaluatorAdapter>: public IEvaluatorFactory {
 public:
  std::unique_ptr<IEvaluator> createNew(const Weighting &w) override {
    return std::make_unique<MoveEvaluatorAdapter>(MoveEvaluator(), w);
  }
};


