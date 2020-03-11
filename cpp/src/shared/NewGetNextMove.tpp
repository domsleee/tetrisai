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
  NewGetNextMove(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider):
    meMfPairProvider_(std::make_unique<MeMfPairProvider<MyMoveFinder>>(meMfPairProvider)) {};

  NewGetNextMove(const NewGetNextMove &getNextMove):
    NewGetNextMove(*(getNextMove.meMfPairProvider_)) {};

  BitPieceInfo getNextMove(const BitBoard& board, BlockType blockType, int level, char firstMoveChar='.') const;
  BitPieceInfo getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, int currentLineClears, char firstMoveChar='.') const;

  MyMoveFinder getMoveFinder(int numLines) {
    return meMfPairProvider_->getMeMfPair(numLines).second;
  }
 private:
  std::unique_ptr<MeMfPairProvider<MyMoveFinder>> meMfPairProvider_;
};


template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType, int level, char firstMoveChar) const {
  auto [me, mf] = meMfPairProvider_->getMeMfPair(level == 18 ? 129 : 130);
  if (firstMoveChar != '.') mf.setFirstMoveDirectionChar(firstMoveChar);
  auto allMoves = mf.findAllMoves(board, blockType);
#ifdef RECORD_MOVES
  Di::getMoveRecorder().recordMoves(board, blockType, allMoves);
#endif
  assert(allMoves.size() > 0);
  auto bestPiece = allMoves[0];
  double bestScore = 6e60;
  for (const auto& piece: allMoves) {
    double score = me.evaluate(board, piece, level);
    if (score < bestScore || (score == bestScore && piece < bestPiece)) {
      bestPiece = piece;
      bestScore = score;
    }
  }
  return bestPiece;
}

template<typename MyMoveFinder>
BitPieceInfo NewGetNextMove<MyMoveFinder>::getNextMove(const BitBoard &board, const BlockType blockType1, const BlockType blockType2, int currentLineClears, char firstMoveChar) const {
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
  if (firstMoveChar != '.') mf.setFirstMoveDirectionChar(firstMoveChar);
  assert(!board.hasNoMoves(blockType1));

  const auto moves = mf.findAllMoves(board, blockType1);
  auto fn = [&, this](const auto nxPiece) -> std::vector<SetT> {
    auto [nxBoard, lineClears] = board.applyPieceInfoCopy(nxPiece);
    int totalLineClears = currentLineClears + lineClears;
    double scoreOffset = lineClears == 4 ? -1e9 : 0;
    auto [me2, mf2] = meMfPairProvider_->getMeMfPair(totalLineClears);
    if (nxBoard.hasNoMoves(blockType2)) {
      return {{1e9, nxPiece, nxBoard.getEmptyPiece()}};
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

