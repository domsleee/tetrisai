#pragma once
#include "src/board/bitboard/BitBoard.h"
#include "src/common/BlockType.hpp"
#include "src/common/common.hpp"
#include "src/common/Weighting.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluator.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorPenalty.hpp"
#include "src/shared/ScoreManager.hpp"
#include <vector>
#include <iostream>
#include <algorithm>


const std::string TEST_FOLDER = "/Users/dom/Documents/git/tetrisAI/src/shared/test/data";

BitBoard readBoard(const std::string &filePath);

Weighting getExpectedWeights(const std::string &filepath);

template<typename MyMoveEvaluator>
Weighting getWeights(const BitBoard &b, const BitPieceInfo &piece, const MyMoveEvaluator &me, int num_factors=NUM_FACTORS);

template<typename MyMoveEvaluator>
Weighting getWeightsFromEmptyPiece(const BitBoard &b, const MyMoveEvaluator &me);

Weighting getWeights2(const BitBoard &b, const BitPieceInfo &piece);

// todo: fix this up
Weighting getWeightsFromEmptyPiece2(const BitBoard &b);

BitBoard getBoardFromPartialStringVector(std::vector<std::string> strings);

#define REQ_IND(w, wExp, ind) REQUIRE(w[ind] == wExp[ind])
