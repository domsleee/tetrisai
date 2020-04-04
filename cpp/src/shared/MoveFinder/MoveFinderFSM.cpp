#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/common/MoveDirection.hpp"
#include "src/shared/MoveFinder/MoveFinderConstraints.h"
#include <queue>
#include <vector>
#include <sstream>

#define SEEN_EMPLACE(seen, v) seen.emplace(v);
const int MAX_DAS_REM = 6;

void onEnterReleased(MoveFinderState &s);
void onEnterTapped(MoveFinderState &s);


using ReleasedSeenT = std::bitset<64>; // 2^(4+2)

struct BfsInfo {
  BfsInfo(const std::vector<RotateDirection> &rds): rotateDirections{rds} {
    seen.reserve(2e5);
  }
  std::vector<RotateDirection> rotateDirections;
  FSMTypes::SeenT seen;
  std::queue<FSMTypes::PairT> releasedQ;
  std::vector<ReleasedSeenT> releasedSeen = decltype(releasedSeen)(BitBoardPre::NUM_INDEXES);
  std::vector<std::vector<bool>> tappedSeen = decltype(tappedSeen)(BitBoardPre::NUM_INDEXES, std::vector<bool>(5, false));
  FSMTypes::MovesT moves = FSMTypes::MovesT(BitBoardPre::NUM_INDEXES, false);
  FSMTypes::PairT qTop() {
#if MOVE_FINDER_FSM_PERFORMANCE == 1
    return q.front();
#else
    return q.top();
#endif
  }
  void qPop() { q.pop(); }
  void qPush(const FSMTypes::PairT &v) { q.push(v); }
  bool qEmpty() { return q.empty(); }
 private:
   
  #if MOVE_FINDER_FSM_PERFORMANCE == 1
    std::queue<FSMTypes::PairT> q;
  #else
    struct Comp {
      bool operator()(const FSMTypes::PairT &l, const FSMTypes::PairT &r) {
        return l.first >= r.first;
      };
    };
    std::priority_queue<FSMTypes::PairT, std::vector<FSMTypes::PairT>, Comp> q;
  #endif

};

struct TopInfo {
  TopInfo(double topScore, const MoveFinderState &top, const BitBoard &b):
    top{top},
    topScore{topScore},
    topPiece{top.getPiece(b)}
  {}
  
  const MoveFinderState top;
  const double topScore;
  const BitPieceInfo topPiece;
};


void safeInsert(BfsInfo &bfsInfo, const BitBoard &b, MoveFinderState state) {
  if (b.vacant(state.getPiece(b))) {
    bfsInfo.seen.emplace(state);
    bfsInfo.qPush({0, state});
  }
}

const std::vector<RotateDirection> &getRotateDirections(const BitPieceInfo &p) {
  static const std::vector<RotateDirection> noRotations = {};
  static const std::vector<RotateDirection> oneRotation = {RotateDirection::ROTATE_AC};
  switch(p.getBlockType()) {
    case BlockType::O_PIECE: return noRotations;
    case BlockType::I_PIECE:
    case BlockType::S_PIECE:
    case BlockType::Z_PIECE: return oneRotation;
    default: return allRotateDirections;
  }
}

std::vector<BitPieceInfo> MoveFinderFSM::findAllMoves(const BitBoard& b, BlockType blockType) {
  pred_.clear();
  pred_.reserve(2e5);
  finalMoveToState_.clear();
  BfsInfo bfsInfo(getRotateDirections(b.getPiece(blockType)));

  setupInitialStates(bfsInfo, b, blockType);

  while (!bfsInfo.qEmpty()) {
    runNode(b, bfsInfo);
  }
  while (!bfsInfo.releasedQ.empty()) {
    const auto [topScore, top] = bfsInfo.releasedQ.front();
    bfsInfo.releasedQ.pop();
    TopInfo topInfo(topScore, top, b);
    runNode(topInfo, bfsInfo);
  }
  while (!bfsInfo.qEmpty()) {
    runNode(b, bfsInfo);
  }

  std::vector<BitPieceInfo> ret;
  for (int i = 0; i < BitBoardPre::NUM_INDEXES; ++i) {
    if (bfsInfo.moves[i]) ret.push_back(b.getPieceFromId(i));
  }
  //printf("stateSpace: %lu\n", bfsInfo.seen.size());
  return ret;
}

