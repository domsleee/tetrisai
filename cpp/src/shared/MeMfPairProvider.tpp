#include "src/shared/MeMfPairProvider.h"

template <typename MyMoveFinder>
std::pair<MoveEvaluatorGroup, MyMoveFinder> MeMfPairProvider<MyMoveFinder>::getMeMfPair(int numLines) const {
  MoveEvaluatorGroup* me = NULL;
  MyMoveFinder* mf = NULL;

  for (auto it = mePairs_.begin(); it != mePairs_.end(); ++it) {
    if (numLines < it->first) {
      me = &(*it->second);
    }
  }

  for (auto it = mfPairs_.begin(); it != mfPairs_.end(); ++it) {
    if (numLines < it->first) {
      mf = &(*it->second);
    }
  }

  assert(me != NULL && mf != NULL);
  return {*me, *mf};
  /*
  for (auto it = pairs_.begin(); it != pairs_.end(); ++it) {
    if (numLines < it->first) {
      return it->second;
    }
  }
  throw std::runtime_error("did not have a handler for this amount of lines");*/
}
