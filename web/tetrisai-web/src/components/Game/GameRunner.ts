import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard, Board } from './common/Board';
import { ExtraInformation } from './ExtraInformation';
import { IReadCurrentPiece } from '../GameRunner/ReadCurrentPiece';

function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export class GameRunner {
  private nextMoveEntries: DemoEntry[] = [];
  private nextMoveBoard: IBoard = new Board();
  private expFrame: number = 0;
  private extraInformation: ExtraInformation = {};

  private readonly demoPlayer: IDemoPlayer;
  private readonly getNextMoveHandler: IGetNextMove;
  private readonly readNextPieceHandler: IReadNextPiece;
  private readonly readCurrentPieceHandler: IReadCurrentPiece;

  private debug: any;
  private tableBoard: any = null;

  public constructor(demoPlayer: IDemoPlayer,
                     getNextMoveHandler: IGetNextMove,
                     readNextPieceHandler: IReadNextPiece,
                     readCurrentPieceHandler: IReadCurrentPiece,
                     debug: any) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
    this.readCurrentPieceHandler = readCurrentPieceHandler;
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
    const currFrame = this.demoPlayer.getFrame();
    this.demoPlayer.addEvent({
      button: DemoButton.BUTTON_LEFT,
      isDown: false,
      frame: currFrame + 2,
    });

    const currPiece = this.readCurrentPieceHandler.getPieceFromEmulator();
    let moveEntries, unused;
    [moveEntries, this.nextMoveBoard, unused] =
      await this.getNextMoveHandler.getNextMoveEntries(new Board(), currPiece, 'LEFT');
    this.addRawDemoEventsToDemoPlayer(moveEntries);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    if (this.tableBoard) { this.tableBoard['board'] = this.nextMoveBoard; }
    [this.nextMoveEntries, this.nextMoveBoard, this.extraInformation] =
      await this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
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
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    this.debug['board'] = this.nextMoveBoard.getBitstring();
    const currFrame = this.demoPlayer.getFrame();
    this.addRawDemoEventsToDemoPlayer(this.nextMoveEntries);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    this.debug['nextPiece'] = nextPiece;
    console.log("next Piece", nextPiece);

    this.nextMoveEntries = [];
    [this.nextMoveEntries, this.nextMoveBoard, this.extraInformation] = await this.getNextMoveHandler.getNextMoveEntries(this.nextMoveBoard, nextPiece);
    // add an event 3 frames later than last frame, and remove first nextMoveEntry
    if (this.nextMoveEntries.length > 0 && this.nextMoveEntries[0].frame === 1) {
      if (this.extraInformation.lastFrame) {
        this.demoPlayer.addEvent({
          frame: currFrame + this.extraInformation.lastFrame + 3,
          button: this.nextMoveEntries[0].button,
          isDown: true,
        });
        this.nextMoveEntries.slice(1);
      }
    }

    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
  }

  private addRawDemoEventsToDemoPlayer(demoEvents: DemoEntry[]) {
    const currFrame = this.demoPlayer.getFrame();
    for (const demoEntry of demoEvents) {
      demoEntry.frame += currFrame;
    }
    console.log('adding', demoEvents);
    this.demoPlayer.addEvents(demoEvents);
  }
}
export type FirstMoveDirectionT = 'LEFT' | 'RIGHT' | 'NONE';
export interface IGetNextMove {
  getNextMoveEntries(board: IBoard, nextPiece: Piece, firstMoveDirection?: FirstMoveDirectionT):
    Promise<[DemoEntry[], IBoard, ExtraInformation]>;
}

export interface IReadNextPiece {
  getCurrentPieceFromEmulator(): Piece;
}
