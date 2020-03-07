#pragma once
#include "src/common/common.hpp"

template <typename MyBoard>
void printBoard(const MyBoard &b) {
  for (int c = 0; c < NUM_COLUMNS*2; ++c) printf("=");
  printf("\n");
  for (int r = 0; r < NUM_ROWS; ++r) {
    printf("|");
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (b.vacant({r, c})) printf(" ");
      else printf("█");
      printf("|");
    }
    printf("\n");
    printf("|");
    for(int c = 0; c < NUM_COLUMNS*2; ++c) printf("-");
    printf("|\n");
  }
  for (int c = 0; c < NUM_COLUMNS; ++c) printf("=");
  printf("\n");
}

template <typename MyBoard, typename MyBoardPiece>
void printBoardWithPiece(const MyBoard &b, const MyBoardPiece &p) {
  for (int c = 0; c < NUM_COLUMNS*2; ++c) printf("=");
  printf("\n");
  p.getPosition().print();
  for (int r = 0; r < NUM_ROWS; ++r) {
    printf("|");
    for (int c = 0; c < NUM_COLUMNS; ++c) {
      if (p.getPosition().hasCoord({r, c})) printf("X");
      else if (b.vacant({r, c})) printf(" ");
      else printf("█");
      printf("|");
    }
    printf("\n");
    printf("|");
    for(int c = 0; c < NUM_COLUMNS*2; ++c) printf("-");
    printf("|\n");
  }
  for (int c = 0; c < NUM_COLUMNS; ++c) printf("=");
  printf("\n");
}

