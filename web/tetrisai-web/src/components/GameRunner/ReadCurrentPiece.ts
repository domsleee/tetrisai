import { Piece, ALL_PIECE_VALUES } from '../Game/common/Enums';
import { IReadBoard } from './ReadBoard';
import { PieceCoords, Coord } from './PieceCoords';
import { ErrorHandler } from '../Game/common/ErrorHandler';

export interface IReadCurrentPiece {
  getPieceFromEmulator(): Piece;
}

export class ReadCurrentPiece implements IReadCurrentPiece {
  private readBoardHandler: IReadBoard;
  public constructor(readBoardHandler: IReadBoard) {
    this.readBoardHandler = readBoardHandler;
  }

  public getPieceFromEmulator(): Piece {
    for (const piece of ALL_PIECE_VALUES) {
      const coords = PieceCoords.getStartingCoords(piece);
      const coordOk = (coord: Coord) => !this.readBoardHandler.vacant(coord.r, coord.c);
      if (coords.every(coordOk)) {
        return piece;
      }
    }
    ErrorHandler.fatal('no piece is being displayed');
  }
}
