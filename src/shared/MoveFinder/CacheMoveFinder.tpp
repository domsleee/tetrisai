#pragma once
#include "src/shared/MoveFinder/AllMoveFinder.tpp"
#include "src/board/bitboard/BitBoard.h"

#include <unordered_map>

namespace CacheMoveFinderNs {
  template <typename MyBoard, typename MyBoardPieceInfo>
  using MyT = std::vector<std::unordered_map<MyBoard, std::vector<MyBoardPieceInfo>>>;
};

template <typename MyMoveFinder=AllMoveFinder<BitBoard, BitPieceInfo>, typename MyBoard=BitBoard, typename MyBoardPieceInfo=BitPieceInfo>
class CacheMoveFinder {
 private:
  MyMoveFinder move_finder_;
  mutable CacheMoveFinderNs::MyT<MyBoard, MyBoardPieceInfo> glob_map_;

  void reset() {
    glob_map_.assign(NUM_BLOCK_TYPES, {});
  }

 public:
  CacheMoveFinder() { reset(); }
  CacheMoveFinder(const MyMoveFinder &mf) {
    move_finder_ = mf;
    reset();
  }
  std::vector<BitPieceInfo> findAllMoves(const MyBoard& board, BlockType blockType) const {
    if (glob_map_.at(blockType).count(board)) {
      return glob_map_.at(blockType)[board];
    }

#ifdef MOVE_FINDER_CACHE
    auto res = move_finder_.findAllMoves(board, blockType);
    std::cout << board << '\n';
    std::cout << blockType << '\n';
    for (auto r: res) std::cout << r << ' '; std::cout << '\n';
    return glob_map_[blockType][board] = res;
#else
    return glob_map_[blockType][board] = move_finder_.findAllMoves(board, blockType);
#endif
  }

  void setMaxDropRem(int maxDropRem) {
    reset();
    move_finder_.setMaxDropRem(maxDropRem);
  }
};