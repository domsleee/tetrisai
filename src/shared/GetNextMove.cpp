#include "src/shared/GetNextMove.h"
#include "src/shared/MoveFinder.h"
#include "src/shared/MoveEvaluator.hpp"
#include "src/shared/AllMoveFinder.tpp"
#include "src/common/common.hpp"

Move GetNextMove::getNextMove(const BitBoard& board, BlockType blockType, const Weighting &w, const ScoreManager &sm) {
  AllMoveFinder f;
  auto allMoves = f.findAllMoves(board, blockType);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  for (const auto& piece: allMoves) {
    MoveEvaluator me;
    double score = me.evaluate(board, piece, w, sm);
    if (score < bestScore) {
      bestPiece = piece;
      bestScore = score;
    }
  }
  //dprintf("bestScore: %.8f\n", bestScore);
  return bestPiece.getPosition();
}
