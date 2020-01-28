#include "src/shared/MoveFinder/MoveFinderFSM.h"
#include "src/common/MoveDirection.hpp"
#include <queue>
#include <vector>
#include <sstream>

const int MAX_DAS_REM = 6;

void onEnterReleased(MoveFinderState &s);
void onEnterTapped(MoveFinderState &s);

template <typename T>
void safeInsert(std::unordered_set<MoveFinderState> &seen, const BitBoard &b, T& q, MoveFinderState state) {
  if (b.vacant(state.piece_)) {
    seen.insert(state);
    q.push({0, state});
  }
}

std::vector<BitPieceInfo> MoveFinderFSM::findAllMoves(const BitBoard& b, BlockType blockType) {
  pred_.clear();
  finalMoveToState_.clear();
  std::unordered_set<MoveFinderState> seen;
  using PairT = std::pair<int, MoveFinderState>;
  auto cmp = [](const PairT &l, const PairT &r) {
    return l.first >= r.first;
  };
  std::priority_queue<PairT, std::vector<PairT>, decltype(cmp)> q(cmp);

  std::set<BitPieceInfo> moves;

  auto s1 = MoveFinderState(b.getPiece(blockType), true, maxDropRem_);
  auto s2 = MoveFinderState(b.getPiece(blockType), false, maxDropRem_);
  s1.releaseCooldown_ = s2.releaseCooldown_ = 1;
  auto s3(s2);
  s3.fsmState_ = FSMState::RELEASED;

  if (!hasFirstMoveConstraint_) {
    safeInsert(seen, b, q, s1);
    safeInsert(seen, b, q, s2);
    safeInsert(seen, b, q, s3);
  } else {
    switch(firstMoveDirectionChar_) {
      case 'L': { safeInsert(seen, b, q, s1); } break;
      case 'R': { safeInsert(seen, b, q, s2); } break;
      case 'N': { safeInsert(seen, b, q, s3); } break;
      default: throw std::runtime_error("Unknown firstMoveDirectionChar_");
    }
  }

  while (!q.empty()) {
    const int SCORE_FRAME_ENTERED = 10000;
    const int SCORE_ROTATED = 100;

    const auto [topScore, top] = q.top(); q.pop();
    auto addNxFrame = [&, top=top, topScore=topScore]{
      auto nxFrame = top;
      nxFrame.nextFrame();
      if (seen.count(nxFrame)) return;
      seen.insert(nxFrame);
      addEdge(top, nxFrame, Action::NONE);
      q.push({topScore, nxFrame});
    };

    // if drop rem is zero, you MUST move down
    if (top.dropRem_ == 0) {
      if (top.piece_.canMove(MoveDirection::DOWN)) {
        auto nxMovedDown = top;
        nxMovedDown.piece_ = top.piece_.move(MoveDirection::DOWN);
        nxMovedDown.dropRem_ = maxDropRem_;
        nxMovedDown.setRotateCooldown(1);
        if (seen.count(nxMovedDown)) continue; seen.insert(nxMovedDown);
        addEdge(top, nxMovedDown, MoveDirection::DOWN);
        q.push({topScore, nxMovedDown});
        continue;
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
        continue;
      }
    }
  
    switch(top.fsmState_) {
      case FSMState::HOLDING: {
        // either keep holding or release.
        // you MUST move across if dasRem is zero when you're holding
        if (top.dasRem_ == 0) {
          // consider bringing in moveCooldownFromRotate_
          if (top.moveCooldown_ != 0) continue; // cant proceed
          //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
          auto moveDirection = top.isLeftHolding_ ? MoveDirection::LEFT : MoveDirection::RIGHT;
          if (top.piece_.canMove(moveDirection)) {
            auto nxMoved = top;
            nxMoved.piece_ = top.piece_.move(moveDirection);
            nxMoved.dasRem_ = MAX_DAS_REM;
            nxMoved.setRotateCooldown(1);
            if (seen.count(nxMoved)) continue;
            seen.insert(nxMoved);
            addEdge(top, nxMoved, moveDirection);
            q.push({topScore+1, nxMoved});
            continue;
          }
        }

        // releasing...
        if (top.releaseCooldown_ == 0) {
          auto nxReleased = top;
          nxReleased.fsmState_ = FSMState::RELEASED;
          nxReleased.moveCooldown_ = 1+1; // can eventually be 1
          onEnterReleased(nxReleased);
          if (!seen.count(nxReleased)) {
            seen.insert(nxReleased);
            addEdge(top, nxReleased, Action::NONE);
            q.push({topScore, nxReleased});
          }
        }
        addNxFrame();
      } break;
      case FSMState::RELEASED: {
        static auto tapMoveDirections = std::vector<MoveDirection>{MoveDirection::LEFT, MoveDirection::RIGHT};
        if (top.moveCooldown_ == 0) {
          for (auto moveDirection: tapMoveDirections) {
            if (top.piece_.canMove(moveDirection)) {
              auto nxTapped = top;
              nxTapped.piece_ = top.piece_.move(moveDirection);
              nxTapped.fsmState_ = FSMState::TAPPED_ONCE;
              nxTapped.setRotateCooldown(1);
              onEnterTapped(nxTapped);
              if (seen.count(nxTapped))
              continue; seen.insert(nxTapped);
              addEdge(top, nxTapped, moveDirection);
              q.push({topScore + 1 + nxTapped.frameEntered_ * SCORE_FRAME_ENTERED, nxTapped});
            }
          }
        }
        addNxFrame();
      } break;
      case FSMState::TAPPED_ONCE: {
        // nothing to do here, lol.
        addNxFrame();
      } break;
    }

    // rotations...
    for (auto rotateDirection: allRotateDirections) {
      if (top.rotateCooldown_[rotateDirection] == 0 && top.piece_.canRotate(rotateDirection)) {
        auto nxRotate = top;
        nxRotate.piece_ = top.piece_.rotate(rotateDirection);
        nxRotate.rotateCooldown_[rotateDirection] = 2;
        // move is processed before rotation. This means that any move after the rotation
        // this means any move after the rotation MUST be at least on the frame after
        nxRotate.moveCooldown_ = 1;
        if (seen.count(nxRotate)) continue; seen.insert(nxRotate);
        addEdge(top, nxRotate, rotateDirection);
        q.push({topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
      }
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
  s.dasRem_ = 0;
  s.isLeftHolding_ = false;
}

void onEnterTapped(MoveFinderState &s) {

}


std::vector<std::string> MoveFinderFSM::getShortestPath(const BitPieceInfo piece) const {
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

  std::vector<std::string> result;
  for (int i = 0; i < forwards.size(); ++i) {
    const auto &[state, action] = forwards[i];
    std::stringstream ss;
    if (action == Action::NONE) continue;
    ss << state.frameEntered_ << ' ' << toString(action) << '\n';
    result.push_back(ss.str());
  }
  return result;
}
