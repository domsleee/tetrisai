#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/ScoreManager.hpp"
#include "src/shared/MeMfPairProvider.h"
#include "src/shared/BranchSearcher/defs.h"

#include <queue>

#define BRANCH_SEARCHER_PARALLEL par_unseq

struct Node {
  Node(): p(BitBoard().getEmptyPiece()), sm({}) {}
  Node(BitPieceInfo _p, ScoreManager _sm, int lineClears): p(_p), sm(_sm), lineClears_(lineClears) {}
  BitPieceInfo p;
  ScoreManager sm;
  friend bool operator<(const Node &n1, const Node &n2) {
    if (n1.p != n2.p) return n1.p < n2.p;
    assert(false);
    return false;
  }
  int getLineClears() {
    return lineClears_;
  }
 private:
  int lineClears_ = 0;
};

namespace BranchSearcherNs {
  using NodePair = std::pair<double, Node>;
}


template<typename MyMoveFinder, typename BranchMoveFinder>
struct BranchSearcher {
  BranchSearcher(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider):
    meMfPairProvider_{std::make_unique<MeMfPairProvider<MyMoveFinder>>(meMfPairProvider)}
    {}

  BranchMoveFinder getMf(const ScoreManager &sm) {
    return BranchMoveFinder(sm.getLevel());
  }

  std::vector<std::pair<double, Node>> getTopN(const BitBoard &board, BlockType blockType, const ScoreManager &sm, int num) {
    auto mf = getMf(sm);
    if (board.hasNoMoves(blockType)) return {};
    auto allMoves = mf.findAllMoves(board, blockType);
    return getTopN(board, allMoves, sm, num);
  }

  std::vector<BranchSearcherNs::NodePair> getTopN(const BitBoard &board, const std::vector<BitPieceInfo> &allMoves, const ScoreManager &sm, int num) {
    using PairTT = std::pair<double, Node>;
    std::vector<PairTT> tempVec(allMoves.size(), {0, {}});
    std::priority_queue<PairTT, std::vector<PairTT>, std::greater<PairTT>> pq;
    
    auto [me, unused] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());
    auto fn = [&, this](const BitPieceInfo &move) -> PairTT {
      double score = me.evaluate(board, move, sm);
      auto sm2 = sm;
      auto [b2, lineClears] = board.applyPieceInfoCopy(move);
      sm2.addLineClears(lineClears);
      return PairTT(score, Node(move, sm2, lineClears));
    };

    std::transform(std::execution::BRANCH_SEARCHER_PARALLEL, allMoves.begin(), allMoves.end(), 
                  tempVec.begin(), fn);
    for (const auto &t: tempVec) pq.push(t);
    std::vector<BranchSearcherNs::NodePair> res;
    while (num-- && !pq.empty()) {
      res.push_back(pq.top());
      pq.pop();
    }
    return res;
  }

  double evalBoardWithPrev(const Node &n, BlockType prevBlockType, int depth) {
    return evalBoard(n, depth, prevBlockType, 0);
  }

  double evalBoardWithNext(const Node &n, BlockType nextBlockType, int depth) {
    BitBoard b = n.p.getBoard();
    b.applyPieceInfo(n.p);
    return evalBlockType(b, n.sm, nextBlockType, depth, 0);
  }

 private:
  double evalBoard(const Node &n, int depth, BlockType prevBlockType, int tetrises) {
    BitBoard b = n.p.getBoard();
    b.applyPieceInfo(n.p);
    std::vector<double> tempVec(allBlockTypes.size());
    auto fn = [&, this](const BlockType blockType) -> double {
      auto prob = blockType == prevBlockType ? 2.0/56 : 9.0/56;
      return prob * evalBlockType(b, n.sm, blockType, depth, tetrises);
    };
    std::transform(std::execution::BRANCH_SEARCHER_PARALLEL, allBlockTypes.begin(), allBlockTypes.end(), 
                  tempVec.begin(), fn);
    return std::accumulate(tempVec.begin(), tempVec.end(), 0.0);
  }

  double evalBlockType(const BitBoard &b, const ScoreManager &sm, BlockType blockType, int depth, int tetrises) {
    auto topN = getTopN(b, blockType, sm, BranchSearcherDefs::TOP_N_BLOCKS);

    if (topN.size() == 0) return 1e9; // bad
    if (depth == 0) {
      double offset = 0;
      auto lineClears = topN[0].second.getLineClears();
      if (lineClears == 4) {
        offset = 1e9;
      } else {
        auto [me, mf] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());
        //offset = -me.w_[MoveEvaluator::TOTAL_LINES_CLEARED] * topN[0].second.getLineClears();
      }
      //printf("tetrises: %d\n", tetrises);
      return topN[0].first + offset + tetrises * -1e9;
    }
      
    std::vector<double> results;
    for (auto [score, node]: topN) {
      int wasTetris = node.getLineClears() == 4;
      results.push_back(evalBoard(node, depth-1, blockType, tetrises + wasTetris));
      if (wasTetris) break;
    }
    return *std::min_element(results.begin(), results.end());
  }

 private:
  std::unique_ptr<MeMfPairProvider<MyMoveFinder>> meMfPairProvider_;

};
