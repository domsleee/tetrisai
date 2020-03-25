#pragma once
#include "src/shared/MoveEvaluator/MoveEvaluatorGroup.hpp"
#include <memory>
#include <vector>

template <typename MyMoveFinder>
class MeMfPairProvider {
  using MeMfPair = std::pair<int, std::pair<MoveEvaluatorGroup*, MyMoveFinder*>>;
  using MePair = std::pair<int, std::shared_ptr<MoveEvaluatorGroup>>;
  using MfPair = std::pair<int, std::shared_ptr<MyMoveFinder>>;
 public:
 MeMfPairProvider() {};
  MeMfPairProvider(std::vector<MePair> mePairs, std::vector<MfPair> mfPairs):
    mePairs_{mePairs},
    mfPairs_{mfPairs}
    {};
  MeMfPairProvider(const MeMfPairProvider<MyMoveFinder> &meMfPairProvider): 
    mePairs_{meMfPairProvider.mePairs_},
    mfPairs_{meMfPairProvider.mfPairs_},
    meMfPairs_{meMfPairProvider.meMfPairs_}
    {};
  std::pair<MoveEvaluatorGroup, MyMoveFinder> getMeMfPair(int numLines) const;
 private:
  std::vector<MePair> mePairs_;
  std::vector<MfPair> mfPairs_;
  std::vector<MeMfPair> meMfPairs_; // for optimsation, if required
};

#include "src/shared/MeMfPairProvider.tpp"