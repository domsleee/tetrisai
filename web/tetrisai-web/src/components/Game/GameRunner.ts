import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard, Board } from './common/Board';
import { ExtraInformation } from './ExtraInformation';
import { IReadCurrentPiece } from '../GameRunner/ReadCurrentPiece';
import { IGetNextMove } from './IGetNextMove';
import { ICapturable } from './ICapturable';

function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export class GameRunner implements ICapturable<any> {
  private nextMoveEntries: DemoEntry[] = [];
  private nextMoveBoard: IBoard = new Board();
  private expFrame: number = 0;
  private extraInformation: ExtraInformation = {};
  private totalLineClears: number = 0;

  private readonly demoPlayer: IDemoPlayer;
  private readonly getNextMoveHandler: IGetNextMove;
  private readonly readNextPieceHandler: IReadNextPiece;
  private readonly readCurrentPieceHandler: IReadCurrentPiece;

  private debug: any;
  private tableBoard: any = null;

  public constructor(
    demoPlayer: IDemoPlayer,
    getNextMoveHandler: IGetNextMove,
    readNextPieceHandler: IReadNextPiece,
    readCurrentPieceHandler: IReadCurrentPiece,
    debug: any
  ) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
    this.readCurrentPieceHandler = readCurrentPieceHandler;
    this.nextMoveBoard = Board.fromNormalBoard([
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000000000',
      '0000110000',
      '0000110000'
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
      frame: currFrame + 2
    });

    const currPiece = this.readCurrentPieceHandler.getPieceFromEmulator();
    let moveEntries;
    [
      moveEntries,
      this.nextMoveBoard,
      this.extraInformation
    ] = await this.getNextMoveHandler.getNextMoveEntries(
      new Board(),
      currPiece,
      {
        firstMoveDirection: 'LEFT',
        totalLineClears: this.totalLineClears
      }
    );
    this.addRawDemoEventsToDemoPlayer(moveEntries, this.extraInformation);

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    if (this.tableBoard) {
      this.tableBoard['board'] = this.nextMoveBoard;
    }
    [
      this.nextMoveEntries,
      this.nextMoveBoard,
      this.extraInformation
    ] = await this.getNextMoveHandler.getNextMoveEntries(
      this.nextMoveBoard,
      nextPiece,
      {
        totalLineClears: this.totalLineClears
      }
    );
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    console.log('currFrame, expFrame', currFrame, this.expFrame);
    //this.demoPlayer.addEvent({frame: 555, button: DemoButton.BUTTON_RIGHT, isDown: true});
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */

  public async onNextPieceAppear() {
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    this.debug['board'] = this.nextMoveBoard.getBitstring();
    const currFrame = this.demoPlayer.getFrame();
    this.addRawDemoEventsToDemoPlayer(
      this.nextMoveEntries,
      this.extraInformation
    );

    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    this.debug['nextPiece'] = nextPiece;
    console.log('next Piece', nextPiece);

    this.nextMoveEntries = [];
    [
      this.nextMoveEntries,
      this.nextMoveBoard,
      this.extraInformation
    ] = await this.getNextMoveHandler.getNextMoveEntries(
      this.nextMoveBoard,
      nextPiece,
      {
        totalLineClears: this.totalLineClears
      }
    );
    // add an event 3 frames later than last frame, and remove first nextMoveEntry
    if (
      this.nextMoveEntries.length > 0 &&
      this.nextMoveEntries[0].frame === 1 &&
      [DemoButton.BUTTON_LEFT, DemoButton.BUTTON_RIGHT].indexOf(
        this.nextMoveEntries[0].button
      ) !== -1
    ) {
      console.log('LATEST');
      console.log(this.nextMoveEntries);
      if (this.extraInformation.lastFrame) {
        this.demoPlayer.addEvent({
          frame: currFrame + this.extraInformation.lastFrame + 3,
          button: this.nextMoveEntries[0].button,
          isDown: true
        });
        this.nextMoveEntries.slice(1);
      }
    }

    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
  }

  public capture(): any {
    return JSON.stringify({
      demoPlayer: this.demoPlayer.capture(),
      nextMoveBoardString: this.nextMoveBoard.getBitstring(),
      nextMoveEntries: JSON.stringify(this.nextMoveEntries),
      extraInformation: JSON.stringify(this.extraInformation),
      totalLineClears: this.totalLineClears
    });
  }

  public restoreFromCapture(capture: any) {
    const json = JSON.parse(capture);
    this.demoPlayer.restoreFromCapture(json.demoPlayer);
    this.nextMoveEntries = JSON.parse(json.nextMoveEntries);
    this.extraInformation = JSON.parse(json.extraInformation);
    this.nextMoveBoard = new Board(json.nextMoveBoardString);
    this.totalLineClears = json.totalLineClears;
  }

  private addRawDemoEventsToDemoPlayer(
    demoEvents: DemoEntry[],
    extraInfo: ExtraInformation
  ) {
    if (extraInfo.lineClears) {
      console.log('totalLineClears', this.totalLineClears);
      this.totalLineClears += extraInfo.lineClears;
      this.debug['totalLineClears'] = this.totalLineClears;
    }

    const currFrame = this.demoPlayer.getFrame();
    for (const demoEntry of demoEvents) {
      demoEntry.frame += currFrame;
    }
    console.log('adding', demoEvents);
    this.demoPlayer.addEvents(demoEvents);
  }
}

export interface IReadNextPiece {
  getCurrentPieceFromEmulator(): Piece;
}
