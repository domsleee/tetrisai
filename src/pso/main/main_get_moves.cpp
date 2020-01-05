
#include "src/common/Weighting.hpp"
#include "src/pso/ClientApi.hpp"

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

const char INSTRUCTION_HEALTH = 'h';
const char INSTRUCTION_GET_MOVE = 'g';
const char INSTRUCTION_QUIT = 'q';

const int LINE_TRANSITION = 100;

std::string best18 = "[-0.9981266671825644 6.314235664122202 9.218841471366666 0.27802110735477736 -4.1780630417598195 7.433743023240332 8.34877823356098 1.8788685887367613 -0.8499180634596748 -1.8078460542586594 7.642165019678991 -4.116700896835317 4.954418944561089 -12.37120499004913 -11.044493568377911 1.8097163771233449 -1.0832040443509103]";
std::string best19 = "[-14.89103037780527 6.1859328293901985 3.1475886283096397 4.549423856563027 6.4739513866334395 7.16934616046338 19.220915906005704 -1.5353264906442918 8.606784031245795 4.760696336241033 13.769672756691616 -2.0002884453295877 -2.4896370025512127 -8.345237667961287 -13.478496677643442 1.0396980163363931 2.178578911705224]";

auto w1 = WeightingFn::readFromString(best18);
auto w2 = WeightingFn::readFromString(best19);

void handleGetMove(int num_lines);
std::string getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2);


int main() {
  while(true) {
    char instruction;
    std::cin >> instruction;
    int num_lines;
    switch(instruction) {
      case(INSTRUCTION_HEALTH): {
        std::cout << "OK\n";
      } break;
      case (INSTRUCTION_GET_MOVE): handleGetMove(num_lines); break;
      case (INSTRUCTION_QUIT): exit(0);
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
  auto mf1 = MoveFinderRewrite();
  auto mf2 = MoveFinderRewrite();
  mf2.setMaxDropRem(2);
  if (num_lines < LINE_TRANSITION) {
    return std::pair(me1, mf1);
  }
  return std::pair(me2, mf2);
}

void handleGetMove(int num_lines) {
  int piece;
  std::string boardStr;

  std::cin >> piece;
  std::cin >> boardStr;

  //std::cout << "board str: " << boardStr << '\n';
  //std::cout << "piece: " << piece << '\n';

  BlockType blockType = static_cast<BlockType>(piece);
  auto board = BitBoard(boardStr);
  auto [me, mf] = getMeMfPair(num_lines);
  auto getNextMoveHandler = NewGetNextMove(me, mf);
  if (board.hasNoMoves(blockType)) {
    std::cout << "result: no moves\n";
    return;
  }
  std::cout << "result: moves\n";
  auto move = getNextMoveHandler.getNextMove(board, blockType);
  
  const auto lineClears = board.applyMove(move);
  const auto &mf_ = getNextMoveHandler.getMoveFinder();
  const auto &pred_ = mf_.getRecordedEdges();

  auto movePiece = board.getPiece(move);

  std::vector<BitPieceInfo> backwards;
  auto curr = movePiece;
  backwards.push_back(curr);
  while (pred_.count(curr)) {
    curr = *pred_.at(curr).begin();
    backwards.push_back(curr);
  }

  auto forwards = backwards;
  std::reverse(forwards.begin(), forwards.end());

  std::cout << "num moves: " << forwards.size()-1 << '\n';
  for (int i = 1; i < forwards.size(); ++i) {
    auto piece1 = forwards[i-1];
    auto piece2 = forwards[i];
    std::cout << getImmediateNeighbourStr(piece1, piece2);    
  }
  std::cout << "board: " << board << '\n';
  std::cout << "line clears: " << lineClears << '\n';
}

std::string getImmediateNeighbourStr(const BitPieceInfo &p1, const BitPieceInfo &p2) {
  // todo: can be precomputed with an empty board (if required)
  std::stringstream ss;
  printf("immediate...\n");

  auto addMoveToSS = [&](const BitPieceInfo &piece1, const BitPieceInfo &piece2, MoveDirection md) {
    assert(piece2 == piece1.move(md));
    ss << 0 << " m " << md << '\n';
  };
  auto addRotationToSS = [&](const BitPieceInfo &piece1, const BitPieceInfo &piece2) {
    for (auto rotateDirection: allRotateDirections) {
      if (piece1.canRotate(rotateDirection)) {
        auto nxPiece = piece1.rotate(rotateDirection);
        if (nxPiece == piece2) {
          ss << 0 << " r " << rotateDirection << '\n';
          return ss.str();
        }
      }
    }
    for (auto rotateDirection: allRotateDirections) {
      if (piece1.canRotate(rotateDirection)) {
        auto nxPiece = piece1.rotate(rotateDirection);
        if (nxPiece.canRotate(rotateDirection)) {
          auto nxPiece2 = nxPiece.rotate(rotateDirection);
          if (nxPiece2 == piece2) {
            ss << 0 << " r " << rotateDirection << '\n';
            ss << 0 << " r " << rotateDirection << '\n';
            return ss.str();
          }
        }
      }
    }
    throw new std::runtime_error("no rotate direction found...");
  };

  for (auto rotPiece: p1.getClosedRotN()) {
    if (rotPiece == p2) {
      addRotationToSS(p1, rotPiece);
      return ss.str();
    }

    for (auto moveDirection: validMoveDirections) {
      if (rotPiece.canMove(moveDirection)) {
        auto nxPiece = rotPiece.move(moveDirection);
        if (nxPiece == p2) {
          addRotationToSS(p1, rotPiece);
          addMoveToSS(rotPiece, p2, moveDirection);
          return ss.str();
        }
      }
    }
  }
  throw new std::runtime_error("unable to complete move!");
}
