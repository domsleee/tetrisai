#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"

std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState& s1) const {
  return std::hash<int>{}(s1.fsmState_)
  ^ std::hash<bool>{}(s1.isLeftHolding_)
  ^ std::hash<BitPieceInfo>{}(s1.piece_)
  ^ std::hash<int>{}(s1.rotateCooldown_[0])
  ^ std::hash<int>{}(s1.rotateCooldown_[1])
  ^ std::hash<int>{}(s1.releaseCooldown_)
  ^ std::hash<int>{}(s1.moveCooldown_[0])
  ^ std::hash<int>{}(s1.moveCooldown_[1])
  ^ std::hash<int>{}(s1.moveCooldown_[2])
  ^ std::hash<int>{}(s1.dropRem_)
  ^ std::hash<int>{}(s1.dasRem_)
  ^ std::hash<int>{}(s1.frameEntered_);
}
