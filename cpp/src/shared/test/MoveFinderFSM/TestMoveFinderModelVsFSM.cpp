
#include "catch.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include "src/board/BoardPrinter.tpp"
#include <vector>
#include <iostream>
#include <algorithm>

#include "src/shared/test/MoveEvaluatorUtility.hpp"

auto helper(int maxDropRem, int height, int c) {
  auto vs = getWell(height, c);
  BitBoard b = {vs};
  MoveFinderFSM mf1;
  MoveFinderRewrite mf2;
  mf1.setMaxDropRem(maxDropRem);
  mf2.setMaxDropRem(maxDropRem);
  auto blockType = BlockType::I_PIECE;
  auto movesFSM = mf1.findAllMoves(b, blockType);
  auto movesRewrite = mf2.findAllMoves(b, blockType);
  std::sort(movesFSM.begin(), movesFSM.end());
  std::sort(movesRewrite.begin(), movesRewrite.end());
  return std::pair(movesFSM, movesRewrite);
}


SCENARIO("I-PIECE has moves on empty board") {
  {
    auto b = BitBoard(getWell(12, 0));
    auto iPiece = b.getPiece(BlockType::I_PIECE);
    std::vector<Action> actions = {
      Action::LEFT,
      Action::DOWN,
      Action::DOWN,
      Action::ROTATE_AC,
      Action::LEFT, // 7
      Action::DOWN,
      Action::DOWN,
      Action::LEFT, // 13
      Action::DOWN,
      Action::DOWN,
      Action::LEFT, // 19
      // LEFT, DOWN      
    };

    for (auto action: actions) iPiece = iPiece.doAction(action);
    //printBoardWithPiece(b, iPiece);
    //printf("%d\n", iPiece.getId());
  }
  //return;

  auto [moves1, moves2] = helper(3, 12, 0);
  //REQUIRE_THAT(moves1, Catch::Matchers::Equals(moves2));

  
  for (int maxDropRem = 2; maxDropRem <= 3; ++maxDropRem) {
    for (int height = 0; height < 15; ++height) {
      for (int c = 0; c < NUM_COLUMNS; ++c) {
        auto [movesFSM, movesRewrite] = helper(maxDropRem, height, c);
        printf("maxDropRem: %d, height: %d, c: %d\n", maxDropRem, height, c);
        REQUIRE_THAT(movesFSM, Catch::Matchers::Equals(movesRewrite));
      }
    }
  }
 
}