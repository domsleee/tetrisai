
#include "src/common/Weighting.hpp"
#include "src/pso/ClientApi.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#define dprintf(...) fprintf(stderr, __VA_ARGS__)

const char INSTRUCTION_HEALTH = 'h';
const char INSTRUCTION_GET_MOVE = 'g';
const char INSTRUCTION_GET_MOVE_GIVEN_FIRST_DIRECTION = 'k';
const char INSTRUCTION_QUIT = 'q';

const char INSTRUCTION_SET_NUM_LINES = 'l';
const char INSTRUCTION_GET_NUM_LINES = 'L';

const bool MY_DEBUG = false;

const int LINE_TRANSITION = 100;
const int LINE_ON_LEVEL_19 = 130;

std::string best18 = "[-0.9981266671825644 6.314235664122202 9.218841471366666 0.27802110735477736 -4.1780630417598195 7.433743023240332 8.34877823356098 1.8788685887367613 -0.8499180634596748 -1.8078460542586594 7.642165019678991 -4.116700896835317 4.954418944561089 -12.37120499004913 -11.044493568377911 1.8097163771233449 -1.0832040443509103]";
std::string best19 = "[-14.89103037780527 6.1859328293901985 3.1475886283096397 4.549423856563027 6.4739513866334395 7.16934616046338 19.220915906005704 -1.5353264906442918 8.606784031245795 4.760696336241033 13.769672756691616 -2.0002884453295877 -2.4896370025512127 -8.345237667961287 -13.478496677643442 1.0396980163363931 2.178578911705224]";

auto w1 = WeightingFn::readFromString(best18);
auto w2 = WeightingFn::readFromString(best19);

void handleGetMove(int num_lines, bool givenFirstMoveDirection=false);
std::pair<int, std::string> getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2, int frame);

template<typename Mf>
std::vector<BitPieceInfo> getForwardLocations(const BitBoard &oldBoard, const BitPieceInfo &startingPiece, const Mf &mf);

template<typename MfT>
std::pair<std::vector<std::string>, int> getStrings(const BitBoard &oldBoard, const Move &move, MfT &mf_);

// notes
// - does drop come before das?
// - no level 19 support atm

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
  mf2.setMaxDropRem(2);
  if (num_lines >= LINE_ON_LEVEL_19) {
    return std::pair(me2, mf2);
  }
  else if (num_lines >= LINE_TRANSITION) {
    return std::pair(me2, mf1);
  }
  return std::pair(me1, mf1);
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

  //auto [strings, ct] = getStrings(oldBoard, move, mf_);
  const auto pieceInfo = oldBoard.getPiece(move);
  auto strings = mf_.getShortestPath(pieceInfo);
  auto ct = strings.size();

  std::cout << "num moves: " << ct << '\n';
  for (auto st: strings) std::cout << st;
  std::cout << "board: " << board << '\n';
  std::cout << "line clears: " << lineClears << '\n';
  std::cout << "OK\n";
}








// begin CANCER

std::string strRep(MoveDirection md) {
  switch(md) {
    case(MoveDirection::LEFT): return "LEFT";
    case(MoveDirection::RIGHT): return "RIGHT";
    case(MoveDirection::UP): return "UP";
    case(MoveDirection::DOWN): return "DOWN";
    default: throw std::runtime_error{"unknown move direction"};
  }
}

std::string strRep(RotateDirection rd) {
  switch(rd) {
    case(RotateDirection::ROTATE_AC): return "ROTATE_AC";
    case(RotateDirection::ROTATE_C): return "ROTATE_C";
    default: throw std::runtime_error{"unknown rotate direction"};
  }
}


