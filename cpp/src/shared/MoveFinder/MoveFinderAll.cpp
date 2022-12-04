#include <array>
#include "src/shared/MoveFinder/MoveFinderAll.h"
#include <cassert>
#include <sstream>



std::vector<BitPieceInfo> MoveFinderAll::findAllMoves(const BitBoard& board, BlockType blockType) {
  std::fill(pred_.begin(), pred_.end(), -1);
  std::fill(lastToNode_.begin(), lastToNode_.end(), -1);
  b_ = board;

  auto pieceInfo = board.getPiece(blockType);

  if (maxDropRem_ == 1) {
    auto [ok, pieceInfo2] = pieceInfo.doActionCopy(Action::DOWN);
    if (!ok) return {pieceInfo};
    addPred({pieceInfo, (1 << 4), 0}, {pieceInfo2, 0, 0});
    pieceInfo = pieceInfo2;
  }
  else {
    addPred({pieceInfo, 0, maxDropRem_}, {pieceInfo, 0, maxDropRem_-1});
  }

  int startingMaxDropRem = std::max(1, maxDropRem_ - 1);
  MoveFinderAllNs::Node n(pieceInfo, 0, startingMaxDropRem);

  std::queue<MoveFinderAllNs::Node> q;
  q.push(n);
  while (!q.empty()) {
    auto front = q.front();
    q.pop();
    bool hasDown = front.getDropRem() == 1 || maxDropRem_ == 0;
    for (const auto &actions: BitBoardPre::getActions(hasDown)) {
      applyTransition(q, front, actions);
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
void MoveFinderAll::applyTransition(std::queue<MoveFinderAllNs::Node> &q, MoveFinderAllNs::Node n, const std::pair<std::vector<Action>, int> &actionPairs) {
  auto orig = n;
  const auto &actions = actionPairs.first;
  int actionsBitfield = actionPairs.second;

  if (n.getCooldownBitField() & actionsBitfield) {
    //printf("field: %d, %d\n", n.getCooldownBitField(), actionsBitfield);
    return;
  }

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
  n.setCooldownBitField(actionsBitfield & 0b1111);
  n.setId(ids.back());

  if (std::any_of(ids.begin()+1, ids.end(), [&](const int id) { return !b_.vacant(id); })) return;

  if (hasDown) {
    auto lastId = BitBoardPre::doActionOnEmptyBoard(ids.back(), Action::DOWN);
    if (lastId == -1 || !b_.vacant(lastId)) {
      n.setDropRem(7);
      if (!isSeen(n)) {
        addPred(orig, n);
        lastToNode_[n.getId()] = n.toInt();
        //printf("ignore\n");
      }
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
  int numFrames = 0;
  //printf("hi %d ==> %d\n", n, pred_[n]);

  auto addActionsToRet = [&](const MoveFinderAllNs::Node &n) {
    auto dropRem = n.getDropRem();
    if (dropRem == maxDropRem_ || dropRem == 7) {
      ret.push_back({numFrames, Action::DOWN});
    }
    
    auto cooldownBitfield = n.getCooldownBitField();
    auto actions = BitBoardPre::bitfieldToActions(cooldownBitfield);
    if (std::find(actions.begin(), actions.end(), Action::DOWN) != actions.end()) {
      printf("WTF\n");
    }
    for (auto action: actions) {
      ret.push_back({numFrames, action});
    }
  };

  addActionsToRet(n);
  numFrames++;
  while (pred_[n] != -1) {
    //auto node = MoveFinderAllNs::Node(n);
    //printf("%d --> %d (id: %d, dropRem: %d, bf: %d)\n", n, pred_[n], node.getId(), node.getDropRem(), node.getCooldownBitField());
    n = pred_[n];
    addActionsToRet(MoveFinderAllNs::Node(n));
    if (pred_[n] != -1) numFrames++;
  }

  //printf("lastFrame: %d\n", lastFrame);
  std::reverse(ret.begin(), ret.end());

  std::vector<std::pair<int, Action>> result;
  for (auto i = 0; i < static_cast<int>(ret.size()); ++i) {
    const auto &[backFrame, action] = ret[i];
    std::stringstream ss;
    if (action == Action::NONE) continue;
    const auto forwardFrame = numFrames-backFrame;
    if (forwardFrame == 0) continue;
    //printf("forwardFrame: %d\n", forwardFrame);
    result.push_back({forwardFrame, action});
  }
  return result;
}

std::vector<std::string> MoveFinderAll::getShortestPath(const BitPieceInfo &piece) const {
  auto actionPairs = getShortestPathActions(piece);

  std::vector<std::string> result;
  for (const auto &[frameEntered, action]: actionPairs) {
    std::stringstream ss;
    ss << frameEntered << ' ' << toString(action) << '\n';
    result.push_back(ss.str());
  }
  return result;
}

