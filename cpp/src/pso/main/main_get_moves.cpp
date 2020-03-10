
#include "src/shared/get_moves_utils.hpp"
#include "src/common/Weighting.hpp"
#include "src/common/common.hpp"
#include "src/pso/ClientApi.hpp"
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
MeMfPairProvider<MoveFinderFSM> getMeMfPairProvider(char firstMoveDirectionChar='.');

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

void handleGetMove(int num_lines, bool givenFirstMoveDirection) {
  int piece;
  char firstMoveDirectionChar;
  std::string boardStr;

  std::cin >> piece;
  if (givenFirstMoveDirection) std::cin >> firstMoveDirectionChar;
  std::cin >> boardStr;

  //std::cout << "firstMoveDirectionChar: " << firstMoveDirectionChar << '\n';
  //std::cout << "board str: " << boardStr << '\n';
  //std::cout << "piece: " << piece << '\n';

  BlockType blockType = static_cast<BlockType>(piece);
  auto board = BitBoard(boardStr);
  auto zz = getMeMfPairProvider(givenFirstMoveDirection ? firstMoveDirectionChar : '.');
  auto [me, mf] = zz.getMeMfPair(num_lines);
  auto getNextMoveHandler = NewGetNextMove(me, mf);

  if (board.hasNoMoves(blockType)) {
    std::cout << "result: no moves\n";
    return;
  }
  std::cout << "result: moves\n";
  auto move = getNextMoveHandler.getNextMove(board, blockType, 18); // todo
  
  const auto oldBoard = board;
  const auto lineClears = board.applyMove(move);

  auto mf2 = getNextMoveHandler.getMoveFinder();
  mf2.findAllMoves(oldBoard, blockType);
  const auto pieceInfo = oldBoard.getPiece(move);
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


void handleGetMoveGivenNextPiece(int num_lines) {
  int blockTypeInt1, blockTypeInt2;
  char firstMoveDirectionChar;
  std::string boardStr;
  std::cin >> blockTypeInt1;
  std::cin >> blockTypeInt2;
  std::cin >> firstMoveDirectionChar;
  std::cin >> boardStr;

  BlockType blockType1 = static_cast<BlockType>(blockTypeInt1);
  BlockType blockType2 = static_cast<BlockType>(blockTypeInt2);

  auto v = getMeMfPairProvider(firstMoveDirectionChar);
  auto getNextMoveHandler = NewGetNextMove(v);
  
  const auto board = BitBoard(boardStr);
  auto bestPieceInfo = getNextMoveHandler.getNextMove(board, blockType1, blockType2, num_lines);
  
  auto mf = getNextMoveHandler.getMoveFinder(num_lines);
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



MeMfPairProvider<MoveFinderFSM> getMeMfPairProvider(char firstMoveDirectionChar) {
  static std::map<bool, MoveEvaluatorGroup> cache;

  if (cache.count(false) == 0) {
    SummaryApi s(DATA_FOLDER);
    auto summary1 = s.readLogFile("18_MoveEvaluatorBlockLinear50_fixed.log");
    auto summary2 = s.readLogFile("19_MoveEvaluatorBlockLinear50_fixed.log");

    auto me1 = getMoveEvaluatorGroups().at(summary1.group).setWeights(summary1.weights);
    auto me2 = getMoveEvaluatorGroups().at(summary2.group).setWeights(summary2.weights);

    cache.insert({false, me1});
    cache.insert({true, me2});
  }
  
  auto me1 = cache.at(0);
  auto me2 = cache.at(1);

  auto mf1 = MoveFinderFSM();
  auto mf2 = MoveFinderFSM();
  auto mf3 = MoveFinderFSM();

  mf1.setMaxDropRem(3);
  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);

  if (firstMoveDirectionChar != '.') {
    mf1.setFirstMoveDirectionChar(firstMoveDirectionChar);
    mf2.setFirstMoveDirectionChar(firstMoveDirectionChar);
    mf3.setFirstMoveDirectionChar(firstMoveDirectionChar);
  }
  
  return MeMfPairProvider<MoveFinderFSM> (
    {
      {90, std::make_shared<MoveEvaluatorGroup>(me1)},
      {1000, std::make_shared<MoveEvaluatorGroup>(me2)}
    },
    {
      {130, std::make_shared<MoveFinderFSM>(mf1)},
      {230, std::make_shared<MoveFinderFSM>(mf2)},
      {1000, std::make_shared<MoveFinderFSM>(mf3)}
    }
  );
}