#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/common/Weighting.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator.hpp"
#include "src/board/BoardPrinter.tpp"




// debug
#include "src/shared/MoveFinder/MoveFinder.h"



template<typename MyMoveFinder, typename MyMoveEvaluator>
class NewGetNextMove {
 public:
  NewGetNextMove(const MyMoveEvaluator &me, MyMoveFinder &mf): me_(me), mf_(mf) {}
  Move getNextMove(const BitBoard& board, BlockType blockType) const;
 private:
  const MyMoveEvaluator &me_;
  const MyMoveFinder &mf_;
};


template<typename MyMoveFinder, typename MyMoveEvaluator>
Move NewGetNextMove<MyMoveFinder, MyMoveEvaluator>::getNextMove(const BitBoard &board, const BlockType blockType) const {
  MoveFinder mf;
  auto allMoves = mf.findAllMoves(board, blockType);
  auto secMoves = mf_.findAllMoves(board, blockType);

  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  for (const auto& piece: allMoves) {
    double score = me_.evaluate(board, piece);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
  }
  //dprintf("bestScore: %.8f\n", bestScore);
  return bestPiece.getPosition();
}
