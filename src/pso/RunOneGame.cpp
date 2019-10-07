#include "src/common/Board.hpp"
#include "src/common/Board.hpp"
#include "src/shared/GetNextMove.h"
#include "src/pso/RunOneGame.h"
#include "src/common/PositionPieceGetter.hpp"
#include <memory>


struct ScoreManager {
  int level_ = 19;
  int score_ = 0;
  int totalLines_ = 0;
  void addLineClears(int lineClears) {
    switch(lineClears) {
      case 0: break;
      case 1: { score_ += 40 * (level_+1); } break;
      case 2: { score_ += 100 * (level_+1); } break;
      case 3: { score_ += 300 * (level_+1); } break;
      case 4: { score_ += 1200 * (level_+1); } break;
    }
    totalLines_ += lineClears;
    if (totalLines_ >= 140) {
      level_ = 19 + (totalLines_ - 130)/10;
    }
  }
  int getScore() {
    return score_;
  }
};

int RunOneGame::runGame(const std::vector<BlockType> &pieceSet, const Weighting &weighting) {
  ScoreManager sm;
  BitBoard b;
  int moves = 0;
  for (auto blockType: pieceSet) {
    if (hasNoMoves(b, blockType)) {
      break;
    };
    auto move = getNextMoveHandler_->getNextMove(b, blockType);
    auto pieceInfo = b.getPiece(blockType, move);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
  }
  //printf("moves: %d\n", moves);
  return sm.getScore();
};
 

bool RunOneGame::hasNoMoves(const BitBoard &b, BlockType blockType) const {
  return !b.vacant(b.getPiece(blockType));
}

