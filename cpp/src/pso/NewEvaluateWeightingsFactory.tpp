#pragma once
#include "src/pso/NewEvaluateWeightings.tpp"
#include "src/pso/RunPieceSet.tpp"
#include "src/shared/NewGetNextMove.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MeMfPairProvider.h"


template<typename MyMoveFinder>
struct NewEvaluateWeightingsFactory {
  static auto getInstance(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider) {
    auto getNextMove = NewGetNextMove(meMfPairProvider);
    return NewEvaluateWeightings(RunPieceSet(getNextMove));
  }
};
