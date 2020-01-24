import { GameRunner } from './GameRunner';
import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard } from './common/Board';
import { ReadNextPiece } from '../GameRunner/ReadNextPiece';
import { GetNextMove } from './GetNextMove';
import { PixelChecker, IPixelChecker } from './PixelGetter';
import { ExtraInformation } from './ExtraInformation';
import { ReadCurrentPiece } from '../GameRunner/ReadCurrentPiece';
import { ReadBoard } from '../GameRunner/ReadBoard';

export class GameRunnerFactory {
  public static getInstance(
    demoPlayer: IDemoPlayer,
    pixelChecker: IPixelChecker,
    debug: any
  ): GameRunner {
    const readBoard = new ReadBoard(pixelChecker);
    return new GameRunner(
      demoPlayer,
      new GetNextMove(),
      new ReadNextPiece(pixelChecker),
      new ReadCurrentPiece(readBoard),
      debug
    );
  }
}
