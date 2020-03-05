#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>
#include <map>
#include <functional>

const int MAX_LINES = 230;

template <typename MyGetNextMove>
class RunPieceSet {
 public:
  using MutatorFnT = std::function<void(MyGetNextMove&)>;
  RunPieceSet(const MyGetNextMove &getNextMoveHandler): getNextMoveHandler_(getNextMoveHandler) {}

  void addTransition(int lineClearCt, MutatorFnT mutator) {
    mutators_.insert({lineClearCt, mutator});
  }

  void addMeTransition(int lineClearCt, const auto &me) {
    mutators_.insert({lineClearCt, [=](auto& getNextMoveHandler) -> void {
      getNextMoveHandler.setMoveEvaluator(me);
    }});;
  }

  void addMfTransition(int lineClearCt, const auto &mf) {
    mutators_.insert({lineClearCt, [=](auto& getNextMoveHandler) -> void {
      getNextMoveHandler.setMoveFinder(mf);
    }});;
  }

  ScoreManager runGame(const std::vector<BlockType> &pieceSet) const;
  void setNumLines(int numLines) {
    numLines_ = numLines;
  }
  void setStartingLines(int startingLines) {
    startingLines_ = startingLines;
  }
  void setMaxDropRem(int maxDropRem) {
    getNextMoveHandler_.setMaxDropRem(maxDropRem);
  }
  void setStartingLevel(int startingLevel) {
    startingLevel_ = startingLevel;
  }
  MyGetNextMove getNextMoveHandler_;

 private:
  int numLines_ = MAX_LINES;
  int startingLevel_ = 18;
  int startingLines_ = 0;
  std::multimap<int, MutatorFnT> mutators_;
};

template <typename MyGetNextMove>
ScoreManager RunPieceSet<MyGetNextMove>::runGame(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm{startingLevel_};
  sm.setLines(startingLines_);
  BitBoard b;
  auto getNextMoveHandler = getNextMoveHandler_;

  int transitionLines = numLines_;
  auto it = mutators_.begin();
  if (it != mutators_.end()) {
    transitionLines = it->first;
  }

  while (it != mutators_.end() && sm.getTotalLines() >= transitionLines) {
    it->second(getNextMoveHandler);
    ++it;
    transitionLines = it == mutators_.end() ? numLines_ : it->first;
  }
  
  int moves = 0;
  for (auto blockType: pieceSet) {
    if (b.hasNoMoves(blockType)) {
      break;
    };
    auto move = getNextMoveHandler.getNextMove(b, blockType, sm.getLevel());
    auto pieceInfo = b.getPiece(blockType, move);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    while (it != mutators_.end() && sm.getTotalLines() >= transitionLines) {
      it->second(getNextMoveHandler);
      ++it;
      transitionLines = it == mutators_.end() ? numLines_ : it->first;
    }
    if (sm.getTotalLines() >= numLines_) break;
  }
  //printf("yes: %d\n", sm.getScore());
  return sm;
};
