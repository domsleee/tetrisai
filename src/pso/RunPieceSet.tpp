#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>

const int MAX_LINES = 230;

template <typename MyGetNextMove, typename MyGetNextMove2=MyGetNextMove>
class RunPieceSet {
 public:
  RunPieceSet(const MyGetNextMove &getNextMoveHandler): RunPieceSet(getNextMoveHandler, getNextMoveHandler) {}
  RunPieceSet(const MyGetNextMove &getNextMoveHandler, const MyGetNextMove2 &getNextMoveHandler2) :
      getNextMoveHandler_(getNextMoveHandler),
      getNextMoveHandler2_(getNextMoveHandler2) {}

  int runGame(const std::vector<BlockType> &pieceSet) const;

 private:
  const MyGetNextMove &getNextMoveHandler_;
  const MyGetNextMove2 &getNextMoveHandler2_;
};

// this is so bad
template <typename MyGetNextMove>
int runGameSingle(BitBoard &b, ScoreManager &sm, const std::vector<BlockType> &pieceSet, int ind, const MyGetNextMove& getNextMoveHandler, int maxLines);

template <typename MyGetNextMove, typename MyGetNextMove2>
int RunPieceSet<MyGetNextMove, MyGetNextMove2>::runGame(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm;
  BitBoard b;

  int ind = runGameSingle(b, sm, pieceSet, 0, getNextMoveHandler_, MAX_LINES);
  if (sm.getTotalLines() < MAX_LINES) {
    runGameSingle(b, sm, pieceSet, ind+1, getNextMoveHandler2_, MAX_LINES);
  }
  return sm.getScore();
};



template <typename MyGetNextMove>
int runGameSingle(BitBoard &b, ScoreManager &sm, const std::vector<BlockType> &pieceSet, int ind, const MyGetNextMove& getNextMoveHandler, int maxLines) {
  int moves = 0, i;
  for (i = ind; i < pieceSet.size(); ++i) {
    auto blockType = pieceSet[i];
    if (b.hasNoMoves(blockType)) {
      break;
    };
    auto move = getNextMoveHandler.getNextMove(b, blockType);
    auto pieceInfo = b.getPiece(blockType, move);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= maxLines) break;
  }
  return i;
}