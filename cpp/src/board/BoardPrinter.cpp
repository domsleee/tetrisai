#include "src/board/BoardPrinter.tpp"
#include "src/board/bitboard/BitBoard.h"

void printPiece(int id) {
  printBoardWithPiece(BitBoard(), BitBoard().getPieceFromId(id));
}