import { ReadCurrentPiece, IReadCurrentPiece } from './ReadCurrentPiece';
import { IPixelChecker } from '../Game/PixelGetter';
import { ReadBoard } from './ReadBoard';

class ReadCurrentPieceFactory {
  public static getInstance(pixelChecker: IPixelChecker): IReadCurrentPiece {
    const readBoard = new ReadBoard(pixelChecker);
    return new ReadCurrentPiece(readBoard);
  }
}