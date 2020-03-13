#include "src/shared/MoveRecorder/MoveRecorder.h"

// wtf
std::ofstream MoveRecorder::fout_;

void MoveRecorder::recordMoves(const BitBoard &board, const BlockType blockType, const std::vector<BitPieceInfo> &allMoves) {
  if (!fout_) {
    fout_.open("/tmp/hello.out");
  }
  fout_ << board << ',' << blockType << ',' << '[';
  for (auto i = 0; i < static_cast<int>(allMoves.size()); ++i) {
    fout_ << allMoves[i].getId();
    if (i != static_cast<int>(allMoves.size()) - 1) fout_ << ',';
  }
  fout_ << "]\n";
}
