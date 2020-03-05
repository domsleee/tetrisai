#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/board/BoardPrinter.tpp"
#include "src/shared/di/di.h"

template<typename MyMoveFinder, typename MyMoveEvaluator>
class NewGetNextMove {
 public:
  NewGetNextMove(const MyMoveEvaluator &me, MyMoveFinder &mf): me_(std::make_unique<MyMoveEvaluator>(me)), mf_(std::make_unique<MyMoveFinder>(mf)) {}
  NewGetNextMove(const NewGetNextMove &getNextMove): me_(std::make_unique<MyMoveEvaluator>(*getNextMove.me_)), mf_(std::make_unique<MyMoveFinder>(*getNextMove.mf_)) {}
  Move getNextMove(const BitBoard& board, BlockType blockType, int level) const;

  void setMoveEvaluator(const MyMoveEvaluator &me) {
    me_ = std::make_unique<MyMoveEvaluator>(me);
  }
  void setMoveFinder(const MyMoveFinder &mf) {
    mf_ = std::make_unique<MyMoveFinder>(mf);
  }
  void setMaxDropRem(int maxDropRem) {
    mf_->setMaxDropRem(maxDropRem);
  }
  const MyMoveFinder& getMoveFinder() { return *mf_; }
 private:
  std::unique_ptr<MyMoveEvaluator> me_;
  std::unique_ptr<MyMoveFinder> mf_;

};


template<typename MyMoveFinder, typename MyMoveEvaluator>
Move NewGetNextMove<MyMoveFinder, MyMoveEvaluator>::getNextMove(const BitBoard &board, const BlockType blockType, int level) const {
  auto allMoves = mf_->findAllMoves(board, blockType);
#ifdef RECORD_MOVES
  Di::getMoveRecorder().recordMoves(board, blockType, allMoves);
#endif
  assert(allMoves.size() > 0);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  dprintf("NewGetNextMove: numMoves: %lu\n", allMoves.size());
  for (const auto& piece: allMoves) {
    double score = me_->evaluate(board, piece, level);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
    dprintf("NewGetNextMove: score: %0.2f\n", score);
  }
  dprintf("NewGetNextMove: bestScore: %.8f\n", bestScore);
  return bestPiece.getPosition();
}
