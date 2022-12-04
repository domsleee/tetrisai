#pragma once
#include "src/pso/NewEvaluateWeightings.tpp"
#include "src/pso/RunPieceSet.tpp"
#include "src/shared/NewGetNextMove.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MeMfPairProvider.h"


template<typename MyMoveFinder, bool useBranching=false>
struct NewEvaluateWeightingsFactory {
  static auto getInstance(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider) {
    auto getNextMove = NewGetNextMove(meMfPairProvider);
    return NewEvaluateWeightings(RunPieceSet<decltype(getNextMove), useBranching>(getNextMove));
  }

  static auto getInstance(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider, std::shared_ptr<IPieceSetGetter> ps) {
    auto getNextMove = NewGetNextMove(meMfPairProvider);
    return NewEvaluateWeightings(RunPieceSet<decltype(getNextMove), useBranching>(getNextMove), ps);
  }
};
