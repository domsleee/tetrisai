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
#include "src/shared/MoveFinder/AllMoveFinder.tpp"

#include "src/board/BoardPrinter.tpp"
#include "src/board/bitboard/BitBoard.h"

void testing();
void unclearableLines();

// 31574 / (2^20) = ~3%
// 13463746 / (2^30) = ~1.25% (2mins)


struct Unclearable {
  AllMoveFinder<BitBoard, BitPieceInfo> mf;
  int amount_ = 4;

  std::unordered_set<BitBoard> getUnclearableBoards() {
    std::unordered_set<BitBoard> unclearableBoards;
    if (amount_ > 2) {
      auto c = Unclearable();
      c.amount_ = amount_-1;
      auto boards = c.getUnclearableBoards();
      printf("needed to check: %lu\n", boards.size());
      for (auto b: boards) {
        int i = 0;
        for (int j = 0; j < NUM_COLUMNS; ++j) i |= (!b.vacant(Coord(NUM_ROWS-1, j))) << j;
        if (isUnclearable(b, i)) {
          unclearableBoards.insert(b);
        }
      }
    } else {
      auto vs = std::vector(NUM_ROWS, std::vector(NUM_COLUMNS, 0));
      for (int i = 0; i < 1024; ++i) {
        for (auto j = 0; j < NUM_COLUMNS; ++j) vs[NUM_ROWS-1][j] = (i & (1 << j)) > 0;
        BitBoard initB(vs);
        if (isUnclearable(initB, i)) {
          unclearableBoards.insert(initB);
        }
      }
    }

    printf("unclearable boards: %lu\n", unclearableBoards.size());
    for (auto b: unclearableBoards) {
      //std::cout << b << '\n';
    }
    return unclearableBoards;
  }

  bool isUnclearable(const BitBoard &initB, int i) {
    std::bitset<200> bottomRow;
    for (auto j = 0; j < NUM_COLUMNS; ++j) if ((i & (1 << j)) > 0) bottomRow.set((NUM_ROWS-1)*NUM_COLUMNS + j);
    auto bottomRowString = bottomRow.to_string().substr(0, 10);

    bool cleared = false;
    std::queue<std::pair<int, BitBoard>> q;
    std::unordered_set<BitBoard> seenBoards;

    seenBoards.insert(initB);
    q.push({0, initB});
    while (!q.empty()) {
      const auto [amount, b] = q.front(); q.pop();
      if (amount >= amount_) break;
      for (auto blockType: allBlockTypes) {
        for (auto piece: mf.findAllMoves(b, blockType)) {
          auto [b2, lineClears] = b.applyPieceInfoCopy(piece);
          if (lineClears >= 1) {
            auto bs = b2.getBitSet();
            bs &= bottomRow;
            if (bs.to_string().substr(0, 10) != bottomRowString) {
              cleared = true;
              break;
            }
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
};


int ct = 0;
const int HEIGHT = 3;
int main(int argc, char ** argv) {
  Unclearable().getUnclearableBoards(); return 0;
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