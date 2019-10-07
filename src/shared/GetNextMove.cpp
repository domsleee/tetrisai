#include "src/shared/GetNextMove.h"
#include "src/shared/MoveFinder.h"
#include "src/shared/MoveEvaluator.hpp"

Move GetNextMove::getNextMove(const BitBoard& board, BlockType blockType) {
  MoveFinder f;
  auto allMoves = f.findAllMoves(board, blockType);
  auto bestPiece = allMoves[0];
  double bestScore = 0;
  for (const auto& piece: allMoves) {
    MoveEvaluator me;
    double score = me.evaluate(board, piece);
    if (score > bestScore) {
      bestPiece = piece;
    }
  }
  return bestPiece.getPosition();
}
