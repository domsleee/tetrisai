#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/common/PositionPieceGetter.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/BoardPrinter.tpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>

const auto EDGE_HOLDING_TO_HOLDING = 0;
const auto EDGE_RELEASED_TO_LAST_HIT_UNUSED = 1;
const auto EDGE_RELEASED_TO_LAST_HIT_USED = 2;
const auto EDGE_RELEASED_NO_LAST_HIT = 3;

struct DoWork {
  std::unordered_map<int, bool> holdingSeen_[2];
  std::vector<bool> releasedSeen_ = std::vector<bool>(BitBoardPre::NUM_INDEXES, false);
  std::unordered_set<BitPieceInfo> moveSet_ = {};
  std::vector<bool> tappedSeen_ = std::vector<bool>(BitBoardPre::NUM_INDEXES, false);
  std::queue<BitPieceInfo> releasedTrueQ_;
  const int maxDropRem_;

  DoWork(int maxDropRem): maxDropRem_(maxDropRem) {}

  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType) {
    holdingSeen_[MoveDirection::LEFT].clear();
    holdingSeen_[MoveDirection::RIGHT].clear();
    moveSet_.clear();
    auto pieceInfo = b.getPiece(blockType);

    runHolding(pieceInfo, MoveDirection::LEFT);
    runHolding(pieceInfo, MoveDirection::RIGHT);
    runReleased(pieceInfo, false);
    
    while (!releasedTrueQ_.empty()) {
      auto top = releasedTrueQ_.front();
      releasedTrueQ_.pop();
      runReleased(top, true);
    }

    //printf("solution space: %lu\n", seen_.size());
    return {moveSet_.begin(), moveSet_.end()};
  }

  void runHolding(const BitPieceInfo& currentPiece, MoveDirection md) {
    runHolding(currentPiece, md, 1, maxDropRem_);
  }

  void runHolding(const BitPieceInfo &currentPiece, MoveDirection md, const int8_t dasRem, const int8_t dropRem) {  
    int8_t m = std::min(dasRem, dropRem);
    if (m > 0) {
      // this is messy and confusing
      // there is a two frame cooldown when you release
      // if you are moving down within the next frame, you MUST move down before sideways
      if (dropRem <= 1) {
        for (const auto &nxPiece: currentPiece.getClosedRotN()) {
          if (nxPiece.canMove(MoveDirection::DOWN)) {
            runReleased(currentPiece.move(MoveDirection::DOWN), false);
          }
          break;
        }
      } else {
        runReleased(currentPiece, false);
      }
      return runHolding(currentPiece, md, dasRem-m, dropRem-m);
    }
    //printf("(dasRem: %d, dropRem: %d)\n", dasRem, dropRem);
    //printBoardWithPiece(currentPiece.getBoard(), currentPiece);

    if (holdingSeen_[md].count(currentPiece.getRepId())) return;
    holdingSeen_[md].insert({currentPiece.getRepId(), true});

    const auto &closedRotN = currentPiece.getClosedRotN();

    if (dasRem == 0) {
      bool canMoveSideways = false;
      for (const auto &nxPiece: closedRotN) {
        if (nxPiece.canMove(md)) {
          if (!canMoveSideways) {
            runHolding(nxPiece.move(md), md, MAX_DAS_REM, dropRem);
          }
          canMoveSideways = true;
        }
        else runHolding(nxPiece, md, MAX_DAS_REM, dropRem);
      }
    }

    if (dropRem == 0) {
      bool canMoveDown = false;
      for (const auto &nxPiece: closedRotN) {
        if (nxPiece.canMove(MoveDirection::DOWN)) {
          if (!canMoveDown) runHolding(nxPiece.move(MoveDirection::DOWN), md, dasRem, maxDropRem_);
          canMoveDown = true;
        }
        else { moveSet_.insert(nxPiece); }
      }
    }
  }

  void runReleased(const BitPieceInfo &currentPiece, bool lastHitUsed) {
    if (lastHitUsed) {
      auto myPiece = currentPiece;
      while (true) {
        if (tappedSeen_[myPiece.getRepId()] || releasedSeen_[myPiece.getRepId()]) return;
        tappedSeen_[myPiece.getRepId()] =  true;
        bool canMove = false;
        auto movePiece = myPiece;
        for (const auto &nxPiece: myPiece.getClosedRotN()) {
          if (!nxPiece.canMove(MoveDirection::DOWN)) moveSet_.insert(nxPiece);
          else if (!canMove) { canMove = true, movePiece = nxPiece.move(MoveDirection::DOWN); };
        }
        if (canMove) {
          myPiece = movePiece;
        }
        else break;
      }

      return;
    }

    if (releasedSeen_[currentPiece.getRepId()]) return;
    releasedSeen_[currentPiece.getRepId()] = true;

    const auto &closedRotN = currentPiece.getClosedRotN();
    for (const auto &nxPiece: closedRotN) {
      if (nxPiece.canMove(MoveDirection::DOWN)) {
        const auto &endPiece = nxPiece.move(MoveDirection::DOWN);
        runReleased(endPiece, false);
      } else moveSet_.insert(nxPiece);
    }

    for (auto md: sidewaysMoveDirections) {
      for (const auto &nxPiece: closedRotN) {
        if (nxPiece.canMove(md)) {
          const auto &endPiece = nxPiece.move(md);
          releasedTrueQ_.push(endPiece);
          break;
        }
      }
    }
  }
};


std::vector<BitPieceInfo> MoveFinderRewrite::findAllMoves(const BitBoard& b, BlockType blockType) const {
  DoWork dw(maxDropRem_);
  return dw.findAllMoves(b, blockType);
}
