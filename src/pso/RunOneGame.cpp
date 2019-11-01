#include "src/shared/GetNextMove.h"
#include "src/pso/RunOneGame.h"
#include "src/common/PositionPieceGetter.hpp"
#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>

int RunOneGame::runGame(const std::vector<BlockType> &pieceSet, const Weighting &weighting) {
  ScoreManager sm;
  BitBoard b;
  int moves = 0;
  for (auto blockType: pieceSet) {
    if (hasNoMoves(b, blockType)) {
      break;
    };
    auto move = getNextMoveHandler_->getNextMove(b, blockType, weighting, sm);
    auto pieceInfo = b.getPiece(blockType, move);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= 230) break;
  }
  dprintf("moves: %d, score: %d, lineClears: %d\n", moves, sm.getScore(), sm.getTotalLines());
  return sm.getScore();
};
 

bool RunOneGame::hasNoMoves(const BitBoard &b, BlockType blockType) const {
  return !b.vacant(b.getPiece(blockType));
}

