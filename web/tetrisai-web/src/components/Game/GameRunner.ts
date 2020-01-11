import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
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
  private debug: any;
  private tableBoard: any = null;

  public constructor(demoPlayer: IDemoPlayer, getNextMoveHandler: IGetNextMove, readNextPieceHandler: IReadNextPiece, debug: any) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
    this.nextMoveBoard = Board.fromNormalBoard([
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000000000",
      "0000110000",
      "0000110000",
    ]);
    this.debug = debug;
    this.debug['board'] = this.nextMoveBoard.getBitstring();
  }

  public setTableBoard(tableBoard: any) {
    this.tableBoard = tableBoard;
  }

  public async onFirstPieceAppear() {
    // todo.
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    const currFrame = this.demoPlayer.getFrame();
    if (this.tableBoard) { this.tableBoard['board'] = this.nextMoveBoard; }
    [this.nextMoveEntries, this.nextMoveBoard, this.extraInformation] = await this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    console.log("currFrame, expFrame", currFrame, this.expFrame);
    //this.demoPlayer.addEvent({frame: 555, button: DemoButton.BUTTON_RIGHT, isDown: true});
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */
  static failed = 0;

  public async onNextPieceAppear() {
    if (this.demoPlayer.getFrame() !== this.expFrame) {
      //console.log(`INCORRECT. Expected ${this.expFrame}, actually ${this.demoPlayer.getFrame()}`);
      //console.log(this.nextMoveBoard);
      //console.log(this.nextMoveEntries);
      GameRunner.failed++;
      if (GameRunner.failed > 1e9) throw new Error("you've failed for the last time!")
      //throw new Error("DFLSJKLF");
    }
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    this.debug['board'] = this.nextMoveBoard.getBitstring();

    const currFrame = this.demoPlayer.getFrame();
    for (const demoEntry of this.nextMoveEntries) {
      demoEntry.frame += currFrame;
    }
    console.log("nextMoveEntries...", this.nextMoveEntries);
    //await sleep(5*1000000000000000);
    this.demoPlayer.addEvents(this.nextMoveEntries);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    this.debug['nextPiece'] = nextPiece;
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
