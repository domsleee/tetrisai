import { Piece } from './common/Enums';

export interface IReadNextPiece {
  getCurrentPieceFromEmulator(): Piece;
}
