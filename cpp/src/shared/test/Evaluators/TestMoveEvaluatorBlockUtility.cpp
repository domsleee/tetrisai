
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveEvaluator/MoveEvaluatorBlockUtility.hpp"
#include "src/shared/ScoreManager.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>

void helper(int maxDropRem, int height, int c) {
  auto move = Move({{R(3), c}, {R(2), c}, {R(1), c}, {R(0), c}});
  auto piece = BitBoard().getPiece(move);
  BitBoard b = {getWell(height, c)};
  auto isTrueByColAccessible = isBottomColAccessible(getColHeights(b).data(), 1);
  MoveFinderFSM mf;
  mf.setMaxDropRem(1);
  auto moves = mf.findAllMoves(b, BlockType::I_PIECE);
  auto isFoundByMoveFinder = std::find(moves.begin(), moves.end(), piece) != moves.end();
  // printf("maxDropRem: %d, c: %d, height: %d (%d == %d)\n", maxDropRem, c, height, isTrueByColAccessible ? 1 : 0, isFoundByMoveFinder ? 1 : 0);
  if (isTrueByColAccessible != isFoundByMoveFinder) {
    printBoard(b);
  }
  REQUIRE(isTrueByColAccessible == isFoundByMoveFinder);
}

SCENARIO("isColAccessible matches MoveFinderFSM opinion (level 29)") {
  helper(1, 19, 5);
  for (int c = 0; c < NUM_COLUMNS; ++c) {
    for (int height = 1; height < NUM_ROWS; ++height) {
      helper(1, height, c);
    }
  }
}