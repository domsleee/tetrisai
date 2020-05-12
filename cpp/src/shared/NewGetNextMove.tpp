#pragma once
#include "src/common/Move.hpp"
#include "src/common/BlockType.hpp"
#include "src/board/bitboard/BitBoard.h"
#include "src/board/BoardPrinter.tpp"
#include "src/shared/di/di.h"
#include "src/shared/get_moves_utils.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include "src/shared/MeMfPairProvider.h"
#include "src/shared/MoveFinder/MoveFinderConstraints.h"
#include "src/shared/BranchSearcher/BranchSearcher.tpp"
#include "src/shared/BranchSearcher/defs.h"
#include "src/shared/MoveFinder/MoveFinderBfs.tpp"

#define LOOKAHEAD_PARALLEL par_unseq

template<typename MyMoveFinder>
class NewGetNextMove {
 public:
  NewGetNextMove(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider):
    meMfPairProvider_(std::make_unique<MeMfPairProvider<MyMoveFinder>>(meMfPairProvider)) {};

  NewGetNextMove(const NewGetNextMove &getNextMove):
    NewGetNextMove(*(getNextMove.meMfPairProvider_)) {};

  BitPieceInfo getNextMove(const BitBoard& board, BlockType blockType, const ScoreManager &sm, char firstMoveChar='.') const;
  BitPieceInfo getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, const ScoreManager &sm, char firstMoveChar=NO_CONSTRAINT) const;
  
  BitPieceInfo getNextMovePredict(const BitBoard &board, const BlockType blockType, const ScoreManager &sm) const;
  BitPieceInfo getNextMovePredict(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, const ScoreManager &sm) const;

  MyMoveFinder getMoveFinder(int numLines) {
    return meMfPairProvider_->getMeMfPair(numLines).second;
  }
 private:
  BitPieceInfo selectBestOrTetris(const std::vector<BranchSearcherNs::NodePair> &topMoves, BlockType nextBlockType) const;
  std::unique_ptr<MeMfPairProvider<MyMoveFinder>> meMfPairProvider_;
};


template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType, const ScoreManager &sm, char firstMoveChar) const {
  auto [me, mf] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());
  if (firstMoveChar != NO_CONSTRAINT) mf.setFirstMoveDirectionChar(firstMoveChar);
  auto allMoves = mf.findAllMoves(board, blockType);
#ifdef RECORD_MOVES
  Di::getMoveRecorder().recordMoves(board, blockType, allMoves);
#endif
  assert(allMoves.size() > 0);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  for (const auto& piece: allMoves) {
    double score = me.evaluate(board, piece, sm);
    //printf("score: %0.2f\n", score);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
  }
  return bestPiece;
}


template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, const ScoreManager &sm, char firstMoveChar) const {
  using SetT = std::tuple<double, BitPieceInfo, BitPieceInfo>;  
  std::set<SetT> scores;
  auto [me, mf] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());

  // when the piece appears, the person has already chosen a move (pieceInfo)
  // the person can change this move, but they must preserve the constraint (firstMoveDirection)
  //auto chosenPieceInfo = getNextMove(board, blockType1, sm);  
  /*if (firstMoveChar == NO_CONSTRAINT) {
    auto constraint = MoveFinderConstraintResolver<MyMoveFinder>::getConstraint(mf, chosenPieceInfo);
    if (constraint != NO_CONSTRAINT) mf.setFirstMoveDirectionChar(constraint);
  }*/

  assert(!board.hasNoMoves(blockType1));
  const auto moves = mf.findAllMoves(board, blockType1);
  auto fn = [&, this](const auto nxPiece) -> std::vector<SetT> {
    auto [nxBoard, lineClears] = board.applyPieceInfoCopy(nxPiece);
    auto sm2 = sm;
    sm2.addLineClears(lineClears);
    double scoreOffset = lineClears == 4 ? -1e9 : 0;
    auto [me2, mf2] = meMfPairProvider_->getMeMfPair(sm2.getTotalLines());
    if (nxBoard.hasNoMoves(blockType2)) {
      return {{1e9, nxPiece, nxBoard.getEmptyPiece()}};
    }
    auto innerMoves = mf2.findAllMoves(nxBoard, blockType2);

    auto innerFn = [&](const auto nxPiece2) -> SetT {
      auto score = me2.evaluate(nxBoard, nxPiece2, sm2);
      return {score + scoreOffset, nxPiece, nxPiece2};
    };
    std::vector<SetT> innerScores(innerMoves.size(), {0, board.getEmptyPiece(), board.getEmptyPiece()});
    std::transform(std::execution::LOOKAHEAD_PARALLEL, innerMoves.begin(), innerMoves.end(), 
                   innerScores.begin(), innerFn);
    return innerScores;
  };

  std::vector<std::vector<SetT>> result(moves.size());
  std::transform(std::execution::LOOKAHEAD_PARALLEL, moves.begin(), moves.end(), 
                 result.begin(), fn);

  for (auto vec: result) for (auto score: vec) scores.insert(score);

  /*printf("SCORES\n======================\n");
  printf("init score: %0.2f (move: %d)\n", me.evaluate(board, chosenPieceInfo, sm), chosenPieceInfo.getId());
  for (auto score: scores) {
    printf("score: %0.2f, move1: %d, move2: %d\n", std::get<0>(score), std::get<1>(score).getId(), std::get<2>(score).getId());
  }
  printf("=========================\n");*/
  return std::get<1>(*scores.begin());
}


template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMovePredict(const BitBoard &board, const BlockType blockType, const ScoreManager &sm) const {
  //return getNextMove(board, blockType, sm);
  BranchSearcher<MyMoveFinder, MoveFinderBfs> bs(*meMfPairProvider_);

  auto [me, mf] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());
  const auto moves = mf.findAllMoves(board, blockType);
  auto topMoves = bs.getTopN(board, moves, sm, BranchSearcherDefs::INIT_TOP_N_BLOCKS);
  assert(topMoves.size() > 0);
  double bestScore = 5e9;
  auto bestPiece = topMoves[0].second.p;
  for (auto [unusedScore, node]: topMoves) {
    if (node.getLineClears() == 4) return node.p;
    double score = bs.evalBoardWithPrev(node, blockType, BranchSearcherDefs::DEPTH);
    if (score < bestScore || (score == bestScore && node.p < bestPiece)) {
      bestPiece = node.p;
      bestScore = score;
    } 
  }

  return bestPiece;
}

template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMovePredict(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, const ScoreManager &sm) const {
  //return getNextMovePredict(board, blockType1, sm);
  BranchSearcher<MyMoveFinder, MoveFinderBfs> bs(*meMfPairProvider_);

  auto [me, mf] = meMfPairProvider_->getMeMfPair(sm.getTotalLines());
  const auto moves = mf.findAllMoves(board, blockType1);
  auto topMoves = bs.getTopN(board, moves, sm, BranchSearcherDefs::INIT_TOP_N_BLOCKS);
  assert(topMoves.size() > 0);
  double bestScore = 5e9;
  auto bestPiece = topMoves[0].second.p;
  for (auto [unusedScore, node]: topMoves) {
    if (node.getLineClears() == 4) return node.p;
    double score = bs.evalBoardWithNext(node, blockType2, BranchSearcherDefs::DEPTH_WITH_KNOWN_BLOCK);
    if (score < bestScore || (score == bestScore && node.p < bestPiece)) {
      bestPiece = node.p;
      bestScore = score;
    } 
  }
  return bestPiece;
}