void MoveFinderFSM::setupInitialStates(BfsInfo &bfsInfo, const BitBoard &b, BlockType blockType) {
  auto s1 = MoveFinderState(b.getPiece(blockType), true, maxDropRem_);
  auto s2 = MoveFinderState(b.getPiece(blockType), false, maxDropRem_);
  for (auto s: {&s1, &s2}) {
    s->setReleaseCooldown(2);
    s->setDasRem(2);
    s->setRotateCooldown(2);
  }
  auto s3(s2);
  onEnterReleased(s3);
  s3.setFsmState(FSMState::RELEASED);
  s3.setRotateCooldown(2);
  s3.setSidewaysMoveCooldown(2);

  if (!hasFirstMoveConstraint_) {
    safeInsert(bfsInfo, b, s1);
    safeInsert(bfsInfo, b, s2);
    safeInsert(bfsInfo, b, s3);
  } else {
    switch(firstMoveDirectionChar_) {
      case FIRST_MOVE_DIRECTION_LEFT: { safeInsert(bfsInfo, b, s1); } break;
      case FIRST_MOVE_DIRECTION_RIGHT: { safeInsert(bfsInfo, b, s2); } break;
      case FIRST_MOVE_DIRECTION_NEITHER: { safeInsert(bfsInfo, b, s3); } break;
      default: throw std::runtime_error("Unknown firstMoveDirectionChar_");
    }
  }
}

void MoveFinderFSM::runNode(const BitBoard &b, BfsInfo &bfsInfo) {
  const auto [topScore, top] = bfsInfo.qTop();
  bfsInfo.qPop();
  TopInfo topInfo(topScore, top, b);
  runNode(topInfo, bfsInfo);
}

void MoveFinderFSM::runNode(const TopInfo& topInfo, BfsInfo &bfsInfo) {
  switch(topInfo.top.getFsmState()) {
    case FSMState::HOLDING: runHolding(topInfo, bfsInfo); break;
    case FSMState::RELEASED: runReleased(topInfo, bfsInfo); break;
    case FSMState::TAPPED_ONCE: runTapped(topInfo, bfsInfo); break;
  }
}

std::vector<bool>::reference tappedSeenPtr(const TopInfo &topInfo, BfsInfo& bfsInfo) {
  if (bfsInfo.rotateDirections.size() == 0) return bfsInfo.tappedSeen[topInfo.top.getPieceId()].at(0);
  else if (bfsInfo.rotateDirections.size() == 1) {
    int v = std::min(topInfo.top.getRotateCooldown(RotateDirection::ROTATE_AC), topInfo.top.getRotateCooldown(RotateDirection::ROTATE_C));
    return bfsInfo.tappedSeen[topInfo.top.getPieceId()].at(v);
  } else {
    // you can rotate around the other way in min+2
    // what if you can't rotate the other way & the cooldown is 1 (which can happen)?
    // ANS: you will be able to rotate the frame after (because there is at least 1 frame until drop, and rotate is before drop)
    int v = std::min(topInfo.top.getRotateCooldown(RotateDirection::ROTATE_AC), topInfo.top.getRotateCooldown(RotateDirection::ROTATE_C));
    return bfsInfo.tappedSeen[topInfo.top.getPieceId()].at(v+2);
  }
  throw std::runtime_error("what");
}

void MoveFinderFSM::runHolding(const TopInfo &topInfo, BfsInfo &bfsInfo) {
  // either keep holding or release.
  // you MUST move across if dasRem is zero when you're holding
  auto moveDirection = topInfo.top.getIsLeftHolding() ? MoveDirection::LEFT : MoveDirection::RIGHT;
  auto otherMoveDirection = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
  if (topInfo.top.getDasRem() == 0) {
    //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
    if (topInfo.top.getMoveCooldown(moveDirection) != 0) return; // cant proceed
    if (topInfo.topPiece.canMove(moveDirection)) {
      auto nxMoved = topInfo.top;
      nxMoved.setPiece(topInfo.topPiece.move(moveDirection));
      nxMoved.setDasRem(MAX_DAS_REM - (nxMoved.frameEntered_ == 2));
      //nxMoved.setRotateCooldown(1);
      auto [ignore, inserted] = bfsInfo.seen.emplace(nxMoved);
      if (inserted == false) return;
      addEdge(topInfo.top, nxMoved, moveDirection);
      bfsInfo.qPush({topInfo.topScore+1, nxMoved});
      return;
    }
  }

  // releasing...
  if (topInfo.top.getReleaseCooldown() == 0 && ((topInfo.top.frameEntered_ < 8 && topInfo.top.getDasRem() > 2) || (topInfo.top.getDasRem() == MAX_DAS_REM))) {
    auto nxReleased = topInfo.top;
    nxReleased.setFsmState(FSMState::RELEASED);
    nxReleased.setMoveCooldown(moveDirection, 1+1);
    nxReleased.setMoveCooldown(otherMoveDirection, 1); // must come after
    onEnterReleased(nxReleased);
    auto [ignore, inserted] = bfsInfo.seen.emplace(nxReleased);
    if (inserted) {
      addEdge(topInfo.top, nxReleased, Action::NONE);
      bfsInfo.qPush({topInfo.topScore, nxReleased});
    }
  }
  considerRotate(topInfo, bfsInfo);
  if (considerMovingDown(topInfo, bfsInfo)) return;
  addNxFrame(topInfo, bfsInfo);
}

