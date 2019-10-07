#include "src/shared/interfaces/IMoveFinder.hpp"
#include "src/common/PositionPieceGetter.hpp"

class AllMoveFinder: public IMoveFinder {
  std::vector<Move> findAllMoves(const Board& board, BlockType blockType) {
    // getStartingPosition
    PositionPieceGetter.getStartingPosition(blockType);

    // DP: coord(200), keyStatus(3), numKeyHits(2), das(16)
  }

  void getMoves
};
