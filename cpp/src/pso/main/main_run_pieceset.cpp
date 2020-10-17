#include <iostream>
#include <stdlib.h>
#include <string>
#include "src/shared/Config.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/pso/SimpleApi.tpp"
#include "src/pso/summary/SummaryApi.h"
#include "src/pso/IPieceSetGetter.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

const std::string moveEvaluatorGroup = MOVE_EVALUATOR_GROUP_BOTH_LINEAR;

struct PieceSetReadResult {
  std::vector<BlockType> pieces;
  int score;
};

PieceSetReadResult readFile(const std::string &filename);

struct MyPieceSetGetter: public IPieceSetGetter {
  MyPieceSetGetter(const PieceSetReadResult &pieceSetReadResult): pieceSetReadResult_{pieceSetReadResult} {}
  void setSeed(int seed) override {}
  std::vector<std::vector<BlockType>> getPieceSets(int numGames) const override {
    return std::vector<std::vector<BlockType>>(numGames, pieceSetReadResult_.pieces);
  }
 private:
  const PieceSetReadResult pieceSetReadResult_;
};


int main(int argc, char ** argv) {
  SummaryApi s;
  const std::string filename = "LinearN_WellLocationsNotBoth.pieces";
  auto summary1 = s.readLogFile("18_LinearN.log");
  auto summary2 = s.readLogFile("19_WellLocationsNotBoth.log");
  auto me1 = getMoveEvaluatorGroups().at(summary1.group).setWeights(summary1.weights);
  auto me2 = getMoveEvaluatorGroups().at(summary2.group).setWeights(summary2.weights);
  auto meMfPairProvider = getMeMfPairProvider<MoveFinderFSM>(me1, me2, 90);
  auto pieceSetReadResult = readFile(filename);

  auto ew = NewEvaluateWeightingsFactory<MoveFinderFSM>::getInstance(
    meMfPairProvider,
    std::make_shared<MyPieceSetGetter>(pieceSetReadResult)
  );
  // summary1.config.applyConfig(ew)
  ew.setNumGames(1);
  ew.setLookahead(1);
  auto sms = ew.getSortedScoreManagers();
  auto expScore = pieceSetReadResult.score;
  auto actScore = sms[0].getScore();

  if (actScore != expScore) {
    printf("expected: %d, actual: %d\n", expScore, actScore);
    printf("failure.\n");
    exit(1);
  } else {
    printf("ok.\n");
  }
}

PieceSetReadResult readFile(const std::string &filename) {
  const std::string file = WORKSPACE_DIR + std::string("/src/pso/main/data/" + filename);
  std::ifstream fin{file};
  std::string line;
  int score = -1, numPieces = -1;
  std::vector<BlockType> pieces;
  for (int lineNumber = 0; std::getline(fin, line); lineNumber++) {
    if (lineNumber <= 1) {
      std::vector<std::string> result;
      boost::split(result, line, boost::is_any_of(":"));
      for (auto str: result) boost::trim(str);
      if (lineNumber == 0) score = std::stoi(result[1]);
      else if (lineNumber == 1) numPieces = std::stoi(result[1]);
      continue;
    }
    pieces.push_back(fromString(line));
  }
  return {pieces, score};
}


