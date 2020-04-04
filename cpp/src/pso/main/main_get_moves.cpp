
#include "src/shared/get_moves_utils.hpp"
#include "src/common/Weighting.hpp"
#include "src/common/common.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/pso/defs.h"
#include "src/pso/summary/SummaryApi.h"

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

const char INSTRUCTION_HEALTH = 'h';
const char INSTRUCTION_GET_MOVE = 'g';
const char INSTRUCTION_GET_MOVE_GIVEN_FIRST_DIRECTION = 'k';
const char INSTRUCTION_GET_MOVE_GIVEN_FIRST_DIRECTION_AND_NEXT_PIECE = 'p';
const char INSTRUCTION_QUIT = 'q';

const char INSTRUCTION_SET_NUM_LINES = 'l';
const char INSTRUCTION_GET_NUM_LINES = 'L';

void handleGetMove(int num_lines, bool givenFirstMoveDirection=false);
void handleGetMoveGivenNextPiece(int num_lines);
std::pair<int, std::string> getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2, int frame);
std::pair<MoveEvaluatorGroup, MoveEvaluatorGroup> getMePair();

// notes
// - does drop come before das?

void run();

int main() {
  try {
    run();
  } catch (std::runtime_error &e) {
    std::cout << "Caught an exception of an unexpected type: "
              << e.what () << '\n';
  }
}

void run() {
  char instruction;
  int num_lines = 0;
  while(std::cin >> instruction) {
    //dprintf("instruction: %c\n", instruction);
    switch(instruction) {
      case(INSTRUCTION_HEALTH): {
        std::cout << "OK\n";
      } break;
      case(INSTRUCTION_GET_MOVE): handleGetMove(num_lines); break;
      case(INSTRUCTION_GET_MOVE_GIVEN_FIRST_DIRECTION): handleGetMove(num_lines, true); break;
      case(INSTRUCTION_GET_MOVE_GIVEN_FIRST_DIRECTION_AND_NEXT_PIECE): handleGetMoveGivenNextPiece(num_lines); break;
      case(INSTRUCTION_SET_NUM_LINES): {
        std::cin >> num_lines;
      } break;
      case(INSTRUCTION_GET_NUM_LINES): {
        std::cout << "num lines: " << num_lines << '\n';
      } break;
      case(INSTRUCTION_QUIT): exit(0);
      default: {
        std::cout << "unknown command\n";
        exit(1);
      }
    }
  }
}

void handleGetMove(int numLines, bool givenFirstMoveDirection) {
  int piece;
  char firstMoveDirectionChar = '.';
  std::string boardStr;

  std::cin >> piece;
  if (givenFirstMoveDirection) std::cin >> firstMoveDirectionChar;
  std::cin >> boardStr;

  //std::cout << "firstMoveDirectionChar: " << firstMoveDirectionChar << '\n';
  //std::cout << "board str: " << boardStr << '\n';
  //std::cout << "piece: " << piece << '\n';

  BlockType blockType = static_cast<BlockType>(piece);
  auto board = BitBoard(boardStr);
  auto [me1, me2] = getMePair();
  auto zz = getMeMfPairProvider<MoveFinderFSM>(me1, me2);
  auto getNextMoveHandler = NewGetNextMove(zz);

  if (board.hasNoMoves(blockType)) {
    std::cout << "result: no moves\n";
    return;
  }
  std::cout << "result: moves\n";
  ScoreManager sm;
  sm.setLines(numLines);
  auto pieceInfo = getNextMoveHandler.getNextMove(board, blockType, sm, firstMoveDirectionChar); // todo
  
  const auto oldBoard = board;
  const auto lineClears = board.applyPieceInfo(pieceInfo);

  auto mf2 = getNextMoveHandler.getMoveFinder(numLines);
  mf2.setFirstMoveDirectionChar(firstMoveDirectionChar);
  mf2.findAllMoves(oldBoard, blockType);
  auto shortestPathStrings = mf2.getShortestPath(pieceInfo);
  auto ct = shortestPathStrings.size();

  std::cout << "num moves: " << ct << '\n';
  for (auto st: shortestPathStrings) std::cout << st;
  std::cout << "board: " << board << '\n';
  std::cout << "line clears: " << lineClears << '\n';
  std::cout << "OK\n";
}

std::pair<BitBoard, int> applyPieceInfo(const BitBoard &b, const BitPieceInfo &nxPiece) {
  BitBoard nb = b;
  int lineClears = nb.applyPieceInfo(nxPiece);
  return {nb, lineClears};
}


void handleGetMoveGivenNextPiece(int numLines) {
  int blockTypeInt1, blockTypeInt2;
  char firstMoveDirectionChar;
  std::string boardStr;
  std::cin >> blockTypeInt1;
  std::cin >> blockTypeInt2;
  std::cin >> firstMoveDirectionChar;
  std::cin >> boardStr;

  BlockType blockType1 = static_cast<BlockType>(blockTypeInt1);
  BlockType blockType2 = static_cast<BlockType>(blockTypeInt2);

  auto [me1, me2] = getMePair();
  auto v = getMeMfPairProvider<MoveFinderFSM>(me1, me2);
  auto getNextMoveHandler = NewGetNextMove(v);
  
  const auto board = BitBoard(boardStr);
  if (board.hasNoMoves(blockType1)) {
    std::cout << "result: no moves\n";
    return;
  }
  ScoreManager sm;
  sm.setLines(numLines);
  auto bestPieceInfo = getNextMoveHandler.getNextMove(board, blockType1, blockType2, sm);
  
  auto mf = getNextMoveHandler.getMoveFinder(numLines);
  //mf.setFirstMoveDirectionChar(firstMoveDirectionChar);
  mf.findAllMoves(board, blockType1);
  auto shortestPathStrings = mf.getShortestPath(bestPieceInfo);
  auto ct = shortestPathStrings.size();

  auto [nxBoard, lineClears] = applyPieceInfo(board, bestPieceInfo);

  std::cout << "result: moves\n";
  std::cout << "num moves: " << ct << '\n';
  for (auto st: shortestPathStrings) std::cout << st;
  std::cout << "board: " << nxBoard << '\n';
  std::cout << "line clears: " << lineClears << '\n';
  std::cout << "OK\n";
}

std::pair<MoveEvaluatorGroup, MoveEvaluatorGroup> getMePair() {
  static std::map<bool, MoveEvaluatorGroup> cache;
  if (cache.count(false) == 0) {
    SummaryApi s(DATA_FOLDER);
    const std::string h = "BothLinearAdvVarN"; // MoveEvaluatorBlockLinear50_fixed
    //auto summary1 = s.readLogFile("18_LinearN.log");
    //auto summary2 = s.readLogFile("19_BothLinearN.log");
    auto summary1 = s.readLogFile("18_LinearGenFin.log");
    auto summary2 = s.readLogFile("19_BothLinearGen.log");

    auto me1 = getMoveEvaluatorGroups().at(summary1.group).setWeights(summary1.weights);
    auto me2 = getMoveEvaluatorGroups().at(summary2.group).setWeights(summary2.weights);

    cache.insert({false, me1});
    cache.insert({true, me2});
  }

  return {cache.at(false), cache.at(true)};
}
