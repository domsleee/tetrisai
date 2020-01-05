#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/SimpleBoard.h"
#include "src/board/bitboard/BitBoardPreExt.hpp"
#include <bitset>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cassert>
#include <iostream>


namespace BitBoardPre {
  const int MAX_IND = 12000; // NUM_BLOCKS * NUM_ROWS * NUM_COLUMNS * NUM_ROTATIONS
  const int NUM_BLOCKS = 8;
  const int NUM_MOVES = 4;
  const int NUM_ROTATIONS = 2;
  const int UNDEFINED = -1;
  
  void bfs(const SimplePieceInfo &p);
  inline int pieceInfoToId(const SimplePieceInfo &p);
  inline void addRotate(int id, int nxId, RotateDirection rotateDirection);
  inline void addMove(int id, int nxId, MoveDirection moveDirection);
  std::bitset<200> pieceInfoToBitset(const SimplePieceInfo &p);
  std::bitset<200> moveToBitset(const Move &move);
  void addEmptyPiece();
  inline int moveToId(const Move& move);
  void fixMoves();
  void setupHeights();
  void setupRepIds();
  void setupOpenRotN();
  bool isValidId(int id);

  int rotateIndex_[MAX_IND][NUM_ROTATIONS];
  int moveIndex_[MAX_IND][NUM_MOVES];
  int startingPieceId_[NUM_BLOCKS];
  std::vector<Move> idToMove_(MAX_IND);
  std::vector<int> idToHeight_(MAX_IND);
  std::vector<BlockType> idToBlockType_(MAX_IND, BlockType::I_PIECE);
  std::vector<int> idToRep_(MAX_IND, UNDEFINED);
  std::unordered_map<Move, int> moveToId_;
  std::bitset<200> idToBitset_[MAX_IND];
  std::vector<std::vector<int>> idToOpenRotN_(MAX_IND);
  std::unordered_map<BitPieceInfo, std::unordered_map<BitPieceInfo, std::vector<Action>>> shortestPathVec_;
  int emptyMoveId_ = -1;

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
    addEmptyPiece();
    setupHeights();
    setupRepIds();
    setupOpenRotN();

    // external
    shortestPathVec_ = BitBoardPreExt::setupAllShortestPaths();
  }

  const std::vector<Action>& getShortestPath(BitPieceInfo &p1, BitPieceInfo &p2) {
    return shortestPathVec_.at(p1).at(p2);
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
      for (auto moveDirection: validMoveDirections) {
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

  void addEmptyPiece() {
    Move move;
    move.coords_ = {};
    emptyMoveId_ = moveToId(move);
  }

  inline int pieceInfoToId(const SimplePieceInfo &p) {
    const auto &move = p.getPosition();
    int id = moveToId(move);
    idToBlockType_[id] = p.getBlockType();
    return id;
  }

  inline int moveToId(const Move& move) {
    if (moveToId_.count(move)) {
      return moveToId_[move];
    }
    int newId = moveToId_.size();
    idToBitset_[newId] = moveToBitset(move);
    idToMove_[newId] = move;
    moveToId_[move] = newId;
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

  int getEmptyMoveId() {
    return emptyMoveId_;
  }

  const std::bitset<200>& idToBitset(int id) {
    return idToBitset_[id];
  }

  const Move& idToMove(int id) {
    return idToMove_[id];
  }

  int getMoveFromId(const Move& move) {
    assert(moveToId_.count(move));
    return moveToId_[move];
  }

  int getRepIdFromId(const int id) {
    assert(isValidId(id));
    return idToRep_[id];
  }

  const std::vector<int>& getOpenRotN(int id) {
    assert(isValidId(id));
    return idToOpenRotN_[id];
  }

  bool isValidId(int id) {
    //printf("id: %d [%d, %lu)\n", id, 0, moveToId_.size());
    return id >= 0 && id < moveToId_.size();
  }

  std::bitset<200> moveToBitset(const Move &move) {
    std::bitset<200> b;
    for (const auto &coord: move.coords_) {
      b.set(coord.r*NUM_COLUMNS+coord.c);
    }
    return b;
  }

  // oof...
  void fixMoves() {
    for (auto move: idToMove_) {
      for (auto coord: move.coords_) {
        move.maxR = std::max(move.maxR, coord.r);
      }
    }
  }

  int getMoveHeight(int moveId) {
    return idToHeight_[moveId];
  }

  void setupHeights() {
    for (int i = 0; i < moveToId_.size(); ++i) {
      auto piece = idToMove(i);
      idToHeight_[i] = 0;
      for (auto coord: piece.coords_) {
        idToHeight_[i] = std::max(idToHeight_[i], NUM_ROWS-coord.r);
      }
    }
  }

  void setupRepIds() {
    auto b = BitBoard();
    for (int i = 0; i < moveToId_.size(); ++i) {
      auto piece = b.getPieceFromId(i);
      auto blockType = idToBlockType_[i];
      auto nxPiece = piece;
      int repId = i;

      auto rotDir = nxPiece.canRotate(RotateDirection::ROTATE_AC) ? RotateDirection::ROTATE_AC : RotateDirection::ROTATE_C;      
      for (int i = 0; i < 4; ++i) {
        if (idToRep_[nxPiece.getId()] != UNDEFINED) {
          repId = idToRep_[nxPiece.getId()];
          break;
        }
        if (nxPiece.canRotate(rotDir)) {
          nxPiece = nxPiece.rotate(rotDir);
        }
      }
      idToRep_[i] = repId;
    }
  }

  void setupOpenRotN() {
    auto b = BitBoard();
    for (int i = 0; i < moveToId_.size(); ++i) {
      auto piece = b.getPieceFromId(i);
      auto blockType = idToBlockType_[i];
      std::unordered_set<int> s, after;
      static const std::vector<RotateDirection> rotDirs = {RotateDirection::ROTATE_C, RotateDirection::ROTATE_AC};      
      for (auto rotDir: rotDirs) {
        if (piece.canRotate(rotDir)) {
          auto rotPiece = piece.rotate(rotDir);
          s.insert(rotPiece.getId());
          //printf("consider1: %d\n", rotPiece.getId());
          if (rotPiece.canRotate(rotDir)) { after.insert(rotPiece.rotate(rotDir).getId()); /*printf("consider2: %d\n", rotPiece.rotate(rotDir).getId()); */ }
        }
      }      
      s.erase(piece.getId());

      if (s.size() >= 4) {
        //std::cout << piece.getPosition() << '\n';;
        printf("PROBLEM - ACTUAL\n");
        piece.print();
        for (int v: s) {
          b.getPieceFromId(v).getPosition().print();
        }
        printf("id: %d\n", i);
        assert(false);
      }
      //assert(s.size() < 4);      
      idToOpenRotN_[i].clear();
      for (auto id: s) idToOpenRotN_[i].push_back(id);
      for (auto id: after) idToOpenRotN_[i].push_back(id);
    }
  }
}

