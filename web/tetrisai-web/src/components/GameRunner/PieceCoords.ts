import { Piece } from '../Game/common/Enums';

export interface Coord {
  r: number;
  c: number;
}

export class PieceCoords {
  public static getStartingCoords(blockType: Piece): Coord[] {
    switch (blockType/*, startingCoord */) {
      case Piece.I_PIECE: {
        return [
          {r: 0, c: 3}, {r: 0, c: 4}, {r: 0, c: 5}, {r: 0, c: 6},
        ];
      }
      case Piece.S_PIECE: {
        return [
          {r: 0, c: 5}, {r: 0, c: 6}, {r: 1, c: 4}, {r: 1, c: 5},
        ];
      }
      case Piece.Z_PIECE: {
        return [
          {r: 0, c: 4}, {r: 0, c: 5}, {r: 1, c: 5}, {r: 1, c: 6},
        ];
      }
      case Piece.O_PIECE: {
        return [
          {r: 0, c: 4}, {r: 0, c: 5}, {r: 1, c: 4}, {r: 1, c: 5},
        ];
      }
      case Piece.T_PIECE: {
        return [
          {r: 0, c: 4}, {r: 0, c: 5}, {r: 0, c: 6}, {r: 1, c: 5},
        ];
      }
      case Piece.J_PIECE: {
        return [
          {r: 0, c: 4}, {r: 0, c: 5}, {r: 0, c: 6}, {r: 1, c: 6},
        ];
      }
      case Piece.L_PIECE: {
        return [
          {r: 0, c: 4}, {r: 0, c: 5}, {r: 0, c: 6}, {r: 1, c: 4},
        ];
      }
    }
  }
}
