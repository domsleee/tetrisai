#pragma once
#include "src/shared/NewGetNextMove.tpp"
#include "src/board/BoardPrinter.tpp"
#include "src/shared/ScoreManager.hpp"
#include <iostream>


// similar to RunOneGame

template<typename MyGetNextMove, typename MyBoard=BitBoard>
class PlayDemo {
 public:
  PlayDemo(const MyGetNextMove& getNextMove): getNextMoveHandler_(getNextMove) {}
  void playDemo(const std::vector<BlockType> &pieceSet) const;
 private:

  bool hasNoMoves(const BitBoard &b, BlockType blockType) const {
    return !b.vacant(b.getPiece(blockType));
  }

  const MyGetNextMove &getNextMoveHandler_;
};


template <typename MyGetNextMove, typename MyBoard>
void PlayDemo<MyGetNextMove, MyBoard>::playDemo(const std::vector<BlockType> &pieceSet) const {
  MyBoard b;
  ScoreManager sm;
  int moveNo = 0;
  for (auto blockType: pieceSet) {
    if (hasNoMoves(b, blockType)) {
      break;
    };
    auto move = getNextMoveHandler_.getNextMove(b, blockType);
    auto pieceInfo = b.getPiece(blockType, move);
    printBoardWithPiece(b, pieceInfo);
    sm.addLineClears(b.applyPieceInfo(pieceInfo));
    moveNo++;
    //printf("moves: %d, score: %d, lineClears: %d\n", moveNo, sm.getScore(), sm.getTotalLines());
    if (sm.getTotalLines() >= 230) break;
  }
  printf("score: %d\n", sm.getScore());
};


