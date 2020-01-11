import { GameRunner, IGetNextMove, IReadNextPiece } from './GameRunner';
import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard } from './common/Board';
import { ReadNextPiece } from '../GameRunner/ReadNextPiece';
import { GetNextMove } from './GetNextMove';
import { PixelChecker, IPixelChecker } from './PixelGetter';
import { ExtraInformation } from './ExtraInformation';

export class GameRunnerFactory {
  public static getInstance(demoPlayer: IDemoPlayer, pixelChecker: IPixelChecker, debug: any): GameRunner {
    return new GameRunner(demoPlayer, new GetNextMove(), new ReadNextPiece(pixelChecker), debug);
  }
}

class SimpleGetNextMoveHandler implements IGetNextMove {
  public async getNextMoveEntries(board: IBoard, nextPiece: Piece): Promise<[DemoEntry[], IBoard, ExtraInformation]> {
    return [[
      {frame: 3, button: DemoButton.BUTTON_LEFT, isDown: true},
      {frame: 4, button: DemoButton.BUTTON_LEFT, isDown: false},
    ], board, {}];
  }
}

class StupidReadNextPiece implements IReadNextPiece {
  public getCurrentPieceFromEmulator(): Piece {
    return Piece.I_PIECE;
  }
}
