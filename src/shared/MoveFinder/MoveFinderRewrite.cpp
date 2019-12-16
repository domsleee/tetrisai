#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/common/PositionPieceGetter.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/BoardPrinter.tpp"
#include <iostream>

#define GET_ROT_DIR(piece) (piece.canRotate(RotateDirection::ROTATE_AC) ? RotateDirection::ROTATE_AC : RotateDirection::ROTATE_C);      

std::vector<BitPieceInfo> MoveFinderRewrite::findAllMoves(const BitBoard& b, BlockType blockType) const {
  holdingSeen_[MoveDirection::LEFT].clear();
  holdingSeen_[MoveDirection::RIGHT].clear();
  releasedSeen_.clear();
  moveSet_.clear();
  auto pieceInfo = b.getPiece(blockType);

  runHolding(pieceInfo, MoveDirection::LEFT);
  runHolding(pieceInfo, MoveDirection::RIGHT);

  //printf("solution space: %lu\n", seen_.size());
  return {moveSet_.begin(), moveSet_.end()};
}

void MoveFinderRewrite::runHolding(const BitPieceInfo &currentPiece, MoveDirection md, int dasRem, int dropRem) const {  
  int m = std::min(dasRem, dropRem);
  if (m) return runHolding(currentPiece, md, dasRem-m, dropRem-m);

  if (holdingSeen_[md][currentPiece.getRepId()]) return;
  holdingSeen_[md][currentPiece.getRepId()] = true;

  //printf("GO\n");

  if (dasRem == 0) {
    auto movePiece = currentPiece;
    bool canMove = false, canStay = false;
    for (const auto &nxPiece: currentPiece.getClosedRotN()) {
      if (nxPiece.canMove(md)) { canMove = true, movePiece = nxPiece.move(md); }
      else { canStay = true; }
    }
    if (canMove) runHolding(movePiece, md, MAX_DAS_REM, dropRem);
    if (canStay) dropRem = 0;
  }

  if (dropRem == 0) {
    auto movePiece = currentPiece;
    bool canMove = false;
    for (const auto &nxPiece: currentPiece.getClosedRotN()) {
      if (nxPiece.canMove(MoveDirection::DOWN)) { canMove = true, movePiece = nxPiece.move(MoveDirection::DOWN); }
      else { moveSet_.insert(nxPiece); }
    }
    if (canMove) runHolding(movePiece, md, dasRem, maxDropRem_);
  }
  runReleased(currentPiece);
}


void MoveFinderRewrite::runReleased(const BitPieceInfo &currentPiece, bool lastHitUsed) const {
  if (lastHitUsed) {
    auto myPiece = currentPiece;
    while (true) {
      bool canMove = false;
      auto movePiece = myPiece;
      for (const auto &nxPiece: myPiece.getClosedRotN()) {
        if (!nxPiece.canMove(MoveDirection::DOWN)) moveSet_.insert(nxPiece);
        else { canMove = true, movePiece = nxPiece.move(MoveDirection::DOWN); };
      }
      if (canMove) myPiece = movePiece;
      else break;
    }

    return;
  }

  if (releasedSeen_[currentPiece.getRepId()]) return;
  releasedSeen_[currentPiece.getRepId()] = true;

  // todo: consider valid rotations here.
  std::vector<MoveDirection> consider = {MoveDirection::LEFT, MoveDirection::RIGHT};
  for (auto md: consider) {
    for (const auto &nxPiece: currentPiece.getClosedRotN()) {
      if (nxPiece.canMove(md)) {
        runReleased(nxPiece.move(md), true);
        break;
      }
    }
  }
  for (const auto &nxPiece: currentPiece.getClosedRotN()) {
    if (nxPiece.canMove(MoveDirection::DOWN)) {
      runReleased(nxPiece.move(MoveDirection::DOWN), false);
    } else moveSet_.insert(nxPiece);
  }
}