#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/board/BoardPrinter.tpp"
#include "src/shared/di/di.h"
#include "src/shared/get_moves_utils.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"

template<typename MyMoveFinder>
class NewGetNextMove {
 public:
  NewGetNextMove(const MoveEvaluatorGroup &me, MyMoveFinder &mf): me_(std::make_unique<MoveEvaluatorGroup>(me)), mf_(std::make_unique<MyMoveFinder>(mf)) {}
  NewGetNextMove(const NewGetNextMove &getNextMove): me_(std::make_unique<MoveEvaluatorGroup>(*getNextMove.me_)), mf_(std::make_unique<MyMoveFinder>(*getNextMove.mf_)) {}
  Move getNextMove(const BitBoard& board, BlockType blockType, int level) const;
  BitPieceInfo getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, int currentLineClears) const;
  auto getMeMfPair(int numLines) const;

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
 private:
  std::unique_ptr<MoveEvaluatorGroup> me_;
  std::unique_ptr<MyMoveFinder> mf_;
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
  const auto moves = mf_->findAllMoves(board, blockType1);
  auto fn = [&, this](const auto nxPiece) -> std::vector<SetT> {
    auto [nxBoard, lineClears] = applyPieceInfo(board, nxPiece);
    int totalLineClears = currentLineClears + lineClears;
    double scoreOffset = lineClears == 4 ? -1e9 : 0;
    auto [me2, mf2] = this->getMeMfPair(totalLineClears);
    auto innerMoves = mf2.findAllMoves(nxBoard, blockType2);
    
    auto innerFn = [&](const auto nxPiece2) -> SetT {
      auto score = me2.evaluate(nxBoard, nxPiece2, totalLineClears >= 130 ? 19 : 18);
      return {score + scoreOffset, nxPiece, nxPiece2};
    };
    std::vector<SetT> innerScores(innerMoves.size(), {0, board.getEmptyPiece(), board.getEmptyPiece()});
    std::transform(std::execution::par_unseq, // par, seq, par_unseq
                  innerMoves.begin(), innerMoves.end(), 
                  innerScores.begin(), innerFn);
    return innerScores;
  };

  std::vector<std::vector<SetT>> result(moves.size());
  std::transform(std::execution::par_unseq, // par, seq, par_unseq
                moves.begin(), moves.end(), 
                result.begin(), fn);
  for (auto vec: result) for (auto score: vec) scores.insert(score);
  return std::get<1>(*scores.begin());
}

template<typename MyMoveFinder>
auto NewGetNextMove<MyMoveFinder>::getMeMfPair(int numLines) const {
  static const int LINE_TRANSITION = 90;
  static const int LINE_ON_LEVEL_19 = 130;
  static const int LINE_ON_LEVEL_29 = 230;
  
  auto me1 = getBestMoveEvaluatorLinear_50_fixed(false);
  auto me2 = getBestMoveEvaluatorLinear_50_fixed(true);
  
  auto mf1 = MoveFinderFSM();
  auto mf2 = MoveFinderFSM();
  auto mf3 = MoveFinderFSM();
  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);

  /*if (num_lines >= LINE_ON_LEVEL_29) {
    return std::pair(me2, mf3);
  }*/
  if (numLines >= LINE_ON_LEVEL_19) {
    return std::pair(me2, mf2);
  }
  else if (numLines >= LINE_TRANSITION) {
    return std::pair(me2, mf1);
  }
  return std::pair(me1, mf1);
}