int getRotRep(const TopInfo &topInfo, BfsInfo &bfsInfo) {
  if (bfsInfo.rotateDirections.size() == 0) return 0;
  else if (bfsInfo.rotateDirections.size() == 1) return std::min(topInfo.top.getRotateCooldown(RotateDirection::ROTATE_C), topInfo.top.getRotateCooldown(RotateDirection::ROTATE_AC));
  else return topInfo.top.getAllRotateCooldowns();
  throw std::runtime_error("what");
}

ReleasedSeenT::reference getReleasedSeenRef(const TopInfo &topInfo, BfsInfo &bfsInfo) {
  return bfsInfo.releasedSeen[topInfo.topPiece.getId()][topInfo.top.getAllMoveCooldowns() + (getRotRep(topInfo, bfsInfo) << 4)];
}

void MoveFinderFSM::runReleased(const TopInfo &topInfo, BfsInfo &bfsInfo) {
  const int dropRem = topInfo.top.getDropRem();
  if (topInfo.topPiece.getBlockType() == BlockType::O_PIECE && dropRem > 0) {
    addNxFrame(topInfo, bfsInfo, topInfo.top.getDropRem());
    return;
  }
  /*else if (bfsInfo.rotateDirections.size() <= 1 && dropRem > 1) {
    addNxFrame(topInfo, bfsInfo, dropRem-1);
    return;
  }*/

  auto v = getReleasedSeenRef(topInfo, bfsInfo);
  if (v) return;
  if (dropRem == 0) v = 1;

  tappedSeenPtr(topInfo, bfsInfo) = true;

  if (dropRem == 0) {
    for (auto moveDirection: sidewaysMoveDirections) {
      if (topInfo.top.getMoveCooldown(moveDirection) != 0) continue;
      auto otherMd = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
      if (topInfo.top.getMoveCooldown(otherMd) == 2) continue;
      if (topInfo.topPiece.canMove(moveDirection)) {
        auto nxTapped = topInfo.top;
        nxTapped.setPiece(topInfo.topPiece.move(moveDirection));
        nxTapped.setFsmState(FSMState::TAPPED_ONCE);
        //nxTapped.setRotateCooldown(1);
        //nxTapped.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] = 1;
        onEnterTapped(nxTapped);
        auto [ignore, inserted] = bfsInfo.seen.emplace(nxTapped);
        if (!inserted) continue;
        addEdge(topInfo.top, nxTapped, moveDirection);
        bfsInfo.releasedQ.push({topInfo.topScore + 1 + nxTapped.frameEntered_ * SCORE_FRAME_ENTERED, nxTapped});
      }
    }
  }
  static std::array rotateRestrictions = {0, 1, 3};
  considerRotate(topInfo, bfsInfo, rotateRestrictions[bfsInfo.rotateDirections.size()]);
  if (considerMovingDown(topInfo, bfsInfo)) return;
  addNxFrame(topInfo, bfsInfo);
}

void MoveFinderFSM::runTapped(const TopInfo &topInfo, BfsInfo &bfsInfo) {
  if (bfsInfo.rotateDirections.size() <= 1 && topInfo.top.getDropRem() > 0) {
    addNxFrame(topInfo, bfsInfo, topInfo.top.getDropRem());
    return;
  }

  if (tappedSeenPtr(topInfo, bfsInfo)) return;
  if (bfsInfo.rotateDirections.size() <= 1 || topInfo.top.getDropRem() == 0) tappedSeenPtr(topInfo, bfsInfo) = true;

  // nothing to do here, lol.
  considerRotate(topInfo, bfsInfo);
  if (considerMovingDown(topInfo, bfsInfo)) return;
  addNxFrame(topInfo, bfsInfo);
}

void MoveFinderFSM::addEdge(const MoveFinderState &s1, const MoveFinderState &s2, Action action) {
  assert(pred_.count(s2) == 0);
  pred_.insert({s2, {s1, action}});
}


void MoveFinderFSM::addNxFrame(const TopInfo& topInfo, BfsInfo& bfsInfo) {
  addNxFrame(topInfo, bfsInfo, 1);
}

