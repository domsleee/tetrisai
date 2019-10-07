#pragma once
#include <unordered_set>
#include "src/common/Move.hpp"
#include "src/common/Board.hpp"
#include "src/board/SimpleBoard.h"


enum KeyStatus {
  LEFT_DOWN = 0,
  RIGHT_DOWN = 1,
  NEITHER = 2
};

namespace std {
  template<>
  struct hash<KeyStatus> {
    std::size_t operator()(const KeyStatus& k) {
      return k;
    }
  };
};

using MyTuple = std::tuple<SimplePieceInfo, KeyStatus, int, int, int, int>;

namespace std {
  template<>
  struct hash<MyTuple> {
    std::size_t operator()(const MyTuple &m) const {
      return std::hash<SimplePieceInfo>{}(std::get<0>(m)) ^
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
  std::vector<Move> findAllMoves(const SimpleBoard&, BlockType);
 private:
  std::unordered_set<MyTuple> seen_;
  std::unordered_set<Move> moves_;

  void dp(SimplePieceInfo currentPiece, KeyStatus keyStatus, int numReleases, int das, int numFrames, int numRotations);
};



