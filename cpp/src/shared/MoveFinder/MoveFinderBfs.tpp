#pragma once
#include "src/board/SimpleBoard.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/bitboard/BitBoardPre.h"

#include <unordered_set>

class MoveFinderBfs {
 public:
  MoveFinderBfs() {}
  MoveFinderBfs(int level) {}
  void setFirstMoveDirectionChar(char c) {}
  void setMaxDropRem(int v) {}

  std::vector<BitPieceInfo> findAllMoves(const BitBoard& board, BlockType blockType) {
    std::fill(seen_.begin(), seen_.end(), false);
    std::fill(moves_.begin(), moves_.end(), false);
    auto pieceInfo = board.getPiece(blockType);
    auto height = board.getPileHeight();
    int canMoveDown = NUM_ROWS - height - 3;
    while (canMoveDown > 0) {
      pieceInfo = pieceInfo.move(MoveDirection::DOWN);
      canMoveDown--;
    }
    board_ = board;

    seen_[pieceInfo.getId()] = true;
    dp(pieceInfo.getId());
    std::vector<BitPieceInfo> ret;
    for (int i = 0; i < BitBoardPre::NUM_INDEXES; ++i) {
      if (moves_[i]) ret.push_back(board.getPieceFromId(i));
    }
    return ret;
  }

  void dp(const int id) {
    static std::array allActions = {Action::LEFT, Action::RIGHT, Action::DOWN, Action::ROTATE_AC, Action::ROTATE_C};

    for (auto action: allActions) {
      int nxId = BitBoardPre::doActionOnEmptyBoard(id, action);
      if (nxId == -1 || !board_.vacant(nxId) || seen_[nxId]) continue;
      seen_[nxId] = true;
      dp(nxId);
    }

    int nxId = BitBoardPre::doActionOnEmptyBoard(id, Action::DOWN);
    if (nxId == -1 || !board_.vacant(nxId)) moves_[id] = true;
  }
 private:
  std::vector<bool> seen_ = std::vector(BitBoardPre::NUM_INDEXES, false);
  std::vector<bool> moves_ = std::vector(BitBoardPre::NUM_INDEXES, false);
  BitBoard board_;
};
