#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/SimpleBoard.h"
#include <bitset>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cassert>
#include <iostream>


namespace BitBoardPre {
  const int NUM_BLOCKS = 8;
  const int NUM_MOVES = 4;
  const int NUM_ROTATIONS = 2;
  const int NUM_ACTIONS = NUM_MOVES + NUM_ROTATIONS + 1;
  const int UNDEFINED = -1;
  
  void bfs(const SimplePieceInfo &p);
  inline int pieceInfoToId(const SimplePieceInfo &p);
  inline void addRotate(int id, int nxId, RotateDirection rotateDirection);
  inline void addMove(int id, int nxId, MoveDirection moveDirection);
  std::bitset<NUM_ROWS * NUM_COLUMNS> pieceInfoToBitset(const SimplePieceInfo &p);
  std::bitset<NUM_ROWS * NUM_COLUMNS> moveToBitset(const Move &move);
  void addEmptyPiece();
  inline int moveToId(const Move& move);
  void fixMoves();
  void setupHeights();
  void setupRepIds(const std::vector<std::vector<int>> &idToOpenRotN);
  void setupOpenRotN();
  void precomputeActions();
  bool isValidId(int id);

  int rotateIndex_[MAX_IND][NUM_ROTATIONS];
  int moveIndex_[MAX_IND][NUM_MOVES];
  std::array<std::array<int, 7>, MAX_IND> actionIndex_;
  int startingPieceId_[NUM_BLOCKS];
  std::vector<Move> idToMove_(MAX_IND);
  std::vector<int> idToHeight_(MAX_IND);
  std::vector<BlockType> idToBlockType_(MAX_IND, BlockType::I_PIECE);
  std::vector<int> idToRep_(MAX_IND, UNDEFINED);
  std::unordered_map<Move, int> moveToId_;
  std::bitset<NUM_ROWS * NUM_COLUMNS> idToBitset_[MAX_IND];
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
      for (int j = 0; j < NUM_ACTIONS; j++) {
        actionIndex_[i][j] = UNDEFINED;
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
    setupOpenRotN();
    setupRepIds(idToOpenRotN_);
    precomputeActions();
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

  BlockType getBlockTypeFromId(int id) {
    assert(isValidId(id));
    return idToBlockType_[id];
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
    actionIndex_[id][static_cast<int>(toAction(rotateDirection))] = nxId;

  }

  inline void addMove(int id, int nxId, MoveDirection moveDirection) {
    moveIndex_[id][moveDirection] = nxId;
    actionIndex_[id][static_cast<int>(toAction(moveDirection))] = nxId;
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

  const std::bitset<NUM_ROWS * NUM_COLUMNS>& idToBitset(int id) {
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
    return id >= 0 && id < static_cast<int>(moveToId_.size());
  }

  std::bitset<NUM_ROWS * NUM_COLUMNS> moveToBitset(const Move &move) {
    std::bitset<NUM_ROWS * NUM_COLUMNS> b;
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
    for (int i = 0; i < static_cast<int>(moveToId_.size()); ++i) {
      auto piece = idToMove(i);
      idToHeight_[i] = 0;
      for (auto coord: piece.coords_) {
        idToHeight_[i] = std::max(idToHeight_[i], NUM_ROWS-coord.r);
      }
    }
  }

  void setupOpenRotN() {
    auto b = BitBoard();
    for (int i = 0; i < static_cast<int>(moveToId_.size()); ++i) {
      auto piece = b.getPieceFromId(i);
      std::unordered_set<int> s;
      static const std::vector<RotateDirection> rotDirs = {RotateDirection::ROTATE_C, RotateDirection::ROTATE_AC};
      for (auto rotDir: rotDirs) {
        auto myPiece = piece;
        for (int i = 0; i < 2; ++i) {
          if (myPiece.canRotate(rotDir)) {
            myPiece = myPiece.rotate(rotDir);
            s.insert(myPiece.getId());
          }
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
    }
  }

  void setupRepIds(const std::vector<std::vector<int>> &idToOpenRotN) {
    for (int i = 0; i < static_cast<int>(moveToId_.size()); ++i) {
      int repIdOverride = UNDEFINED;
      for (auto rotId: idToOpenRotN[i]) {
        if (idToRep_[rotId] != UNDEFINED) {
          if (repIdOverride != UNDEFINED && repIdOverride != idToRep_[rotId]) {
            throw std::runtime_error("Unexpected idToRep override");
          }
          repIdOverride = idToRep_[rotId];
        }
      }

      idToRep_[i] = repIdOverride != UNDEFINED ? repIdOverride : i;
    }
  }

  static std::vector<std::vector<std::pair<std::vector<Action>, int>>> actions_ = std::vector<std::vector<std::pair<std::vector<Action>, int>>>(2);

  int doActionOnEmptyBoard(int id, Action action) {
    return actionIndex_[id][static_cast<int>(action)];
  }

  void precomputeBitfieldToActions();
  std::vector<std::pair<std::vector<Action>, int>> precomputeActions(bool includeDown);

  void precomputeActions() {
    actions_[true] = precomputeActions(true);
    actions_[false] = precomputeActions(false);
    precomputeBitfieldToActions();
  }
  const std::vector<std::pair<std::vector<Action>, int>> &getActions(bool includeDown) {
    return actions_[includeDown];
  }

  int actionsToBitfield(const std::vector<Action> &actions);
  std::vector<std::pair<std::vector<Action>, int>> precomputeActions(bool includeDown) {
    std::vector<std::pair<std::vector<Action>, int>> res;
    constexpr std::array choices1 = {Action::NONE, Action::LEFT, Action::RIGHT};
    constexpr std::array choices2 = {Action::NONE, Action::ROTATE_C, Action::ROTATE_AC};
    std::array choices3 = {Action::DOWN};
    if (!includeDown) choices3[0] = Action::NONE;
    for (auto a1: choices1) {
      for (auto a2: choices2) {
        for (auto a3: choices3) {
          std::vector<Action> n;
          for (auto a: {a1, a2, a3}) {
            if (a != Action::NONE) n.push_back(a);
          }
          res.push_back({n, actionsToBitfield(n)});
        }
      }
    }
    return res;
  }
  int actionsToBitfield(const std::vector<Action> &actions) {
    int ret = 0;
    for (auto action: actions) {
      switch(action) {
        case Action::LEFT: ret |= 1; break;
        case Action::RIGHT: ret |= 1 << 1; break;
        case Action::ROTATE_C: ret |= 1 << 2; break;
        case Action::ROTATE_AC: ret |= 1 << 3; break;
        case Action::DOWN: ret |= 1 << 4; break;
        default: throw std::runtime_error("what");
      }
    }
    return ret;
  }
    

  static std::array<std::vector<Action>, (1<<5)+1> bitfieldToActionsCache_;
  std::vector<Action> precomputeBitfieldToActions(int bitfield);
  const std::vector<Action> &bitfieldToActions(int bitfield) {
    return bitfieldToActionsCache_[bitfield];
  }
  void precomputeBitfieldToActions() {
    for (int i = 0; i < (1<<5)+1; ++i) {
      bitfieldToActionsCache_[i] = precomputeBitfieldToActions(i);
    }
  }
  std::vector<Action> precomputeBitfieldToActions(int bitfield) {
    std::vector<Action> ret;
    if (bitfield & 1) ret.push_back(Action::LEFT);
    if (bitfield & (1 << 1)) ret.push_back(Action::RIGHT);
    if (bitfield & (1 << 2)) ret.push_back(Action::ROTATE_C);
    if (bitfield & (1 << 3)) ret.push_back(Action::ROTATE_AC);
    if (bitfield & (1 << 4)) ret.push_back(Action::DOWN);
    return ret;
  }
}

