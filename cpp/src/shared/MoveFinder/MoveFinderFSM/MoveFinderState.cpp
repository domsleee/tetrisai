#include "src/shared/MoveFinder/MoveFinderFSM/MoveFinderState.hpp"

std::size_t std::hash<MoveFinderState>::operator()(const MoveFinderState& s1) const {
  return std::hash<BitPieceInfo>{}(s1.piece_)
  ^ std::hash<int>{}(s1.frameEntered_)
  ^ std::hash<uint32_t>{}(s1.counterRep_);
}
