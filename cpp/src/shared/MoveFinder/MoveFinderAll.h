#pragma once
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <vector>
#include <queue>
#include "src/common/Move.hpp"
#include "src/board/bitboard/BitBoard.h"
#include <memory>

namespace MoveFinderAllNs {
  struct Node {

    int getId() const { return id_; }
    void setId(int id) { id_ = id; }
    int getCooldownBitField() const { return cooldownBitField_; }
    void setCooldownBitField(int v) { assert(v <= 0b1111); cooldownBitField_ = v; }
    int getDropRem() const { return dropRem_; }
    void setDropRem(int v) { assert(v <= 0b111); dropRem_ = v; }
    void decDropRem(int maxDropRem) {
      if (--dropRem_ == 0) dropRem_ = maxDropRem;
    }
    void doAction(Action action) {
      id_ = BitBoardPre::doActionOnEmptyBoard(id_, action);
    }
    Node(const BitPieceInfo &p, int cooldownBitfield, int dropRem): Node(p.getId(), cooldownBitfield, dropRem) {}
    Node(const int v) {
      id_ = v >> 7;
      cooldownBitField_ = (v >> 3) & 0b1111;
      dropRem_ = v & 0b111;
    }
    Node(int id, int cooldownBitField, int dropRem):
      id_{id},
      cooldownBitField_{cooldownBitField},
      dropRem_{dropRem} {}

    int toInt() const {
      return (id_ << 7) + (cooldownBitField_ << 3) + dropRem_;
    }
   private:
    int id_;
    int cooldownBitField_;
    int dropRem_;
  };
  using ActionPair = std::pair<Action, int>;
};

class MoveFinderAll {
  std::array<bool, BitBoardPre::NUM_INDEXES> isFinalState_;
  std::vector<int> pred_ = std::vector<int>(BitBoardPre::NUM_INDEXES << (4 + 3 + 1), -1);

  int maxDropRem_ = 3;
 public:
  MoveFinderAll() {};
 
  std::vector<BitPieceInfo> findAllMoves(const BitBoard& b, BlockType blockType);
  
  std::vector<std::string> getShortestPath(const BitPieceInfo &piece) const;
  std::vector<std::pair<int, Action>> getShortestPathActions(const BitPieceInfo &piece) const;

  void setMaxDropRem(int maxDropRem) { maxDropRem_ = maxDropRem; }
  void setRecordEdges(bool ok) {}
  void setFirstMoveDirectionChar(char firstMoveDirectionChar) {}

 private:
  BitBoard b_;
  std::vector<int> lastToNode_ = std::vector<int>(BitBoardPre::NUM_INDEXES, -1);

  void applyTransition(std::queue<MoveFinderAllNs::Node> &q, MoveFinderAllNs::Node n, const std::pair<std::vector<Action>, int> &actionPairs);
  bool isSeen(const MoveFinderAllNs::Node &n) {
    // todo.
    return pred_[n.toInt()] != -1;
  }
  void addPred(const MoveFinderAllNs::Node &n1, const MoveFinderAllNs::Node &n2) {
    pred_[n2.toInt()] = n1.toInt();
    if (n2.toInt() == 321024 && n1.toInt() == 321025) {
      throw std::runtime_error("sdlfkj");
    }
    //pred_[n2.toInt()] = n1.toInt();
  }
  
};
