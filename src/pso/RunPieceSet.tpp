#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>

const int MAX_LINES = 230;

template <typename MyGetNextMove>
class RunPieceSet {
 public:
  RunPieceSet(const MyGetNextMove &getNextMoveHandler): getNextMoveHandler_(getNextMoveHandler) {}
  int runGame(const std::vector<BlockType> &pieceSet) const;

 private:
  bool hasNoMoves(const BitBoard &b, BlockType blockType) const;
  const MyGetNextMove &getNextMoveHandler_;
};


template <typename MyGetNextMove>
int RunPieceSet<MyGetNextMove>::runGame(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm;
  BitBoard b;
  int moves = 0;
  for (auto blockType: pieceSet) {
    if (b.hasNoMoves(blockType)) {
      break;
    };
    auto move = getNextMoveHandler_.getNextMove(b, blockType);
    auto pieceInfo = b.getPiece(blockType, move);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= MAX_LINES) break;
  }
  dprintf("moves: %d, score: %d, lineClears: %d\n", moves, sm.getScore(), sm.getTotalLines());
  return sm.getScore();
};

