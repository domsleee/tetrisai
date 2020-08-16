#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "src/shared/Config.hpp"
#include "src/shared/MoveEvaluator/MoveEvaluatorGroups.hpp"
#include "src/pso/SimpleApi.tpp"
#include "src/pso/summary/SummaryApi.h"
#include "src/pso/IPieceSetGetter.h"
#include <fstream>
#include <stack>
#include "src/shared/MoveFinder/AllMoveFinder.tpp"

#include "src/board/BoardPrinter.tpp"
#include "src/board/bitboard/BitBoard.h"

#define logf(...) { printf("%s: ", currentDateTime().c_str()); printf(__VA_ARGS__); }

void testing();
void unclearableLines();

// 31574 / (2^20) = ~3%
// 13463746 / (2^30) = ~1.25% (2mins)


struct Unclearable {
  AllMoveFinder<BitBoard, BitPieceInfo> mf;
  const int amount_ = 6;

  Unclearable(int amount): amount_{amount} {
    mf.setOffsetHeight(2);
  }

  std::unordered_set<BitBoard> getUnclearableBoards(const std::vector<BitBoard> &boards) {
    std::unordered_set<BitBoard> unclearableBoards;
    for (auto b: boards) {
      int i = 0;
      for (int j = 0; j < NUM_COLUMNS; ++j) i |= (!b.vacant(Coord(NUM_ROWS-1, j))) << j;
      if (isUnclearable(b, i)) {
        unclearableBoards.insert(b);
      }
    }
    return unclearableBoards;
  }

  std::unordered_set<BitBoard> getUnclearableBoards() {
    std::unordered_set<BitBoard> unclearableBoards;
    if (amount_ > 2) {
      auto c = Unclearable(amount_-1);
      auto boards = c.getUnclearableBoards();
      checkedBoard_ = c.checkedBoard_;
      pred_ = c.pred_;
      leadsToClearable_ = c.leadsToClearable_;

      printf("needed to check: %lu with %d moves\n", boards.size(), amount_);
      std::vector<BitBoard> boardsV = {boards.begin(), boards.end()};
      return getUnclearableBoards(boardsV);
      
    } else {
      auto vs = std::vector(NUM_ROWS, std::vector(NUM_COLUMNS, 0));
      // note: 10 (0b1010) is unsolvable
      for (int i = 1018; i < 1019; ++i) {
        for (auto j = 0; j < NUM_COLUMNS; ++j) vs[NUM_ROWS-1][j] = (i & (1 << (NUM_COLUMNS-j-1))) > 0;
        BitBoard initB(vs);
        if (isUnclearable(initB, i)) {
          unclearableBoards.insert(initB);
        }
      }
    }

    for (auto b: unclearableBoards) {
      std::cout << b << '\n';
    }
    printf("unclearable boards: %lu\n", unclearableBoards.size());
    return unclearableBoards;
  }

  bool isUnclearable(const BitBoard &initB, int i) {
    if (initB.toString() == "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001010") {
      return true;
    }

    bool cleared = false;
    std::queue<std::pair<int, BitBoard>> q;
    std::unordered_set<BitBoard> seenBoards;

    seenBoards.insert(initB);
    q.push({0, initB});
    printBoard(initB);
    while (!q.empty()) {
      const auto [amount, b] = q.front(); q.pop();
      if (amount >= amount_) break;
      for (auto blockType: allBlockTypes) {
        for (auto piece: mf.findAllMoves(b, blockType)) {
          auto [b2, maxRow] = b.applyPieceInfoMaxRowCopy(piece);
          if (maxRow == NUM_ROWS-1) {
            printBoard(b);
            printBoardWithPiece(b, piece);
            cleared = true;
            break;
          }
          if (!seenBoards.count(b2)) {
            seenBoards.insert(b2);
            q.push({amount+1, b2});
          }
        }
        if (cleared) break;
      }
      if (cleared) break;
    }
    return !cleared;
  }

  std::unordered_set<BitBoard> leadsToClearable_;
  std::unordered_map<BitBoard, int> checkedBoard_;
  std::unordered_map<BitBoard, std::unordered_set<BitBoard>> pred_;

  bool isUnclearableSpeed(const BitBoard &initB, int i) {
    bool cleared = false;
    std::queue<std::pair<int, BitBoard>> q;
    std::unordered_set<BitBoard> seenBoards;

    seenBoards.insert(initB);
    q.push({0, initB});
    while (!q.empty()) {
      const auto [amount, b] = q.front(); q.pop();
      if (leadsToClearable_.count(b)) {
        cleared = true;
        for (auto pb: pred_[b]) markPred(pb);
        break;
      }

      auto movesRemaining = amount_ - amount;
      if (movesRemaining == 0) break;
 
      for (auto blockType: allBlockTypes) {
        for (auto piece: mf.findAllMoves(b, blockType)) {
          auto [b2, maxRow] = b.applyPieceInfoMaxRowCopy(piece);
          if (maxRow == NUM_ROWS-1) {
            cleared = true;
            markPred(b);
            break;
          }
          if (!seenBoards.count(b2)) {
            seenBoards.insert(b2);
            pred_[b2].insert(b);
            q.push({amount+1, b2});
          }
        }
        if (cleared) break;
      }
      if (cleared) break;
    }
    return !cleared;
  }

