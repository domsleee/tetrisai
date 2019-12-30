import { IDemoPlayer, DemoEntry } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard, Board } from './common/Board';

export class GameRunner {
  private nextMoveEntries: DemoEntry[] = [];
  private nextMoveBoard: IBoard = new Board();

  private demoPlayer: IDemoPlayer;
  private getNextMoveHandler: IGetNextMove;
  private readNextPieceHandler: IReadNextPiece;

  public constructor(demoPlayer: IDemoPlayer, getNextMoveHandler: IGetNextMove, readNextPieceHandler: IReadNextPiece) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
  }

  public onFirstPieceAppear() {
    // todo.
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    [this.nextMoveEntries, this.nextMoveBoard] = this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
    console.log(this.nextMoveEntries);
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */
  public onNextPieceAppear() {
    for (const demoEntry of this.nextMoveEntries) {
      demoEntry.frame += this.demoPlayer.getLatestFrame();
    }
    this.demoPlayer.addEvents(this.nextMoveEntries);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    [this.nextMoveEntries, this.nextMoveBoard] = this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
  }
}

export interface IGetNextMove {
  getNextMoveEntries(board: IBoard, nextPiece: Piece): [DemoEntry[], IBoard];
}

export interface IReadNextPiece {
  getCurrentPieceFromEmulator(): Piece;
}
