#include "src/board/bitboard/BitBoardPre.h"
#include "src/board/bitboard/BitBoard.h"

#include <vector>
#include <unordered_map>

namespace BitBoardPreExt {
  using GraphT = std::unordered_map<BitPieceInfo, std::unordered_map<BitPieceInfo, std::vector<BitBoardPre::Action>>>;
  GraphT setupAllShortestPaths() {
    GraphT res;
    return res;
  }
}