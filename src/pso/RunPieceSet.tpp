#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>
#include <map>
#include <functional>

const int MAX_LINES = 230;

template <typename MyGetNextMove>
class RunPieceSet {
 public:
  using MutatorFnT = std::function<void(RunPieceSet<MyGetNextMove>&)>;
  RunPieceSet(MyGetNextMove &getNextMoveHandler): getNextMoveHandler_(getNextMoveHandler) {}

  void addTransition(int lineClearCt, MutatorFnT mutator) {
    mutators_.insert({lineClearCt, mutator});
  }
  ScoreManager runGame(const std::vector<BlockType> &pieceSet);
  void setNumLines(int numLines) {
    numLines_ = numLines;
  }
  void setStartingLines(int startingLines) {
    startingLines_ = startingLines;
  }
  void setMaxDropRem(int maxDropRem) {
    getNextMoveHandler_.setMaxDropRem(maxDropRem);
  }
  MyGetNextMove &getNextMoveHandler_;

 private:
  int numLines_ = MAX_LINES;
  int startingLines_ = 0;
  std::multimap<int, MutatorFnT> mutators_;
};

template <typename MyGetNextMove>
ScoreManager RunPieceSet<MyGetNextMove>::runGame(const std::vector<BlockType> &pieceSet) {
  ScoreManager sm;
  sm.setLines(startingLines_);
  BitBoard b;

  int transitionLines = MAX_LINES;
  auto it = mutators_.begin();
  if (it != mutators_.end()) {
    transitionLines = it->first;
  }
  
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
    if (sm.getTotalLines() >= transitionLines) {
      if (it != mutators_.end()) {
        it->second(*this);
        ++it;
        transitionLines = it == mutators_.end() ? MAX_LINES : it->first;
      }
      if (sm.getTotalLines() >= MAX_LINES) break;
    };
  }
  //printf("yes: %d\n", sm.getScore());
  return sm;
};
