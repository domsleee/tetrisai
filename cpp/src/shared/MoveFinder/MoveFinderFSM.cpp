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
  s1.releaseCooldown_ = s2.releaseCooldown_ = 2;
  s1.dasRem_ = s2.dasRem_ = 2;
  s1.setRotateCooldown(2);
  s2.setRotateCooldown(2);
  auto s3(s2);
  onEnterReleased(s3);
  s3.fsmState_ = FSMState::RELEASED;
  s3.setRotateCooldown(2);
  s3.setSidewaysMoveCooldown(2);

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
      if (seen.count(nxFrame)) return;
      seen.insert(nxFrame);
      addEdge(top, nxFrame, Action::NONE);
      q.push({topScore, nxFrame});
    };

    auto considerRotate = [&]() {
      for (auto rotateDirection: rotateDirections) {
        if (top.rotateCooldown_[rotateDirection] == 0 && top.piece_.canRotate(rotateDirection)) {
          auto nxRotate = top;
          nxRotate.piece_ = top.piece_.rotate(rotateDirection);
          nxRotate.setRotateCooldown(1);
          nxRotate.rotateCooldown_[rotateDirection] = 2;
          nxRotate.setSidewaysMoveCooldown(1);
          if (seen.count(nxRotate)) continue;
          seen.insert(nxRotate);
          addEdge(top, nxRotate, rotateDirection);
          q.push({topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
        }
      }
    };

    auto considerMovingDown = [&]() -> bool {
      // if drop rem is zero, you MUST move down
      if (top.dropRem_ == 0) {
        if (top.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] != 0) return true; // cant proceed
        if (top.piece_.canMove(MoveDirection::DOWN)) {
          auto nxMovedDown = top;
          nxMovedDown.piece_ = top.piece_.move(MoveDirection::DOWN);
          nxMovedDown.dropRem_ = maxDropRem_;
          nxMovedDown.setRotateCooldown(1);
          nxMovedDown.setSidewaysMoveCooldown(1);
          
          if (seen.count(nxMovedDown)) return true;
          seen.insert(nxMovedDown);
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

  
    switch(top.fsmState_) {
      case FSMState::HOLDING: {
        // either keep holding or release.
        // you MUST move across if dasRem is zero when you're holding
        auto moveDirection = top.isLeftHolding_ ? MoveDirection::LEFT : MoveDirection::RIGHT;
        auto otherMoveDirection = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
        if (top.dasRem_ == 0) {
          //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
          if (top.moveCooldown_[static_cast<int>(moveDirection)] != 0) continue; // cant proceed
          if (top.piece_.canMove(moveDirection)) {
            auto nxMoved = top;
            nxMoved.piece_ = top.piece_.move(moveDirection);
            nxMoved.dasRem_ = MAX_DAS_REM - (nxMoved.frameEntered_ == 2);
            //nxMoved.setRotateCooldown(1);
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
          nxReleased.moveCooldown_[static_cast<int>(moveDirection)] = 1+1;
          nxReleased.moveCooldown_[static_cast<int>(otherMoveDirection)] = 1; // must come after
          onEnterReleased(nxReleased);
          if (!seen.count(nxReleased)) {
            seen.insert(nxReleased);
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
          if (top.moveCooldown_[static_cast<int>(moveDirection)] != 0) continue;
          if (top.piece_.canMove(moveDirection)) {
            auto nxTapped = top;
            nxTapped.piece_ = top.piece_.move(moveDirection);
            nxTapped.fsmState_ = FSMState::TAPPED_ONCE;
            //nxTapped.setRotateCooldown(1);
            //nxTapped.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] = 1;
            onEnterTapped(nxTapped);
            if (seen.count(nxTapped)) continue;
            seen.insert(nxTapped);
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


std::vector<BitPieceInfo> MoveFinderFSM::findAllMovesConst(const BitBoard& b, BlockType blockType) const {
  std::unordered_set<MoveFinderState> seen;
  using PairT = std::pair<int, MoveFinderState>;
  std::queue<PairT> q;

  std::set<BitPieceInfo> moves;

  auto s1 = MoveFinderState(b.getPiece(blockType), true, maxDropRem_);
  auto s2 = MoveFinderState(b.getPiece(blockType), false, maxDropRem_);
  s1.releaseCooldown_ = s2.releaseCooldown_ = 5;
  auto s3(s2);
  onEnterReleased(s3);
  s3.fsmState_ = FSMState::RELEASED;
  s3.releaseCooldown_ = 0;

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

  auto rotateDirections = getRotateDirections(b.getPiece(blockType));

  while (!q.empty()) {
    const int SCORE_FRAME_ENTERED = 10000;
    const int SCORE_ROTATED = 100;

    const auto [topScore, top] = q.front(); q.pop();
    auto addNxFrame = [&, top=top, topScore=topScore]{
      auto nxFrame = top;
      nxFrame.nextFrame();
      if (seen.count(nxFrame)) return;
      seen.insert(nxFrame);
      q.push({topScore, nxFrame});
    };

    auto considerRotate = [&]() {
      for (auto rotateDirection: rotateDirections) {
        if (top.rotateCooldown_[rotateDirection] == 0 && top.piece_.canRotate(rotateDirection)) {
          auto nxRotate = top;
          nxRotate.piece_ = top.piece_.rotate(rotateDirection);
          nxRotate.rotateCooldown_[rotateDirection] = 2;
          // move is processed before rotation.
          // this means any move after the rotation MUST be at least on the frame after
          nxRotate.setMoveCooldown(1);
          if (seen.count(nxRotate)) continue;
          seen.insert(nxRotate);
          q.push({topScore+1 + SCORE_ROTATED * nxRotate.frameEntered_, nxRotate});
        }
      }
    };

    auto considerMovingDown = [&]() -> bool {
      // if drop rem is zero, you MUST move down
      if (top.dropRem_ == 0) {
        if (top.moveCooldown_[static_cast<int>(MoveDirection::DOWN)] != 0) return true; // cant proceed
        if (top.piece_.canMove(MoveDirection::DOWN)) {
          auto nxMovedDown = top;
          nxMovedDown.piece_ = top.piece_.move(MoveDirection::DOWN);
          nxMovedDown.dropRem_ = maxDropRem_;
          nxMovedDown.setMoveCooldown(1);
          nxMovedDown.setRotateCooldown(1);
          if (seen.count(nxMovedDown)) return true;
          seen.insert(nxMovedDown);
          q.push({topScore, nxMovedDown});
          return true;
        } else {
          // a final state
          const auto &piece = top.piece_;
          moves.insert(top.piece_);
          return true;
        }
      }
      return false;
    };

  
    switch(top.fsmState_) {
      case FSMState::HOLDING: {
        // either keep holding or release.
        // you MUST move across if dasRem is zero when you're holding
        auto moveDirection = top.isLeftHolding_ ? MoveDirection::LEFT : MoveDirection::RIGHT;
        auto otherMoveDirection = moveDirection == MoveDirection::LEFT ? MoveDirection::RIGHT : MoveDirection::LEFT;
        if (top.dasRem_ == 0) {
          //printf("dasRem_ 0 on frame: %d\n", top.frameEntered_);
          if (top.moveCooldown_[static_cast<int>(moveDirection)] != 0) continue; // cant proceed
          if (top.piece_.canMove(moveDirection)) {
            auto nxMoved = top;
            nxMoved.piece_ = top.piece_.move(moveDirection);
            nxMoved.dasRem_ = MAX_DAS_REM;
            if (seen.count(nxMoved)) continue;
            seen.insert(nxMoved);
            q.push({topScore+1, nxMoved});
            continue;
          }
        }

        // releasing...
        if (top.releaseCooldown_ == 0) {
          auto nxReleased = top;
          nxReleased.fsmState_ = FSMState::RELEASED;
          nxReleased.moveCooldown_[static_cast<int>(moveDirection)] = 1+1;
          nxReleased.moveCooldown_[static_cast<int>(otherMoveDirection)] = 1; // must come after
          onEnterReleased(nxReleased);
          if (!seen.count(nxReleased)) {
            seen.insert(nxReleased);
            q.push({topScore, nxReleased});
          }
        }
        considerRotate();
        if (considerMovingDown()) break;
        addNxFrame();
      } break;
      case FSMState::RELEASED: {
        for (auto moveDirection: sidewaysMoveDirections) {
          if (top.moveCooldown_[static_cast<int>(moveDirection)] != 0) continue;
          if (top.piece_.canMove(moveDirection)) {
            auto nxTapped = top;
            nxTapped.piece_ = top.piece_.move(moveDirection);
            nxTapped.fsmState_ = FSMState::TAPPED_ONCE;
            onEnterTapped(nxTapped);
            if (seen.count(nxTapped)) continue;
            seen.insert(nxTapped);
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