void MoveFinderFSM::addNxFrame(const TopInfo& topInfo, BfsInfo& bfsInfo, int frames) {
  auto nxFrame = topInfo.top;
  while (frames--) nxFrame.nextFrame();
  auto [ignore, inserted] = bfsInfo.seen.emplace(nxFrame);
  if (!inserted) return;
  addEdge(topInfo.top, nxFrame, Action::NONE);
  bfsInfo.qPush({topInfo.topScore, nxFrame});
}

void MoveFinderFSM::considerRotate(const TopInfo& topInfo, BfsInfo& bfsInfo, int rotateRestrictionDropRem) {
  if (topInfo.top.getDropRem() > rotateRestrictionDropRem) return;
  for (auto rotateDirection: bfsInfo.rotateDirections) {
    auto otherRd = rotateDirection == RotateDirection::ROTATE_C ? RotateDirection::ROTATE_AC : RotateDirection::ROTATE_C;
    if (topInfo.top.getRotateCooldown(otherRd) == 2) continue;
    if (topInfo.top.getRotateCooldown(rotateDirection) == 0 && topInfo.topPiece.canRotate(rotateDirection)) {
      auto nxRotate = topInfo.top;
      nxRotate.setPiece(topInfo.topPiece.rotate(rotateDirection));
      assert(nxRotate.getRotateCooldown(otherRd) <= 1);

      nxRotate.setRotateCooldown(otherRd, 1); // implcitly set to 1
      nxRotate.setRotateCooldown(rotateDirection, 2);
      nxRotate.setSidewaysMoveCooldown(1);
      auto [ignore, inserted] = bfsInfo.seen.emplace(nxRotate);
      if (!inserted) continue;
      addEdge(topInfo.top, nxRotate, rotateDirection);
      bfsInfo.qPush({topInfo.topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
    }
  }
}

// returns true if must move down
bool MoveFinderFSM::considerMovingDown(const TopInfo& topInfo, BfsInfo& bfsInfo) {
  // if drop rem is zero, you MUST move down
  if (topInfo.top.getDropRem() == 0) {
    if (topInfo.topPiece.canMove(MoveDirection::DOWN)) {
      auto nxMovedDown = topInfo.top;
      nxMovedDown.setPiece(topInfo.topPiece.move(MoveDirection::DOWN));
      nxMovedDown.setDropRem(maxDropRem_);
      nxMovedDown.setRotateCooldown(1);
      nxMovedDown.setSidewaysMoveCooldown(1);
      
      auto [ignore, inserted] = bfsInfo.seen.emplace(nxMovedDown);
      if (!inserted) return true;
      addEdge(topInfo.top, nxMovedDown, MoveDirection::DOWN);
      bfsInfo.qPush({topInfo.topScore, nxMovedDown});
      return true;
    } else {
      // a final state
      const auto &piece = topInfo.topPiece;
      if (finalMoveToState_.count(piece) == 0) {
        finalMoveToState_.insert({piece, {topInfo.top, topInfo.topScore}});
      } else {
        auto [oldState, oldScore] = finalMoveToState_.at(piece);
        if (topInfo.topScore < oldScore) {
          finalMoveToState_.insert({piece, {topInfo.top, topInfo.topScore}});
        }
      }
      assert(pred_.count(topInfo.top) == 1);
      bfsInfo.moves[topInfo.top.getPieceId()] = true;
      return true;
    }
  }
  return false;
};


//optimisations
void onEnterReleased(MoveFinderState &s) {
  s.setDasRem(0);
  s.setIsLeftHolding(false);
}

void onEnterTapped(MoveFinderState &s) {
  s.setMoveCooldown(0);
}


std::vector<std::pair<int, Action>> MoveFinderFSM::getShortestPathActions(const BitPieceInfo &piece) const {
  assert(finalMoveToState_.count(piece));
  auto [state, numMoves] = finalMoveToState_.at(piece);
  std::vector<std::pair<MoveFinderState, Action>> backwards;
  while (pred_.count(state)) {
    const auto &[nxState, action] = pred_.at(state);
    backwards.push_back({state, action});
    state = nxState;
  }
  auto forwards = backwards;
  std::reverse(forwards.begin(), forwards.end());

  std::vector<std::pair<int, Action>> result;
  for (auto i = 0; i < static_cast<int>(forwards.size()); ++i) {
    const auto &[state, action] = forwards[i];
    std::stringstream ss;
    if (action == Action::NONE) continue;
    result.push_back({state.frameEntered_, action});
  }
  return result;
}

std::vector<std::string> MoveFinderFSM::getShortestPath(const BitPieceInfo &piece) const {
  auto actionPairs = getShortestPathActions(piece);

  std::vector<std::string> result;
  for (const auto [frameEntered, action]: actionPairs) {
    std::stringstream ss;
    ss << frameEntered << ' ' << toString(action) << '\n';
    result.push_back(ss.str());
  }
  return result;
}
