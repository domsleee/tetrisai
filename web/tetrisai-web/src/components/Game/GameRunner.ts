import { IDemoPlayer, DemoEntry } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard, Board } from './common/Board';
import { ExtraInformation } from './ExtraInformation';

function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export class GameRunner {
  private nextMoveEntries: DemoEntry[] = [];
  private nextMoveBoard: IBoard = new Board();
  private expFrame: number = 0;
  private extraInformation: ExtraInformation = {};

  private demoPlayer: IDemoPlayer;
  private getNextMoveHandler: IGetNextMove;
  private readNextPieceHandler: IReadNextPiece;

  public constructor(demoPlayer: IDemoPlayer, getNextMoveHandler: IGetNextMove, readNextPieceHandler: IReadNextPiece) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
  }

  public async onFirstPieceAppear() {
    // todo.
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    const currFrame = this.demoPlayer.getLatestFrame();
    [this.nextMoveEntries, this.nextMoveBoard, this.extraInformation] = await this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    console.log("currFrame, expFrame", currFrame, this.expFrame);
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */
  static failed = 0;

  public async onNextPieceAppear() {
    if (this.demoPlayer.getLatestFrame() !== this.expFrame) {
      console.log(`INCORRECT. Expected ${this.expFrame}, actually ${this.demoPlayer.getLatestFrame()}`);
      console.log(this.nextMoveBoard);
      console.log(this.nextMoveEntries);
      GameRunner.failed++;
      if (GameRunner.failed > 15) throw new Error("you've failed for the last time!")
      //throw new Error("DFLSJKLF");
    }
    const currFrame = this.demoPlayer.getLatestFrame();
    for (const demoEntry of this.nextMoveEntries) {
      demoEntry.frame += currFrame;
    }
    //console.log(this.nextMoveEntries);
    //await sleep(5*1000000000000000);
    this.demoPlayer.addEvents(this.nextMoveEntries);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    console.log("next Piece", nextPiece);

    this.nextMoveEntries = [];
    [this.nextMoveEntries, this.nextMoveBoard, this.extraInformation] = await this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
  }
}

export interface IGetNextMove {
  getNextMoveEntries(board: IBoard, nextPiece: Piece): Promise<[DemoEntry[], IBoard, ExtraInformation]>;
}

export interface IReadNextPiece {
  getCurrentPieceFromEmulator(): Piece;
}
