#pragma once
#include <vector>
#include "src/common/Weighting.hpp"
#include "src/shared/Config.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/shared/MeMfPairProvider.h"
#include "src/pso/NewEvaluateWeightingsFactory.tpp"
#include "src/pso/NewEvaluateWeightings.tpp"
#include "src/pso/SimpleApi.tpp"

#define SIMPLE_API_DEFAULT_MF MoveFinderRewrite

template<typename MyMoveFinder=SIMPLE_API_DEFAULT_MF>
MeMfPairProvider<MyMoveFinder> getMeMfPairProvider(const MoveEvaluatorGroup &me1, const MoveEvaluatorGroup &me2, int transitionLines=90) {
  auto mf1 = MyMoveFinder();
  auto mf2 = MyMoveFinder();
  auto mf3 = MyMoveFinder();

  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);
  
  MeMfPairProvider<MyMoveFinder> meMfPairProvider(
    {
      {transitionLines, std::make_shared<MoveEvaluatorGroup>(me1)},
      {1000, std::make_shared<MoveEvaluatorGroup>(me2)}
    },
    {
      {130, std::make_shared<MyMoveFinder>(mf1)},
      {230, std::make_shared<MyMoveFinder>(mf2)},
      {1000, std::make_shared<MyMoveFinder>(mf3)}
    }
  );
  return meMfPairProvider;
}

template<typename MyMoveFinder=SIMPLE_API_DEFAULT_MF>
auto getEvaluateWeightings(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider, const Config &config, int transitionLines) {
  auto ew = NewEvaluateWeightingsFactory<MyMoveFinder>::getInstance(meMfPairProvider);
  config.applyConfig(ew);
  return ew;
}

template<typename MyMoveFinder=SIMPLE_API_DEFAULT_MF>
auto getEvaluateWeightings(const MoveEvaluatorGroup &me1, const MoveEvaluatorGroup &me2, const Config &config, int transitionLines) {
  MeMfPairProvider<MyMoveFinder> meMfPairProvider = getMeMfPairProvider<MyMoveFinder>(me1, me2, 90);
  return getEvaluateWeightings(meMfPairProvider, config, transitionLines);
}

template<typename MyMoveFinder=SIMPLE_API_DEFAULT_MF>
auto getEvaluateWeightings(const MoveEvaluatorGroup &me1, const Config &config) {
  return getEvaluateWeightings<MyMoveFinder>(me1, me1, config, 90);
}
