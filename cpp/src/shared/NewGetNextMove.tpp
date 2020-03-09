#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/board/BoardPrinter.tpp"
#include "src/shared/di/di.h"
#include "src/shared/get_moves_utils.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MeMfPairProvider.h"

#define LOOKAHEAD_PARALLEL par_unseq

template<typename MyMoveFinder>
class NewGetNextMove {
 public:

  NewGetNextMove(const MoveEvaluatorGroup &me, const MyMoveFinder &mf, const MeMfPairProvider<MyMoveFinder> &meMfPairProvider):
    me_(std::make_unique<MoveEvaluatorGroup>(me)),
    mf_(std::make_unique<MyMoveFinder>(mf)),
    meMfPairProvider_(std::make_unique<MeMfPairProvider<MyMoveFinder>>(meMfPairProvider))
    {};

  NewGetNextMove(const MoveEvaluatorGroup &me, const MyMoveFinder &mf):
    NewGetNextMove(me, mf, MeMfPairProvider<MyMoveFinder>()) {};
  
  NewGetNextMove(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider):
    NewGetNextMove(meMfPairProvider.getMeMfPair(0).first, meMfPairProvider.getMeMfPair(0).second, meMfPairProvider) {};

  NewGetNextMove(const NewGetNextMove &getNextMove):
    NewGetNextMove(*(getNextMove.me_), *(getNextMove.mf_), *(getNextMove.meMfPairProvider_)) {};


  Move getNextMove(const BitBoard& board, BlockType blockType, int level) const;
  BitPieceInfo getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, int currentLineClears) const;

  void setMoveEvaluator(const MoveEvaluatorGroup &me) {
    me_ = std::make_unique<MoveEvaluatorGroup>(me);
  }
  void setMoveFinder(const MyMoveFinder &mf) {
    mf_ = std::make_unique<MyMoveFinder>(mf);
  }
  void setMaxDropRem(int maxDropRem) {
    mf_->setMaxDropRem(maxDropRem);
  }
  const MyMoveFinder& getMoveFinder() { return *mf_; }
  MyMoveFinder getMoveFinder(int numLines) {
    return meMfPairProvider_->getMeMfPair(numLines).second;
  }
 private:
  std::unique_ptr<MoveEvaluatorGroup> me_;
  std::unique_ptr<MyMoveFinder> mf_;
  std::unique_ptr<MeMfPairProvider<MyMoveFinder>> meMfPairProvider_;
};


template<typename MyMoveFinder>
Move NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType, int level) const {
  auto allMoves = mf_->findAllMoves(board, blockType);
#ifdef RECORD_MOVES
  Di::getMoveRecorder().recordMoves(board, blockType, allMoves);
#endif
  assert(allMoves.size() > 0);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  dprintf("NewGetNextMove: numMoves: %lu\n", allMoves.size());
  for (const auto& piece: allMoves) {
    double score = me_->evaluate(board, piece, level);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
    dprintf("NewGetNextMove: score: %0.2f\n", score);
  }
  dprintf("NewGetNextMove: bestScore: %.8f\n", bestScore);
  return bestPiece.getPosition();
}

template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, int currentLineClears) const {
  using SetT = std::tuple<double, BitPieceInfo, BitPieceInfo>;
  struct SetComp {
    bool operator()(const SetT t1, const SetT t2) const {
      if (std::get<0>(t1) != std::get<0>(t2)) {
        return std::get<0>(t1) < std::get<0>(t2);
      }
      return std::get<2>(t1) < std::get<2>(t2);
    }
  };
  
  std::set<SetT, SetComp> scores;
  auto [me, mf] = meMfPairProvider_->getMeMfPair(currentLineClears);
  assert(!board.hasNoMoves(blockType1));

  const auto moves = mf.findAllMoves(board, blockType1);
  auto fn = [&, this](const auto nxPiece) -> std::vector<SetT> {
    auto [nxBoard, lineClears] = board.applyPieceInfoCopy(nxPiece);
    int totalLineClears = currentLineClears + lineClears;
    double scoreOffset = lineClears == 4 ? -1e9 : 0;
    auto [me2, mf2] = meMfPairProvider_->getMeMfPair(totalLineClears);
    if (nxBoard.hasNoMoves(blockType2)) {
      return {{0, nxPiece, nxBoard.getEmptyPiece()}};
    }
    assert(!nxBoard.hasNoMoves(blockType2));
    auto innerMoves = mf2.findAllMoves(nxBoard, blockType2);
    
    auto innerFn = [&](const auto nxPiece2) -> SetT {
      auto score = me2.evaluate(nxBoard, nxPiece2, totalLineClears >= 130 ? 19 : 18);
      return {score + scoreOffset, nxPiece, nxPiece2};
    };
    std::vector<SetT> innerScores(innerMoves.size(), {0, board.getEmptyPiece(), board.getEmptyPiece()});
    std::transform(std::execution::LOOKAHEAD_PARALLEL, // par, seq, par_unseq
                  innerMoves.begin(), innerMoves.end(), 
                  innerScores.begin(), innerFn);
    return innerScores;
  };

  std::vector<std::vector<SetT>> result(moves.size());
  std::transform(std::execution::LOOKAHEAD_PARALLEL, // par, seq, par_unseq
                moves.begin(), moves.end(), 
                result.begin(), fn);
  for (auto vec: result) for (auto score: vec) scores.insert(score);
  return std::get<1>(*scores.begin());
}

