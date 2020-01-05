#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/board/BoardPrinter.tpp"

template<typename MyMoveFinder, typename MyMoveEvaluator>
class NewGetNextMove {
 public:
  NewGetNextMove(const MyMoveEvaluator &me, MyMoveFinder &mf): me_(std::make_unique<MyMoveEvaluator>(me)), mf_(std::make_unique<MyMoveFinder>(mf)) {}
  Move getNextMove(const BitBoard& board, BlockType blockType) const;

  void setMoveEvaluator(MyMoveEvaluator &me) {
    me_ = std::make_unique<MyMoveEvaluator>(me);
  }
  void setMoveFinder(MyMoveFinder &mf) {
    mf_ = std::make_unique<MyMoveFinder>(mf);
  }
  const MyMoveFinder& getMoveFinder() { return *mf_; }
 private:
  //const MyMoveEvaluator &me_;
  //const MyMoveFinder &mf_;

  std::unique_ptr<MyMoveEvaluator> me_;
  std::unique_ptr<MyMoveFinder> mf_;

};


template<typename MyMoveFinder, typename MyMoveEvaluator>
Move NewGetNextMove<MyMoveFinder, MyMoveEvaluator>::getNextMove(const BitBoard &board, const BlockType blockType) const {
  auto allMoves = mf_->findAllMoves(board, blockType);
  assert(allMoves.size() > 0);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  for (const auto& piece: allMoves) {
    double score = me_->evaluate(board, piece);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
  }
  //dprintf("bestScore: %.8f\n", bestScore);
  return bestPiece.getPosition();
}
