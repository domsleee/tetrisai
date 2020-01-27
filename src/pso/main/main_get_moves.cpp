
#include "src/pso/main/get_moves_utils.hpp"
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
    bool operator()(const SetT t1, const SetT t2) const {
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
