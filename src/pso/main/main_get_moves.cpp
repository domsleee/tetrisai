
#include "src/common/Weighting.hpp"
#include "src/common/common.hpp"
#include "src/pso/ClientApi.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"

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

const bool MY_DEBUG = false;

const int LINE_TRANSITION = 90;
const int LINE_ON_LEVEL_19 = 130;
const int LINE_ON_LEVEL_29 = 230;

std::string best18 = "[-0.9981266671825644 6.314235664122202 9.218841471366666 0.27802110735477736 -4.1780630417598195 7.433743023240332 8.34877823356098 1.8788685887367613 -0.8499180634596748 -1.8078460542586594 7.642165019678991 -4.116700896835317 4.954418944561089 -12.37120499004913 -11.044493568377911 1.8097163771233449 -1.0832040443509103]";
std::string best19 = "[-14.89103037780527 6.1859328293901985 3.1475886283096397 4.549423856563027 6.4739513866334395 7.16934616046338 19.220915906005704 -1.5353264906442918 8.606784031245795 4.760696336241033 13.769672756691616 -2.0002884453295877 -2.4896370025512127 -8.345237667961287 -13.478496677643442 1.0396980163363931 2.178578911705224]";

auto w1 = WeightingFn::readFromString(best18);
auto w2 = WeightingFn::readFromString(best19);

void handleGetMove(int num_lines, bool givenFirstMoveDirection=false);
void handleGetMoveGivenNextPiece(int num_lines);
std::pair<int, std::string> getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2, int frame);

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

auto getMeMfPair(int num_lines) {
  auto me1 = MoveEvaluatorAdapter(MoveEvaluator(), w1);
  auto me2 = MoveEvaluatorAdapter(MoveEvaluator(), w2);
  
  auto mf1 = MoveFinderFSM();
  auto mf2 = MoveFinderFSM();
  auto mf3 = MoveFinderFSM();
  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);

  if (false && num_lines >= LINE_ON_LEVEL_29) {
    return std::pair(me2, mf3);
  }
  if (num_lines >= LINE_ON_LEVEL_19) {
    return std::pair(me2, mf2);
  }
  else if (num_lines >= LINE_TRANSITION) {
    return std::pair(me2, mf1);
  }
  std::cout << "HERE...\n";
  return std::pair(me1, mf1);
}

auto getNextMoveHandlerFactory(int num_lines) {
  auto [me, mf] = getMeMfPair(num_lines);
  return NewGetNextMove(me, mf);
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
  auto [me, mf] = getMeMfPair(num_lines);
  mf.setRecordEdges(true);
  if (givenFirstMoveDirection) mf.setFirstMoveDirectionChar(firstMoveDirectionChar);
  auto getNextMoveHandler = NewGetNextMove(me, mf);
  if (board.hasNoMoves(blockType)) {
    std::cout << "result: no moves\n";
    return;
  }
  std::cout << "result: moves\n";
  auto move = getNextMoveHandler.getNextMove(board, blockType);
  
  const auto oldBoard = board;
  const auto lineClears = board.applyMove(move);
  const auto &mf_ = getNextMoveHandler.getMoveFinder();

  const auto pieceInfo = oldBoard.getPiece(move);
  auto shortestPathStrings = mf_.getShortestPath(pieceInfo);
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

  auto [me, mf] = getMeMfPair(num_lines);
  mf.setFirstMoveDirectionChar(firstMoveDirectionChar);

  using SetT = std::tuple<double, BitPieceInfo, BitPieceInfo>;
  struct SetComp {
    bool operator()(const SetT t1, const SetT t2) {
      // todo: investigate nondeterminism
      return std::get<0>(t1) < std::get<0>(t2);
    }
  };

  std::set<SetT, SetComp> scores;
  const auto board = BitBoard(boardStr);
  for (const auto nxPiece: mf.findAllMoves(board, blockType1)) {
    auto [nxBoard, lineClears] = applyPieceInfo(board, nxPiece);
    int totalLineClears = num_lines + lineClears;
    double scoreOffset = lineClears == 4 ? -1e9 : 0;
    auto [me2, mf2] = getMeMfPair(totalLineClears);
    for (const auto nxPiece2: mf2.findAllMoves(nxBoard, blockType2)) {
      auto score = me2.evaluate(nxBoard, nxPiece2);
      scores.insert({score + scoreOffset, nxPiece, nxPiece2});
    }
  }

  if (scores.size() == 0) {
    std::cout << "result: no moves\n";
    return;
  }

  auto bestScoreTup = *scores.begin();
  dprintf("best score: %0.2f\n", std::get<0>(bestScoreTup));
  auto bestPieceInfo = std::get<1>(bestScoreTup);
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
