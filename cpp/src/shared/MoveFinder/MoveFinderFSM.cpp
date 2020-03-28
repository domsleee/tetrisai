#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/common/MoveDirection.hpp"
#include "src/shared/MoveFinder/MoveFinderConstraints.h"
#include <queue>
#include <vector>
#include <sstream>

#define SEEN_EMPLACE(seen, v) seen.emplace(v);
const int MAX_DAS_REM = 6;

struct BfsInfo {
  FSMTypes::SeenT seen;
  FSMTypes::MovesT moves = FSMTypes::MovesT(BitBoardPre::NUM_INDEXES, false);
  BfsInfo() {
    seen.reserve(1e5);
  }
  FSMTypes::PairT qTop() {
#if MOVE_FINDER_FSM_PERFORMANCE == 1
    return q.front();
#else
    return q.top();
#endif
  }
  void qPop() { q.pop(); }
  void qPush(const FSMTypes::PairT &v) {
    q.push(v);
  }
  bool qEmpty() {
    return q.empty();
  }
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

void onEnterReleased(MoveFinderState &s);
void onEnterTapped(MoveFinderState &s);

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
#if RECORD_MOVEFINDER_EDGES == 0
  return findAllMovesConst(b, blockType);
#endif
  pred_.clear();
  finalMoveToState_.clear();
  BfsInfo bfsInfo;

  auto s1 = MoveFinderState(b.getPiece(blockType), true, maxDropRem_);
  auto s2 = MoveFinderState(b.getPiece(blockType), false, maxDropRem_);
  for (auto s: {s1, s2}) {
    s.setReleaseCooldown(2);
    s.setDasRem(2);
    s.setRotateCooldown(2);
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

  auto rotateDirections = getRotateDirections(b.getPiece(blockType));
  while (!bfsInfo.qEmpty()) {
    const auto [topScore, top] = bfsInfo.qTop();
    bfsInfo.qPop();
    TopInfo topInfo(topScore, top, b);
    auto topPiece = topInfo.topPiece;
  
    switch(top.getFsmState()) {
      case FSMState::HOLDING: {
        // either keep holding or release.
        // you MUST move across if dasRem is zero when you're holding
        auto moveDirection = top.getIsLeftHolding() ? MoveDirection::LEFT : MoveDirection::RIGHT;
        auto otherMoveDirection = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
        if (top.getDasRem() == 0) {
          //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
          if (top.getMoveCooldown(moveDirection) != 0) continue; // cant proceed
          if (topPiece.canMove(moveDirection)) {
            auto nxMoved = top;
            nxMoved.setPiece(topPiece.move(moveDirection));
            nxMoved.setDasRem(MAX_DAS_REM - (nxMoved.frameEntered_ == 2));
            //nxMoved.setRotateCooldown(1);
            auto [ignore, inserted] = bfsInfo.seen.emplace(nxMoved);
            if (inserted == false) continue;
            addEdge(top, nxMoved, moveDirection);
            bfsInfo.qPush({topScore+1, nxMoved});
            continue;
          }
        }

        // releasing...
        if (top.getReleaseCooldown() == 0) {
          auto nxReleased = top;
          nxReleased.setFsmState(FSMState::RELEASED);
          nxReleased.setMoveCooldown(moveDirection, 1+1);
          nxReleased.setMoveCooldown(otherMoveDirection, 1); // must come after
          onEnterReleased(nxReleased);
          auto [ignore, inserted] = bfsInfo.seen.emplace(nxReleased);
          if (inserted) {
            addEdge(top, nxReleased, Action::NONE);
            bfsInfo.qPush({topScore, nxReleased});
          }
        }
        considerRotate(topInfo, bfsInfo, rotateDirections);
        if (considerMovingDown(topInfo, bfsInfo)) break;
        addNxFrame(topInfo, bfsInfo);
      } break;
      case FSMState::RELEASED: {
        for (auto moveDirection: sidewaysMoveDirections) {
          if (top.getMoveCooldown(moveDirection) != 0) continue;
          if (topPiece.canMove(moveDirection)) {
            auto nxTapped = top;
            nxTapped.setPiece(topPiece.move(moveDirection));
            nxTapped.setFsmState(FSMState::TAPPED_ONCE);
            //nxTapped.setRotateCooldown(1);
            //nxTapped.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] = 1;
            onEnterTapped(nxTapped);
            auto [ignore, inserted] = bfsInfo.seen.emplace(nxTapped);
            if (!inserted) continue;
            addEdge(top, nxTapped, moveDirection);
            bfsInfo.qPush({topScore + 1 + nxTapped.frameEntered_ * SCORE_FRAME_ENTERED, nxTapped});
          }
        }
        considerRotate(topInfo, bfsInfo, rotateDirections);
        if (considerMovingDown(topInfo, bfsInfo)) break;
        addNxFrame(topInfo, bfsInfo);
      } break;
      case FSMState::TAPPED_ONCE: {
        // nothing to do here, lol.
        considerRotate(topInfo, bfsInfo, rotateDirections);
        if (considerMovingDown(topInfo, bfsInfo)) break;
        addNxFrame(topInfo, bfsInfo);
      } break;
    }
    
  }

  std::vector<BitPieceInfo> ret;
  for (int i = 0; i < BitBoardPre::NUM_INDEXES; ++i) {
    if (bfsInfo.moves[i]) ret.push_back(b.getPieceFromId(i));
  }
  return ret;
}

void MoveFinderFSM::addEdge(const MoveFinderState &s1, const MoveFinderState &s2, Action action) {
  assert(pred_.count(s2) == 0);
  pred_.insert({s2, {s1, action}});
}


void MoveFinderFSM::addNxFrame(const TopInfo& topInfo, BfsInfo& bfsInfo) {
  auto nxFrame = topInfo.top;
  nxFrame.nextFrame();
  auto [ignore, inserted] = bfsInfo.seen.emplace(nxFrame);
  if (!inserted) return;
  addEdge(topInfo.top, nxFrame, Action::NONE);
  bfsInfo.qPush({topInfo.topScore, nxFrame});
}

void MoveFinderFSM::considerRotate(const TopInfo& topInfo, BfsInfo& bfsInfo, const std::vector<RotateDirection>& rotateDirections) {
  for (auto rotateDirection: rotateDirections) {
    if (topInfo.top.getRotateCooldown(rotateDirection) == 0 && topInfo.topPiece.canRotate(rotateDirection)) {
      auto nxRotate = topInfo.top;
      nxRotate.setPiece(topInfo.topPiece.rotate(rotateDirection));
      nxRotate.setRotateCooldown(1);
      nxRotate.setRotateCooldown(rotateDirection, 2);
      nxRotate.setSidewaysMoveCooldown(1);
      auto [ignore, inserted] = bfsInfo.seen.emplace(nxRotate);
      if (!inserted) continue;
      addEdge(topInfo.top, nxRotate, rotateDirection);
      bfsInfo.qPush({topInfo.topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
    }
  }
}

bool MoveFinderFSM::considerMovingDown(const TopInfo& topInfo, BfsInfo& bfsInfo) {
  // if drop rem is zero, you MUST move down
  if (topInfo.top.getDropRem() == 0) {
    if (topInfo.top.getMoveCooldown(MoveDirection::DOWN) != 0) return true; // cant proceed
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
