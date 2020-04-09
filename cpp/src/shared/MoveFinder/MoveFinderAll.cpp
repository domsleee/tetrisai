#include <array>
#include "src/shared/MoveFinder/MoveFinderAll.h"
#include <cassert>
#include <sstream>



template<bool includeDown>
std::vector<std::vector<Action>> getActions();
int actionsToBitfield(const std::vector<Action> &actions);
std::vector<Action> bitfieldToActions(int bitfield);

std::vector<BitPieceInfo> MoveFinderAll::findAllMoves(const BitBoard& board, BlockType blockType) {
  b_ = board;
  auto pieceInfo = board.getPiece(blockType);
  if (maxDropRem_ == 1) {
    auto [ok, pieceInfo2] = pieceInfo.doActionCopy(Action::DOWN);
    if (!ok) return {pieceInfo};
    addPred({pieceInfo, 0}, {pieceInfo2, 0});
    pieceInfo = pieceInfo2;
  }
  int startingMaxDropRem = std::max(0, maxDropRem_ - 2);
  MoveFinderAllNs::Node n(pieceInfo, 0);
  n.setDropRem(startingMaxDropRem);

  std::queue<MoveFinderAllNs::Node> q;
  q.push(n);
  while (!q.empty()) {
    auto front = q.front();
    q.pop();
    if (front.getDropRem() == 0) {
      for (auto actions: getActions<true>()) {
        applyTransition(q, front, actions);
      }
    } else {
      for (auto actions: getActions<false>()) {
        applyTransition(q, front, actions);
      }
    }
  }
  std::vector<BitPieceInfo> res;
  for (int i = 0; i < static_cast<int>(lastToNode_.size()); ++i) {
    if (lastToNode_[i] == -1) continue;
    res.push_back(board.getPieceFromId(i));
  }
  return res;
}


// rotateAC && rotateC && moveLeft && moveRight && moveDown
void MoveFinderAll::applyTransition(std::queue<MoveFinderAllNs::Node> &q, MoveFinderAllNs::Node n, const std::vector<Action> &actions) {
  auto orig = n;
  n.decDropRem(maxDropRem_);

  if (!actions.size()) {
    n.setCooldownBitField(0);
    addPred(orig, n);
    q.push(n);
    return;
  }
  bool hasDown = actions.back() == Action::DOWN;
  int actionsToConsider = hasDown ? actions.size()-1 : actions.size();
  std::vector<int> ids(actionsToConsider + 1);

  ids[0] = n.getId();
  for (int i = 0; i < actionsToConsider; ++i) {
    ids[i+1] = BitBoardPre::doActionOnEmptyBoard(ids[i], actions[i]);
    if (ids[i+1] == -1) return;
  }
  n.setCooldownBitField(actionsToBitfield(actions));
  n.setId(ids.back());

  if (std::any_of(ids.begin()+1, ids.end(), [&](const int id) { return !b_.vacant(id); })) return;

  if (hasDown) {
    auto lastId = BitBoardPre::doActionOnEmptyBoard(ids.back(), Action::DOWN);
    if (lastId == -1) {
      lastToNode_[n.getId()] = n.toInt();
      return;
    }
    n.setId(lastId);
  }

  if (isSeen(n)) return;
  addPred(orig, n);
  q.push(n);
}


std::vector<std::pair<int, Action>> MoveFinderAll::getShortestPathActions(const BitPieceInfo &piece) const {
  assert(lastToNode_[piece.getId()] != -1);
  int n = lastToNode_[piece.getId()];
  std::vector<std::pair<int, Action>> ret;
  int frame = 0;
  //printf("hi %d ==> %d\n", n, pred_[n]);
  while (pred_[n] != -1) {
    //printf("hi n: %d, pred_[n]: %d\n", n, pred_[n]);
    auto actions = bitfieldToActions(MoveFinderAllNs::Node(pred_[n]).getCooldownBitField());
    for (auto action: actions) {
      ret.push_back({frame, action});
    }
    frame++;
    n = pred_[n];
    
  }
  std::reverse(ret.begin(), ret.end());

  std::vector<std::pair<int, Action>> result;
  for (auto i = 0; i < static_cast<int>(ret.size()); ++i) {
    const auto &[state, action] = ret[i];
    std::stringstream ss;
    if (action == Action::NONE) continue;
    result.push_back({i+2, action});
  }
  return result;
}

std::vector<std::string> MoveFinderAll::getShortestPath(const BitPieceInfo &piece) const {
  auto actionPairs = getShortestPathActions(piece);

  std::vector<std::string> result;
  for (const auto [frameEntered, action]: actionPairs) {
    std::stringstream ss;
    ss << frameEntered << ' ' << toString(action) << '\n';
    result.push_back(ss.str());
  }
  return result;
}

template<bool includeDown>
std::vector<std::vector<Action>> getActions() {
  std::vector<std::vector<Action>> res;
  constexpr std::array choices1 = {Action::LEFT, Action::RIGHT, Action::NONE};
  constexpr std::array choices2 = {Action::ROTATE_C, Action::ROTATE_AC, Action::NONE};
  std::array choices3 = {Action::DOWN};
  if (!includeDown) choices3[0] = Action::NONE;
  for (auto a1: choices1) {
    for (auto a2: choices2) {
      for (auto a3: choices3) {
        std::vector<Action> n;
        for (auto a: {a1, a2, a3}) {
          if (a != Action::NONE) n.push_back(a);
        }
        if (std::find(res.begin(), res.end(), n) == res.end()) res.push_back(n);
      }
    }
  }
  return res;
}

int actionsToBitfield(const std::vector<Action> &actions) {
  int ret = 0;
  for (auto action: actions) {
    switch(action) {
      case Action::LEFT: ret |= 1; break;
      case Action::RIGHT: ret |= 1 << 1; break;
      case Action::ROTATE_C: ret |= 1 << 2; break;
      case Action::ROTATE_AC: ret |= 1 << 3; break;
      case Action::DOWN: break;
      default: throw std::runtime_error("what");
    }
  }
  return ret;
}

std::vector<Action> bitfieldToActions(int bitfield) {
  std::vector<Action> ret;
  if (bitfield & 1) ret.push_back(Action::LEFT);
  if (bitfield & 2) ret.push_back(Action::RIGHT);
  if (bitfield & 4) ret.push_back(Action::ROTATE_C);
  if (bitfield & 8) ret.push_back(Action::ROTATE_AC);
  if (bitfield & 16) ret.push_back(Action::DOWN);
  return ret;
}
