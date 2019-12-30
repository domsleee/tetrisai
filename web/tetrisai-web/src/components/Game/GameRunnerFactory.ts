import { GameRunner, IGetNextMove, IReadNextPiece } from './GameRunner';
import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard } from './common/Board';
import { ReadNextPiece } from '../GameRunner/ReadNextPiece';
import { PixelChecker, IPixelChecker } from './PixelGetter';

export class GameRunnerFactory {
  public static getInstance(demoPlayer: IDemoPlayer, pixelChecker: IPixelChecker): GameRunner {
    return new GameRunner(demoPlayer, new SimpleGetNextMoveHandler(), new ReadNextPiece(pixelChecker));
  }
}

class SimpleGetNextMoveHandler implements IGetNextMove {
  public getNextMoveEntries(board: IBoard, nextPiece: Piece): [DemoEntry[], IBoard] {
    return [[
      {frame: 3, button: DemoButton.BUTTON_LEFT, isDown: true},
      {frame: 4, button: DemoButton.BUTTON_LEFT, isDown: false},
    ], board];
  }
}

class StupidReadNextPiece implements IReadNextPiece {
  public getCurrentPieceFromEmulator(): Piece {
    return Piece.I_PIECE;
  }
}