  void markPred(const BitBoard &b) {
    if (leadsToClearable_.count(b)) return;
    leadsToClearable_.insert(b);
    //printf("markpred... %lu\n", leadsToClearable_.size());
    for (auto pb: pred_[b]) markPred(pb);
  }
 
};

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

struct UnclearableHelper {
  static std::vector<BitBoard> initCurrentBoardsNaive() {
    std::vector<BitBoard> currentBoards;

    auto vs = std::vector(NUM_ROWS, std::vector(NUM_COLUMNS, 0));
    for (int i = 1018; i < 1019; ++i) {
      for (auto j = 0; j < NUM_COLUMNS; ++j) vs[NUM_ROWS-1][j] = (i & (1 << (NUM_COLUMNS-j-1))) > 0;
      currentBoards.push_back(BitBoard(vs));
    }
    return currentBoards;
  }

  static std::vector<BitBoard> initCurrentBoardsFromFile(const std::string &filename) {
    std::ifstream fin(std::string(WORKSPACE_DIR) + "/singles/" + filename);
    std::vector<BitBoard> currentBoards;

    std::string s;
    fin >> s >> s;
    while (fin >> s) {
      if (s == "end") break;
      currentBoards.push_back(BitBoard(s));
    }
    return currentBoards;
  }
};

const int BATCH_SIZE = 1;
int doUnclearable() {
  std::vector<BitBoard> currentBoards = UnclearableHelper::initCurrentBoardsNaive();
  for (int i = 3; i < 6; ++i) {
    auto outname = std::string(WORKSPACE_DIR) + "/singles/" + std::to_string(i) + ".out";
    std::ofstream fout(outname, std::ofstream::out);
    fout << "numMoves: " << i << std::endl;
    logf("num moves: %d, remaining boards: %lu\n", i, currentBoards.size());
    auto fn = [i](const BitBoard &board) -> bool {
      auto v = Unclearable(i);
      return v.getUnclearableBoards({board}).size() == 1;
    };
    std::vector<BitBoard> nxCurrent;

    for (int j = 0; j < currentBoards.size(); j += BATCH_SIZE) {
      int lastIndex = std::min(j+BATCH_SIZE-1, (int)currentBoards.size()-1);
      logf("current batch: %d-%d (batch %d/%0.f)\n", j, lastIndex, (j/BATCH_SIZE)+1, std::ceil((double)currentBoards.size()/BATCH_SIZE));
      std::vector<BitBoard> batch;
      for (int k = j; k <= lastIndex; ++k) batch.push_back(currentBoards[k]);
      std::vector<bool> isUnclearable(lastIndex - j + 1);
      std::transform(std::execution::par_unseq, batch.begin(), batch.end(), isUnclearable.begin(), fn);
      std::vector<BitBoard> batchOut;
      for (int k = 0; k < isUnclearable.size(); ++k) {
        if (isUnclearable[k]) {
          nxCurrent.push_back(batch[k]);
          batchOut.push_back(batch[k]);
        }
      }
      logf("nxCurrent size: %lu\n", nxCurrent.size());
      for (auto v: batchOut) {
        fout << v << std::endl;
      }
    }
    printf("---------\n");
    fout << "end batch" << std::endl;

    currentBoards = nxCurrent;
  }
  return 0;
}

int ct = 0;
const int HEIGHT = 3;
int main(int argc, char ** argv) {
  return doUnclearable();
  //testing(); return 0;
  auto startTime = std::chrono::system_clock::now();
  std::unordered_set<BitBoard> boards;
  std::queue<BitBoard> q;
  boards.insert(BitBoard());
  q.push(BitBoard());
  AllMoveFinder<BitBoard, BitPieceInfo> mf;
  mf.setOffsetHeight(2);

  while (!q.empty()) {
    ct++;
    if (ct%100000 == 0) printf("ct: %d\n", ct);
    const auto b = q.front(); q.pop();
    for (const auto blockType: allBlockTypes) {
      const auto moves = mf.findAllMoves(b, blockType);
      for (const auto pieceInfo: moves) {
        const auto [b2, lineClears] = b.applyPieceInfoCopy(pieceInfo);
        if (b2.getPileHeight() > HEIGHT || boards.count(b2)) continue;
        boards.insert(b2);
        q.push(b2);
      }
    }
  }

  std::cout << "count: " << ct << '\n';
  auto endTime = std::chrono::system_clock::now();
  printf("time taken: %ldms\n", getMs(endTime-startTime));
}



// todo: subsequent board
void testing() {
  BitBoard b;
  AllMoveFinder<BitBoard, BitPieceInfo> mf;
  mf.findAllMoves(b, BlockType::O_PIECE);
  auto [b2, lineClears] = b.applyPieceInfoCopy(b.getPieceFromId(121));
  auto allMoves = mf.findAllMoves(b2, BlockType::I_PIECE);
  for (auto move: allMoves) {
    b.applyPieceInfoCopy(move);
  }
}