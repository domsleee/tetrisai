#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/board/SimpleBoard.h"
#include <bitset>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>


namespace BitBoardPre {
  const int MAX_IND = 200000; // NUM_BLOCKS * NUM_ROWS * NUM_COLUMNS * NUM_ROTATIONS
  const int NUM_BLOCKS = 8;
  const int NUM_MOVES = 4;
  const int NUM_ROTATIONS = 2;
  const int UNDEFINED = -1;
  
  void bfs(const SimplePieceInfo &p);
  inline int pieceInfoToId(const SimplePieceInfo &p);
  inline void addRotate(int id, int nxId, RotateDirection rotateDirection);
  inline void addMove(int id, int nxId, MoveDirection moveDirection);
  std::bitset<200> pieceInfoToBitset(const SimplePieceInfo &p);

  int rotateIndex_[MAX_IND][NUM_ROTATIONS];
  int moveIndex_[MAX_IND][NUM_MOVES];
  int startingPieceId_[NUM_BLOCKS];
  std::vector<Move> idToMove_(MAX_IND);
  std::unordered_map<Move, int> moveToId_;
  std::bitset<200> idToBitset_[MAX_IND];


  void precompute() {
    static int done = 0;
    if (done) return;
    done = 1;

    for (int i = 0; i < MAX_IND; i++) {
      for (int j = 0; j < NUM_ROTATIONS; j++) {
        rotateIndex_[i][j] = UNDEFINED;
      }
      for (int j = 0; j < NUM_MOVES; j++) {
        moveIndex_[i][j] = UNDEFINED;
      }
    }
    SimpleBoard b;
    for (const auto &blockType: allBlockTypes) {
      auto piece = b.getPiece(blockType);
      startingPieceId_[blockType] = pieceInfoToId(piece);
      bfs(piece);
    }
  }

  void bfs(const SimplePieceInfo &p) {
    std::queue<SimplePieceInfo> q;
    std::vector<RotateDirection> allRotateDirections = {RotateDirection::ROTATE_C, RotateDirection::ROTATE_AC};
    std::vector<MoveDirection> allMoveDirections = {MoveDirection::LEFT, MoveDirection::RIGHT, MoveDirection::DOWN};

    std::unordered_set<int> seen; 
    q.push(p);
    seen.insert(pieceInfoToId(p));

    while (!q.empty()) {
      const auto tp = q.front();
      int id = pieceInfoToId(tp);
      q.pop();      
      for (auto rotateDirection: allRotateDirections) {
        if (!tp.canRotate(rotateDirection)) continue;
        const auto &nxP = tp.rotate(rotateDirection);
        int nxId = pieceInfoToId(nxP);
        addRotate(id, nxId, rotateDirection);
        if (!seen.count(nxId)) {
          seen.insert(nxId);
          q.push(nxP);
        }
      }
      for (auto moveDirection: allMoveDirections) {
        if (!tp.canMove(moveDirection)) continue;
        const auto &nxP = tp.move(moveDirection);
        int nxId = pieceInfoToId(nxP);
        addMove(id, nxId, moveDirection);
        if (!seen.count(nxId)) {
          seen.insert(nxId);
          q.push(nxP);
        }
      }
    }
  }

  inline int pieceInfoToId(const SimplePieceInfo &p) {
    const auto &move = p.getPosition();
    if (moveToId_.count(move)) {
      return moveToId_[move];
    }
    int newId = moveToId_.size();
    idToBitset_[newId] = pieceInfoToBitset(p);
    idToMove_[newId] = p.getPosition();
    moveToId_[p.getPosition()] = newId;
    return newId;
  }

  inline void addRotate(int id, int nxId, RotateDirection rotateDirection) {
    rotateIndex_[id][rotateDirection] = nxId;
  }

  inline void addMove(int id, int nxId, MoveDirection moveDirection) {
    moveIndex_[id][moveDirection] = nxId;
  }

  int getStartingPieceId(BlockType blockType) {
    return startingPieceId_[blockType];
  }

  int getRotate(int id, RotateDirection rotateDirection) {
    return rotateIndex_[id][rotateDirection];
  }

  int getMove(int id, MoveDirection moveDirection) {
    return moveIndex_[id][moveDirection];
  }

  const std::bitset<200>& idToBitset(int id) {
    return idToBitset_[id];
  }

  const Move& idToMove(int id) {
    return idToMove_[id];
  }

  int moveToId(const Move& move) {
    if (!moveToId_.count(move)) {
      move.print();
      throw new std::runtime_error{"move not found!"};
    }
    return moveToId_[move];
  }

  std::bitset<200> pieceInfoToBitset(const SimplePieceInfo &p) {
    std::bitset<200> b;
    auto move = p.getPosition();
    for (const auto &coord: move.coords_) {
      b.set(coord.r*NUM_COLUMNS+coord.c);
    }
    return b;
  }
}

