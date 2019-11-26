#pragma once
#include "src/shared/NewGetNextMove.tpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorAdapter.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/demo/PlayDemo.tpp"
#include "src/demo/PlayBestDemo.tpp"
#include "src/pso/RunPieceSet.tpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"

// this is a weird factory thing

template<typename MyMoveEvaluator, typename MyMoveFinder>
class PlayBestDemoContainer {
  std::unique_ptr<MyMoveEvaluator> me_;
  std::unique_ptr<MyMoveFinder> mf_;

  using MyGetNextMove = NewGetNextMove<MyMoveFinder, MyMoveEvaluator>;
  using MyPlayDemo = PlayDemo<MyGetNextMove>;
  using MyRunPieceSet = RunPieceSet<MyGetNextMove>;
  using MyPlayBestDemo = PlayBestDemo<MyPlayDemo, MyRunPieceSet>;

  std::unique_ptr<MyGetNextMove> getNextMove_;
  std::unique_ptr<MyPlayDemo> playDemo_;
  std::unique_ptr<MyRunPieceSet> runPieceSet_;

 public:
  PlayBestDemoContainer(const MyMoveEvaluator &me, const MyMoveFinder &mf) :
    me_(std::make_unique<MyMoveEvaluator>(me)),
    mf_(std::make_unique<MyMoveFinder>(mf))
  {}

  MyPlayBestDemo getInstance() {
    getNextMove_ = std::make_unique<MyGetNextMove>(*me_, *mf_); 
    playDemo_ = std::make_unique<MyPlayDemo>(*getNextMove_);
    runPieceSet_ = std::make_unique<MyRunPieceSet>(*getNextMove_);
    return MyPlayBestDemo(*playDemo_, *runPieceSet_);
  }
};

