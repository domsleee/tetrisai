#include "src/shared/test/MoveEvaluatorUtility.hpp"
#include "src/shared/MoveFinder/MoveFinderRewrite.h"
#include <iostream>
#include "catch.h"

SCENARIO("large data") {
  const auto file = TEST_FOLDER + "/big_data3.in";
  std::ifstream fin(file);
  DISABLE_TEST;

  char boardStr[205];
  boardStr[200] = '\0';
  std::string line;
  while (std::getline(fin, line)) {
    std::stringstream ss(line);
    ss.read(boardStr, 200);
    int piece;
    char sep;

    // ,3,
    ss >> sep;
    assert(sep == ',');
    ss >> piece >> sep;
    assert(sep == ',');
    BitBoard b(boardStr);
    BlockType blockType = static_cast<BlockType>(piece);
    std::vector<int> expMoveInts;
    while (ss >> sep && sep != ']') {
      int id;
      ss >> id;
      expMoveInts.push_back(id);
    }

    auto mf = MoveFinderRewrite();
    mf.setMaxDropRem(2);
    auto moves = mf.findAllMoves(b, blockType);
    std::vector<int> moveInts;
    for (auto move: moves) {
      moveInts.push_back(move.getId());
    }

    /*
    std::cout << boardStr << " " << blockType << '\n';
    std::cout << b << '\n';
    std::set<int> expSet(expMoveInts.begin(), expMoveInts.end());
    for (auto v: moveInts) expSet.erase(v);
    std::cout << *expSet.begin() << '\n';
    printBoardWithPiece(b, b.getPieceFromId(*expSet.begin()));*/

    REQUIRE(moveInts.size() == expMoveInts.size());
    std::sort(moveInts.begin(), moveInts.end());
    std::sort(expMoveInts.begin(), expMoveInts.end());
    REQUIRE_THAT(moveInts, Catch::Matchers::Equals(expMoveInts));
  }
}
