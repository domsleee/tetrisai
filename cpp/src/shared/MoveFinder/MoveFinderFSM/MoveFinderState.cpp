#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"

std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState& s1) const {
  return std::hash<int>{}(s1.pieceId_)
  ^ std::hash<int>{}(s1.frameEntered_)
  ^ std::hash<uint32_t>{}(s1.counterVars_.getCounterRep());
}
