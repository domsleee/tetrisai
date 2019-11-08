#pragma once
#include <unordered_set>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"


enum KeyStatus {
  LEFT_DOWN = 0,
  RIGHT_DOWN = 1,
  NEITHER = 2
};

namespace std {
  template<>
  struct hash<KeyStatus> {
    std::size_t operator()(const KeyStatus& k) {
      return std::hash<int>{}(k);
    }
  };
};

using MyTuple = std::tuple<BitPieceInfo, KeyStatus, int, int, int, int>;

namespace std {
  template<>
  struct hash<MyTuple> {
    std::size_t operator()(const MyTuple &m) const {
      return std::hash<BitPieceInfo>{}(std::get<0>(m)) ^
             std::hash<KeyStatus>{}(std::get<1>(m)) ^
             std::hash<int>{}(std::get<2>(m)) ^
             std::hash<int>{}(std::get<3>(m)) ^
             std::hash<int>{}(std::get<4>(m)) ^
             std::hash<int>{}(std::get<5>(m));
    }
  };
};


class MoveFinder {
 public:
  std::vector<BitPieceInfo> findAllMoves(const BitBoard&, BlockType) const;
 private:
  mutable const BitBoard *b_; // like, this is temporary
  mutable std::unordered_set<MyTuple> seen_;
  mutable std::unordered_set<BitPieceInfo> moves_;
  void dp(BitPieceInfo currentPiece, KeyStatus keyStatus, int numReleases = 0, int das = 16, int numFrames = 0, int numRotations = 0) const;
  //void dp(BitPieceInfo, KeyStatus, int, int, int, int);
};



