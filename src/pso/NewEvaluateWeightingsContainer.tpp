#pragma once
#include "src/pso/NewEvaluateWeightings.tpp"
#include "src/pso/RunPieceSet.tpp"
#include "src/shared/NewGetNextMove.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"

// this is a weird factory thing

template<typename MyMoveEvaluator, typename MyMoveFinder>
class NewEvaluateWeightingsContainer {
  std::unique_ptr<MyMoveEvaluator> me_;
  std::unique_ptr<MyMoveFinder> mf_;

  using MyGetNextMove = NewGetNextMove<MyMoveFinder, MyMoveEvaluator>;
  using MyRunPieceSet = RunPieceSet<MyGetNextMove>;
  using MyNewEvaluateWeightings = NewEvaluateWeightings<MyRunPieceSet>;

  std::unique_ptr<MyGetNextMove> getNextMove_;
  std::unique_ptr<MyRunPieceSet> runPieceSet_;

 public:
  NewEvaluateWeightingsContainer(const MyMoveEvaluator &me, const MyMoveFinder &mf) :
    me_(std::make_unique<MyMoveEvaluator>(me)),
    mf_(std::make_unique<MyMoveFinder>(mf))
  {}

  MyNewEvaluateWeightings getInstance() {
    getNextMove_ = std::make_unique<MyGetNextMove>(*me_, *mf_); 
    return MyNewEvaluateWeightings(MyRunPieceSet(*getNextMove_));
  }
};

