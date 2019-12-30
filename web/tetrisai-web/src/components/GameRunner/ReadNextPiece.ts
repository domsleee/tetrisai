import { IReadNextPiece } from "../Game/GameRunner";
import { Piece } from '../Game/common/Enums';
import { IPixelChecker } from '../Game/PixelGetter';


class NiceCoord {
  public readonly r: number;
  public readonly c: number;

  constructor(r: number, c: number) {
    this.r = r;
    this.c = c;
  }

  public move(dr: number, dc: number) {
    return new NiceCoord(this.r + 8 * dr, this.c + 8 * dc);
  }
}

export class ReadNextPiece implements IReadNextPiece {
  private static readonly lAnchor = new NiceCoord(121, 197);
  private static readonly zAnchor = ReadNextPiece.lAnchor;
  private static readonly tAnchor = ReadNextPiece.lAnchor;
  private static readonly jAnchor = ReadNextPiece.lAnchor;
  private static readonly oAnchor = new NiceCoord(121, 201);
  private static readonly iAnchor = new NiceCoord(125, 193);

  private readonly pixelChecker: IPixelChecker;

  public constructor(pixelChecker: IPixelChecker) {
    this.pixelChecker = pixelChecker;
  }

  public getCurrentPieceFromEmulator(): Piece {
    const isWhite = (coord: NiceCoord) => this.pixelChecker.getPixel(coord.r, coord.c) === 0xffffffff;

    const isLPiece = [
      ReadNextPiece.lAnchor,
      ReadNextPiece.lAnchor.move(0, 1),
      ReadNextPiece.lAnchor.move(0, 2),
      ReadNextPiece.lAnchor.move(1, 0),
    ].every(isWhite);

    const isIPiece = [
      ReadNextPiece.iAnchor,
      ReadNextPiece.iAnchor.move(0, 1),
      ReadNextPiece.iAnchor.move(0, 2),
      ReadNextPiece.iAnchor.move(0, 3),
    ].every(isWhite);

    const isZPiece = [
      ReadNextPiece.zAnchor,
      ReadNextPiece.zAnchor.move(0, 1),
      ReadNextPiece.zAnchor.move(1, 1),
      ReadNextPiece.zAnchor.move(1, 2),
    ].every(isWhite);

    const isTPiece = [
      ReadNextPiece.tAnchor,
      ReadNextPiece.tAnchor.move(0, 1),
      ReadNextPiece.tAnchor.move(0, 2),
      ReadNextPiece.tAnchor.move(1, 1),
    ].every(isWhite);

    const isOPiece = [
      ReadNextPiece.oAnchor,
      ReadNextPiece.oAnchor.move(0, 1),
      ReadNextPiece.oAnchor.move(1, 0),
      ReadNextPiece.oAnchor.move(1, 1),
    ].every(isWhite);

    const isJPiece = [
      ReadNextPiece.jAnchor,
      ReadNextPiece.jAnchor.move(0, 1),
      ReadNextPiece.jAnchor.move(0, 2),
      ReadNextPiece.jAnchor.move(1, 2),
    ].every(isWhite);

    switch (true) {
      case isLPiece: return Piece.L_PIECE;
      case isIPiece: return Piece.I_PIECE;
      case isZPiece: return Piece.Z_PIECE;
      case isTPiece: return Piece.T_PIECE;
      case isOPiece: return Piece.O_PIECE;
      case isJPiece: return Piece.J_PIECE;
      default: return Piece.S_PIECE;
    }
  }
}
