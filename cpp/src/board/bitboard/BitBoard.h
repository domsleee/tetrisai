#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"
#include "src/common/PositionPieceGetter.hpp"
#include "src/common/RotateDirection.hpp"
#include "src/common/MoveDirection.hpp"
#include "src/common/common.hpp"
#include "src/board/bitboard/BitBoardPre.h"
#include <bitset>
#include <iostream>
#include <set>
#include <unordered_set>
#include <cassert>
#include "src/board/BoardPrinter.tpp"
#include "src/common/Action.h"

class BitBoard;
class BitPieceInfo;


template<>
struct std::hash<BitBoard> {
  std::size_t operator()(const BitBoard&) const;
};

class BitBoard {
 friend BitPieceInfo;
 public:
  using B = std::bitset<NUM_ROWS*NUM_COLUMNS>;
  BitBoard() { BitBoardPre::precompute(); };

  // whoever thought this was a good idea should be shot
  int applyPieceInfo(const BitPieceInfo&);
  
  std::pair<BitBoard, int> applyPieceInfoCopy(const BitPieceInfo &nxPiece) const {
    BitBoard nb = *this;
    int lineClears = nb.applyPieceInfo(nxPiece);
    return {nb, lineClears};
  }
  BitPieceInfo getPiece(BlockType blockType) const;
  bool vacant(const BitPieceInfo&) const;

  bool hasNoMoves(BlockType blockType) const;

  // probably shouldn't use
  BitPieceInfo getPieceFromId(int id) const;

  // convenience
  BitBoard(const std::vector<std::vector<int>>&);
  BitBoard(const std::string&);
  int applyMove(const Move&move);
  BitPieceInfo getPiece(const Move& position) const;
  BitPieceInfo getPiece(BlockType blockType, const Move& position) const;
  bool vacant(const Move&) const;
  bool vacant(const Coord&) const;
  bool vacant(int id) const;
  friend bool operator==(const BitBoard &b1, const BitBoard &b2) {
    return b1.bitset_ == b2.bitset_;
  }
  friend std::ostream& operator<<(std::ostream& os, const BitBoard& b) {
    auto s = b.bitset_.to_string();
    std::reverse(s.begin(), s.end());
    os << s;
    return os;
  }

  // hmmm.
  BitPieceInfo getEmptyPiece() const;
  int getPileHeight() const;

  friend size_t std::hash<BitBoard>::operator ()(const BitBoard&) const;


 private:
  B bitset_;
  int height_ = 0;
};



template<>
struct std::hash<BitPieceInfo> {
  std::size_t operator()(const BitPieceInfo&) const;
};

class BitPieceInfo {
  friend BitBoard;
 public:
  BlockType getBlockType() const { return BitBoardPre::getBlockTypeFromId(id_); }
  BitPieceInfo(const BitPieceInfo &other) : BitPieceInfo(other.id_, other.b_) {}
  bool canRotate(RotateDirection) const;
  BitPieceInfo rotate(RotateDirection) const;
  bool canMove(MoveDirection) const;
  BitPieceInfo move(MoveDirection) const;
  BitPieceInfo doAction(Action) const;
  Move getPosition() const;
  int getId() const { return id_; }
  int getRepId() const { return BitBoardPre::getRepIdFromId(id_); }
  const BitBoard& getBoard() const { return *b_; };

  std::vector<BitPieceInfo> getClosedRotN() const {
    {
      switch(getBlockType()) {
        case BlockType::O_PIECE: return {*this};
        case BlockType::I_PIECE:
        case BlockType::S_PIECE:
        case BlockType::Z_PIECE: {
          return getClosedRotNTwoPossibleRots();
        }
        case BlockType::J_PIECE:
        case BlockType::L_PIECE:
        case BlockType::T_PIECE: {
          return getClosedRotNFourPossibleRots();
        }
      }
    }
  }

  void print() const;

  friend bool operator==(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return p1.id_ == p2.id_;
  };
  friend bool operator!=(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return !(p1.id_ == p2.id_);
  };
  friend std::ostream& operator<<(std::ostream& os, const BitPieceInfo& p) {
    os << p.id_;
    return os;
  };
  friend bool operator<(const BitPieceInfo& p1, const BitPieceInfo& p2) {
    return p1.id_ < p2.id_;
  }

  friend size_t std::hash<BitPieceInfo>::operator ()(const BitPieceInfo&) const;

 private:
  BitPieceInfo(int id, const BitBoard *b) : id_(id), b_(b) {}
  int id_;
  const BitBoard* b_;

  inline std::vector<BitPieceInfo> getClosedRotNTwoPossibleRots() const {
    if (canRotate(RotateDirection::ROTATE_AC)) {
      return {*this, rotate(RotateDirection::ROTATE_AC)};
    }
    return {*this};
  }

  inline std::vector<BitPieceInfo> getClosedRotNFourPossibleRots() const {
    std::vector<BitPieceInfo> vs{*this};
    for (int id: BitBoardPre::getOpenRotN(id_)) {
      auto piece = b_->getPieceFromId(id);
      if (b_->vacant(piece)) {
        vs.push_back(piece);
      }
    }
    return vs;
  }


  // unused code

  std::vector<BitPieceInfo> __UNUSED_getClosedRotN_e() const {
    static std::unordered_map<int, std::unordered_map<BitBoard, std::vector<BitPieceInfo>>> my_m_ = {};
    if (my_m_.count(getRepId()) && my_m_[getRepId()].count(*b_)) {
      return my_m_[getRepId()][*b_];
    }
    if (!my_m_.count(getRepId())) my_m_[getRepId()] = {};
    return my_m_[getRepId()][*b_] = getClosedRotN();
  }

  bool __UNUSED_assertSizes(const std::vector<BitPieceInfo> &r1, const std::vector<BitPieceInfo> &r2) const {
    if (r1.size() != r2.size()) return false;
    return true;
  }

  std::vector<BitPieceInfo> __UNUSED_getClosedRotNFourPossibleRotsNew() const {
    std::vector<BitPieceInfo> vs{*this};

    bool halfTurn = false;
    for (auto rotDir: allRotateDirections) {
      auto nxPiece = *this;
      if (nxPiece.canRotate(rotDir)) {
        nxPiece = nxPiece.rotate(rotDir);
        vs.push_back(nxPiece);
        if (nxPiece.canRotate(rotDir)) {
          if (!halfTurn) {
            vs.push_back(nxPiece.rotate(rotDir));
            halfTurn = true;
          }
        }
      }
    }
    return vs;
  }
};
