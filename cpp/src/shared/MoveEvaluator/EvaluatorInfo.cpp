#include "src/shared/MoveEvaluator/EvaluatorInfo.hpp"

EvaluatorInfo::EvaluatorInfo(const BitBoard &b, const BitPieceInfo &p, const ScoreManager &sm):
    oldB_{b},
    p_{p},
    oldSm_{sm},
    appliedBoardVac_{b}
  {
    auto r = oldB_.applyPieceInfoCopy(p);
    newB_ = r.first;
    newSm_ = oldSm_;
    lineClears_ = r.second;
    newSm_.addLineClears(r.second);
    appliedBoardVac_ = {newB_}; // slow!
    colHeights_ = std::move(getColHeights(appliedBoardVac_));
  };
