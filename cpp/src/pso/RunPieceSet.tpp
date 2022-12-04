#pragma once
#include "src/shared/ScoreManager.hpp"
#include "src/common/common.hpp"
#include <memory>
#include <map>
#include <functional>
#include <type_traits>
#include <typeinfo>

const int MAX_LINES = 230;

template<typename MyGetNextMove>
struct HandlerBranching {
  static auto getNextMove(MyGetNextMove &getNextmoveHandler, const BitBoard &b, BlockType blockType, ScoreManager sm) {
    return getNextmoveHandler.getNextMovePredict(b, blockType, sm);
  }
  static auto getNextMove(MyGetNextMove &getNextmoveHandler, const BitBoard &b, BlockType blockType1, BlockType blockType2, ScoreManager sm) {
    return getNextmoveHandler.getNextMovePredict(b, blockType1, blockType2, sm);
  }
};

template<typename MyGetNextMove>
struct HandlerNoBranching {
  static auto getNextMove(MyGetNextMove &getNextmoveHandler, const BitBoard &b, BlockType blockType, ScoreManager sm) {
    return getNextmoveHandler.getNextMove(b, blockType, sm);
  }
  static auto getNextMove(MyGetNextMove &getNextmoveHandler, const BitBoard &b, BlockType blockType1, BlockType blockType2, ScoreManager sm) {
    return getNextmoveHandler.getNextMove(b, blockType1, blockType2, sm);
  }
};



template <typename MyGetNextMove, bool useBranching=false,
          typename GetNextMoveHandler=typename std::conditional<
            useBranching, 
            HandlerBranching<MyGetNextMove>,
            HandlerNoBranching<MyGetNextMove>
          >::type
         >
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
  void setStartingLevel(int startingLevel) {
    startingLevel_ = startingLevel;
  }

  MyGetNextMove getNextMoveHandler_;

 private:
  int numLines_ = MAX_LINES;
  int startingLevel_ = 18;
  int startingLines_ = 0;
};

template <typename MyGetNextMove, bool useBranching, typename GetNextMoveHandler>
ScoreManager RunPieceSet<MyGetNextMove, useBranching, GetNextMoveHandler>::runGame(const std::vector<BlockType> &pieceSet) const {
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


template <typename MyGetNextMove, bool useBranching, typename GetNextMoveHandler>
ScoreManager RunPieceSet<MyGetNextMove, useBranching, GetNextMoveHandler>::runGameWithLookahead(const std::vector<BlockType> &pieceSet) const {
  ScoreManager sm{startingLevel_};
  sm.setLines(startingLines_);
  BitBoard b;
  auto getNextMoveHandler = getNextMoveHandler_;
  
  int moves = 0;
  for (auto i = 0; i < static_cast<int>(pieceSet.size()-1); ++i) {
    auto blockType1 = pieceSet[i];
    auto blockType2 = pieceSet[i+1];
    if (b.hasNoMoves(blockType1)) {
      break;
    };
    auto pieceInfo = GetNextMoveHandler::getNextMove(getNextMoveHandler, b, blockType1, blockType2, sm);
    int lineClears = b.applyPieceInfo(pieceInfo);
    sm.addLineClears(lineClears);
    moves++;
    if (sm.getTotalLines() >= numLines_) break;
  }
  return sm;
};
