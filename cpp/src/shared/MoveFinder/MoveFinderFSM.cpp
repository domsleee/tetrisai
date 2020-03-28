#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/common/MoveDirection.hpp"
#include "src/shared/MoveFinder/MoveFinderConstraints.h"
#include <queue>
#include <vector>
#include <sstream>

const int MAX_DAS_REM = 6;

void onEnterReleased(MoveFinderState &s);
void onEnterTapped(MoveFinderState &s);

template <typename T>
void safeInsert(std::unordered_set<MoveFinderState> &seen, const BitBoard &b, T& q, MoveFinderState state) {
  if (b.vacant(state.piece_)) {
    seen.emplace(state);
    q.push({0, state});
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
  std::unordered_set<MoveFinderState> seen;
  using PairT = std::pair<int, MoveFinderState>;
  auto cmp = [](const PairT &l, const PairT &r) {
    return l.first >= r.first;
  };
  
#if MOVE_FINDER_FSM_PERFORMANCE == 1
  std::queue<PairT> q;
#else
  std::priority_queue<PairT, std::vector<PairT>, decltype(cmp)> q(cmp);
#endif
  

  std::set<BitPieceInfo> moves;

  auto s1 = MoveFinderState(b.getPiece(blockType), true, maxDropRem_);
  auto s2 = MoveFinderState(b.getPiece(blockType), false, maxDropRem_);
  s1.setReleaseCooldown(2);
  s1.setDasRem(2);
  s1.setRotateCooldown(2);
  s2.setDasRem(2);
  s2.setReleaseCooldown(2);
  s2.setRotateCooldown(2);
  auto s3(s2);
  onEnterReleased(s3);
  s3.setFsmState(FSMState::RELEASED);
  s3.setRotateCooldown(2);
  s3.setSidewaysMoveCooldown(2);

  if (!hasFirstMoveConstraint_) {
    safeInsert(seen, b, q, s1);
    safeInsert(seen, b, q, s2);
    safeInsert(seen, b, q, s3);
  } else {
    switch(firstMoveDirectionChar_) {
      case FIRST_MOVE_DIRECTION_LEFT: { safeInsert(seen, b, q, s1); } break;
      case FIRST_MOVE_DIRECTION_RIGHT: { safeInsert(seen, b, q, s2); } break;
      case FIRST_MOVE_DIRECTION_NEITHER: { safeInsert(seen, b, q, s3); } break;
      default: throw std::runtime_error("Unknown firstMoveDirectionChar_");
    }
  }

  auto rotateDirections = getRotateDirections(b.getPiece(blockType));

  while (!q.empty()) {
    const int SCORE_FRAME_ENTERED = 10000;
    const int SCORE_ROTATED = 100;

#if MOVE_FINDER_FSM_PERFORMANCE == 1
    const auto [topScore, top] = q.front(); q.pop();
#else
    const auto [topScore, top] = q.top(); q.pop();
#endif
    auto addNxFrame = [&, top=top, topScore=topScore]{
      auto nxFrame = top;
      nxFrame.nextFrame();
      auto [ignore, inserted] = seen.emplace(nxFrame);
      if (!inserted) return;
      addEdge(top, nxFrame, Action::NONE);
      q.push({topScore, nxFrame});
    };

    auto considerRotate = [&, top=top, topScore=topScore]() {
      for (auto rotateDirection: rotateDirections) {
        if (top.getRotateCooldown(rotateDirection) == 0 && top.piece_.canRotate(rotateDirection)) {
          auto nxRotate = top;
          nxRotate.piece_ = top.piece_.rotate(rotateDirection);
          nxRotate.setRotateCooldown(1);
          nxRotate.setRotateCooldown(rotateDirection, 2);
          nxRotate.setSidewaysMoveCooldown(1);
          auto [ignore, inserted] = seen.emplace(nxRotate);
          if (!inserted) continue;
          addEdge(top, nxRotate, rotateDirection);
          q.push({topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
        }
      }
    };

    auto considerMovingDown = [&, top=top, topScore=topScore]() -> bool {
      // if drop rem is zero, you MUST move down
      if (top.getDropRem() == 0) {
        if (top.getMoveCooldown(MoveDirection::DOWN) != 0) return true; // cant proceed
        if (top.piece_.canMove(MoveDirection::DOWN)) {
          auto nxMovedDown = top;
          nxMovedDown.piece_ = top.piece_.move(MoveDirection::DOWN);
          nxMovedDown.setDropRem(maxDropRem_);
          nxMovedDown.setRotateCooldown(1);
          nxMovedDown.setSidewaysMoveCooldown(1);
          
          auto [ignore, inserted] = seen.emplace(nxMovedDown);
          if (!inserted) return true;
          addEdge(top, nxMovedDown, MoveDirection::DOWN);
          q.push({topScore, nxMovedDown});
          return true;
        } else {
          // a final state
          const auto &piece = top.piece_;
          if (finalMoveToState_.count(piece) == 0) {
            finalMoveToState_.insert({piece, {top, topScore}});
          } else {
            auto [oldState, oldScore] = finalMoveToState_.at(piece);
            if (topScore < oldScore) {
              finalMoveToState_.insert({piece, {top, topScore}});
            }
          }
          assert(pred_.count(top) == 1);
          moves.insert(top.piece_);
          return true;
        }
      }
      return false;
    };

  
    switch(top.getFsmState()) {
      case FSMState::HOLDING: {
        // either keep holding or release.
        // you MUST move across if dasRem is zero when you're holding
        auto moveDirection = top.getIsLeftHolding() ? MoveDirection::LEFT : MoveDirection::RIGHT;
        auto otherMoveDirection = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
        if (top.getDasRem() == 0) {
          //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
          if (top.getMoveCooldown(moveDirection) != 0) continue; // cant proceed
          if (top.piece_.canMove(moveDirection)) {
            auto nxMoved = top;
            nxMoved.piece_ = top.piece_.move(moveDirection);
            nxMoved.setDasRem(MAX_DAS_REM - (nxMoved.frameEntered_ == 2));
            //nxMoved.setRotateCooldown(1);
            auto [ignore, inserted] = seen.emplace(nxMoved);
            if (inserted == false) continue;
            addEdge(top, nxMoved, moveDirection);
            q.push({topScore+1, nxMoved});
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
          auto [ignore, inserted] = seen.emplace(nxReleased);
          if (inserted) {
            addEdge(top, nxReleased, Action::NONE);
            q.push({topScore, nxReleased});
          }
        }
        considerRotate();
        if (considerMovingDown()) break;
        addNxFrame();
      } break;
      case FSMState::RELEASED: {
        for (auto moveDirection: sidewaysMoveDirections) {
          if (top.getMoveCooldown(moveDirection) != 0) continue;
          if (top.piece_.canMove(moveDirection)) {
            auto nxTapped = top;
            nxTapped.piece_ = top.piece_.move(moveDirection);
            nxTapped.setFsmState(FSMState::TAPPED_ONCE);
            //nxTapped.setRotateCooldown(1);
            //nxTapped.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] = 1;
            onEnterTapped(nxTapped);
            auto [ignore, inserted] = seen.emplace(nxTapped);
            if (!inserted) continue;
            addEdge(top, nxTapped, moveDirection);
            q.push({topScore + 1 + nxTapped.frameEntered_ * SCORE_FRAME_ENTERED, nxTapped});
          }
        }
        considerRotate();
        if (considerMovingDown()) break;
        addNxFrame();
      } break;
      case FSMState::TAPPED_ONCE: {
        // nothing to do here, lol.
        considerRotate();
        if (considerMovingDown()) break;
        addNxFrame();
      } break;
    }
    
  }

  return {moves.begin(), moves.end()};  
}

void MoveFinderFSM::addEdge(const MoveFinderState &s1, const MoveFinderState &s2, Action action) {
  assert(pred_.count(s2) == 0);
  pred_.insert({s2, {s1, action}});
}


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