template<typename MfT>
std::pair<std::vector<std::string>, int> getStrings(const BitBoard &oldBoard, const Move &move, MfT &mf_) {
  auto forwards = getForwardLocations(oldBoard, oldBoard.getPiece(move), mf_);
  for (auto piece: forwards) {
    printBoardWithPiece(piece.getBoard(), piece);
  }

  const auto &pred_frame = mf_.getPredFrame();
  std::vector<std::string> strings;
  int ct = 0;
  for (int i = 1; i < forwards.size(); ++i) {
    auto piece1 = forwards[i-1];
    auto piece2 = forwards[i];
    if (MY_DEBUG) {
      printBoardWithPiece(piece1.getBoard(), piece1);
      printBoardWithPiece(piece2.getBoard(), piece2);
    }
    int frame = pred_frame.at(piece2);
    auto [delta, st] = getImmediateNeighbourStr(piece1, piece2, frame);
    ct += delta;
    strings.push_back(st);
  }
  return {strings, ct};
}

template<typename Mf>
std::vector<BitPieceInfo> getForwardLocations(const BitBoard &oldBoard, const BitPieceInfo &startingPiece, const Mf &mf) {
  const auto &pred_ = mf.getRecordedEdges();
  const auto &pred_priority = mf.getPredPriority();
  std::vector<BitPieceInfo> backwards;
  auto curr = startingPiece;
  int priority = 500; // max
  backwards.push_back(curr);
  while (pred_.count(curr)) {
    bool ended = true;
    auto &edges = pred_.at(curr);
    for (auto nx: edges) {
      // at root.
      if (pred_priority.count(nx) == 0) {
        backwards.push_back(nx);
        ended = true;
        break;
      }
      if (pred_priority.count(nx) && pred_priority.at(nx) > priority) continue;
      priority = pred_priority.at(nx);
      curr = nx;
      if (MY_DEBUG) {
        printf("curr: %d\n", curr.getId());
        curr.getPosition().print();
        printf("pred.count(curr) %d, pred_priority.count(curr): %d (%d)\n", pred_.count(curr), pred_priority.count(curr), pred_priority.at(curr));
      }
      backwards.push_back(curr);
      ended = false;
      break;
    }
    if (ended) break;
  }

  auto forwards = backwards;
  std::reverse(forwards.begin(), forwards.end());
  for (int i = 0; i < forwards.size(); ++i) {
    forwards[i] = oldBoard.getPieceFromId(forwards[i].getId());
  }
  return forwards;
}

std::pair<int, std::string> getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2, int frame) {
  // todo: can be precomputed with an empty board (if required)
  std::stringstream ss;
  int ct = 0;

  auto addMoveToSS = [&](const BitPieceInfo &piece1, const BitPieceInfo &piece2, MoveDirection md) {
    assert(piece2 == piece1.move(md));
    ct++;
    ss << frame << " " << strRep(md) << '\n';
  };
  auto addRotationToSS = [&](const BitPieceInfo &piece1, const BitPieceInfo &piece2) {
    if (piece1 == piece2) return;
    for (auto rotateDirection: allRotateDirections) {
      if (piece1.canRotate(rotateDirection)) {
        auto nxPiece = piece1.rotate(rotateDirection);
        if (nxPiece == piece2) {
          ss << frame << " " << strRep(rotateDirection) << '\n';
          ct++;
          return;
        }
      }
    }
    for (auto rotateDirection: allRotateDirections) {
      if (piece1.canRotate(rotateDirection)) {
        auto nxPiece = piece1.rotate(rotateDirection);
        if (nxPiece.canRotate(rotateDirection)) {
          auto nxPiece2 = nxPiece.rotate(rotateDirection);
          if (nxPiece2 == piece2) {
            ss << frame << " " << strRep(rotateDirection) << '\n';
            ss << frame << " " << strRep(rotateDirection) << '\n';
            ct += 2;
            return;
          }
        }
      }
    }
    throw std::runtime_error("no rotate direction found...");
  };

  for (auto rotPiece: p1.getClosedRotN()) {
    if (rotPiece == p2) {
      addRotationToSS(p1, rotPiece);
      return {ct, ss.str()};
    }

    for (auto moveDirection: validMoveDirections) {
      if (rotPiece.canMove(moveDirection)) {
        auto nxPiece = rotPiece.move(moveDirection);
        if (nxPiece == p2) {
          addRotationToSS(p1, rotPiece);
          addMoveToSS(rotPiece, p2, moveDirection);
          return {ct, ss.str()};
        }
      }
    }
  }
  throw std::runtime_error("unable to complete move!");
}
