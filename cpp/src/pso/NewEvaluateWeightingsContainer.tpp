#pragma once
#include "src/pso/NewEvaluateWeightings.tpp"
#include "src/pso/RunPieceSet.tpp"
#include "src/shared/NewGetNextMove.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"

// this is a weird factory thing

template<typename MyMoveFinder>
class NewEvaluateWeightingsContainer {
  std::unique_ptr<MoveEvaluatorGroup> me_;
  std::unique_ptr<MyMoveFinder> mf_;

  using MyGetNextMove = NewGetNextMove<MyMoveFinder>;
  using MyRunPieceSet = RunPieceSet<MyGetNextMove>;
  using MyNewEvaluateWeightings = NewEvaluateWeightings<MyRunPieceSet>;

  std::unique_ptr<MyGetNextMove> getNextMove_;

 public:
  std::unique_ptr<MyRunPieceSet> runPieceSet_;
  NewEvaluateWeightingsContainer(const MoveEvaluatorGroup &me, const MyMoveFinder &mf) :
    me_(std::make_unique<MoveEvaluatorGroup>(me)),
    mf_(std::make_unique<MyMoveFinder>(mf))
  {}
  

  MyNewEvaluateWeightings getInstance() {
    getNextMove_ = std::make_unique<MyGetNextMove>(*me_, *mf_);
    return MyNewEvaluateWeightings(MyRunPieceSet(*getNextMove_));
  }
};
