#pragma once
#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>
#include <map>
#include <functional>

const int MAX_LINES = 230;

template <typename MyGetNextMove>
class RunPieceSet {
 public:
  RunPieceSet(const MyGetNextMove &getNextMoveHandler): getNextMoveHandler_(getNextMoveHandler) {}

  ScoreManager runGame(const std::vector<BlockType> &pieceSet) const;
  ScoreManager runGameWithLookahead(const std::vector<BlockType> &pieceSet) const;
  void setNumLines(int numLines) {
    numLines_ = numLines;
  }
  void setStartingLines(int startingLines) {
    startingLines_ = startingLines;
  }
  void setMaxDropRem(int maxDropRem) {
    // todo: delete
    //getNextMoveHandler_.setMaxDropRem(maxDropRem);
  }
  void setStartingLevel(int startingLevel) {
    startingLevel_ = startingLevel;
  }
  MyGetNextMove getNextMoveHandler_;

 private:
  int numLines_ = MAX_LINES;
  int startingLevel_ = 18;
  int startingLines_ = 0;
};

template <typename MyGetNextMove>
ScoreManager RunPieceSet<MyGetNextMove>::runGame(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm{startingLevel_};
  sm.setLines(startingLines_);
  BitBoard b;
  auto getNextMoveHandler = getNextMoveHandler_;
  
  int moves = 0;
  for (auto blockType: pieceSet) {
    if (b.hasNoMoves(blockType)) {
      break;
    };
    auto pieceInfo = getNextMoveHandler.getNextMove(b, blockType, sm);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= numLines_) break;
  }
  return sm;
};


template <typename MyGetNextMove>
ScoreManager RunPieceSet<MyGetNextMove>::runGameWithLookahead(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm{startingLevel_};
  sm.setLines(startingLines_);
  BitBoard b;
  auto getNextMoveHandler = getNextMoveHandler_;
  
  int moves = 0;
  for (auto i = 0; i < pieceSet.size()-1; ++i) {
    auto blockType1 = pieceSet[i];
    auto blockType2 = pieceSet[i+1];
    if (b.hasNoMoves(blockType1)) {
      break;
    };
    auto pieceInfo = getNextMoveHandler.getNextMove(b, blockType1, blockType2, sm);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= numLines_) break;
  }
  return sm;
};
