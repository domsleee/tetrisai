#pragma once
#include "src/shared/MoveFinder/AllMoveFinder.tpp"
#include "src/board/bitboard/BitBoard.h"

#include <unordered_map>

namespace CacheMoveFinderNs {
  template <typename MyBoard, typename MyBoardPieceInfo>
  using MyT = std::vector<std::unordered_map<MyBoard, std::vector<MyBoardPieceInfo>>>;
};

template <template<typename, typename> class MyMoveFinder = AllMoveFinder, typename MyBoard=BitBoard, typename MyBoardPieceInfo=BitPieceInfo>
class CacheMoveFinder {
 private:
  MyMoveFinder<MyBoard, MyBoardPieceInfo> move_finder_;
  static CacheMoveFinderNs::MyT<MyBoard, MyBoardPieceInfo> glob_map_;

 public:
  std::vector<BitPieceInfo> findAllMoves(const MyBoard& board, BlockType blockType) {
    if (glob_map_[blockType].count(board)) {
      return glob_map_[blockType][board];
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
};

template <template<typename, typename> class MyMoveFinder, typename MyBoard, typename MyBoardPieceInfo>
CacheMoveFinderNs::MyT<MyBoard, MyBoardPieceInfo> CacheMoveFinder<MyMoveFinder, MyBoard, MyBoardPieceInfo>::glob_map_(static_cast<size_t>(NUM_BLOCK_TYPES));
