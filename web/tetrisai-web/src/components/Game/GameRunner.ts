import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { IBoard, Board } from './common/Board';
import { ExtraInformation } from './ExtraInformation';
import { IReadCurrentPiece } from '../GameRunner/ReadCurrentPiece';
import { IGetNextMove, FirstMoveDirectionT } from './IGetNextMove';
import { ICapturable } from './ICapturable';
import { IReadNextPiece } from './IReadNextPiece';
import { getDemoEntry, getDemoEntryWithStartFrame } from './DemoEntryHelpers';
import { Features } from './Features';

function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export class GameRunner implements ICapturable<any> {
  private nextMoveEntries: DemoEntry[] = [];
  private nextMoveBoard: IBoard = new Board();
  private expFrame: number = 0;
  private extraInformation: ExtraInformation;
  private totalLineClears: number = 0;

  private readonly demoPlayer: IDemoPlayer;
  private readonly getNextMoveHandler: IGetNextMove;
  private readonly readNextPieceHandler: IReadNextPiece;
  private readonly readCurrentPieceHandler: IReadCurrentPiece;

  private debug: any;
  private tableBoard: any = null;
  private oldPiece: Piece;
  private oldBoard: IBoard;
  private fpsControl = true;

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
    //this.debug['board'] = this.nextMoveBoard.getBitstring();
  }

  public setTableBoard(tableBoard: any) {
    this.tableBoard = tableBoard;
  }

  public disableFpsControl() {
    this.fpsControl = false;
  }

  public async onFirstPieceAppear() {
    const currFrame = this.demoPlayer.getFrame();
    this.demoPlayer.addEvents([
      getDemoEntryWithStartFrame(currFrame + 1, 0, DemoButton.BUTTON_LEFT, true),
      getDemoEntryWithStartFrame(currFrame + 2, 1, DemoButton.BUTTON_LEFT, false)
    ]);
    console.log(currFrame + 2);

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
    this.oldBoard = this.nextMoveBoard;
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
    this.oldPiece = nextPiece;
    console.log('currFrame, expFrame', currFrame, this.expFrame);
    //this.demoPlayer.addEvent({frame: 555, button: DemoButton.BUTTON_RIGHT, isDown: true});
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */

  public async onNextPieceAppear() {
    const oldFps = this.demoPlayer.timer.getFps();
    this.demoPlayer.timer.freeze();
    const currFrame = this.demoPlayer.getFrame();
    this.addRawDemoEventsToDemoPlayer(
      this.nextMoveEntries,
      this.extraInformation
    );
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();

    //this.debug['board'] = this.nextMoveBoard.getBitstring();
    this.debug['nextPiece'] = nextPiece;
    console.log('next Piece', nextPiece);

    const oldPiece = this.oldPiece;
    const oldLineClears = this.extraInformation.lineClears;
    await this.adaptBasedOnNextPiece(
      this.oldBoard,
      oldPiece,
      nextPiece,
      oldLineClears
    );
    this.oldBoard = this.nextMoveBoard;
    await this.prepareEntriesForNextPieceAppear(nextPiece);
    this.forceEarlyMove(currFrame);

    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    this.oldPiece = nextPiece;
    this.demoPlayer.timer.unfreeze();
  }

  public capture(): any {
    return JSON.stringify({
      demoPlayer: this.demoPlayer.capture(),
      nextMoveBoardString: this.nextMoveBoard.getBitstring(),
      oldBoardString: this.oldBoard.getBitstring(),
      oldPiece: this.oldPiece,
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
    this.oldBoard = new Board(json.oldBoardString);
    // @ts-ignore
    this.oldPiece = Piece[json.oldPiece];
  }

  private async prepareEntriesForNextPieceAppear(nextPiece: Piece) {
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
  }

  private forceEarlyMove(currFrame: number) {
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
        this.demoPlayer.addEvent(
          getDemoEntry(
            currFrame + this.extraInformation.lastFrame + 3,
            this.nextMoveEntries[0].button,
            true
          )
        );
        this.nextMoveEntries.slice(1);
      }
    }
  }

  private async adaptBasedOnNextPiece(
    oldBoard: IBoard,
    oldPiece: Piece,
    nextPiece: Piece,
    oldLineClears: number
  ) {
    if (!Features.adaptBasedOnNextPiece) return;
    const currFrame = this.demoPlayer.getFrame();
    const firstMoveDirection = this.getFirstMoveDirection(
      this.nextMoveEntries,
      currFrame
    );

    const res = await this.getNextMoveHandler.getNextMoveEntriesGivenNextPiece(
      oldBoard,
      oldPiece,
      nextPiece,
      this.totalLineClears,
      firstMoveDirection
    );

    const clearAmount = firstMoveDirection !== 'NONE' ? 2 : 0;
    const demoEntries = res.demoEntries.slice(clearAmount);
    for (const demoEntry of demoEntries) {
      demoEntry.frame += currFrame;
    }

    this.nextMoveBoard = res.board;
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    //this.debug['board'] = this.nextMoveBoard.getBitstring();

    const lineClears = res.extraInformation.lineClears;
    this.totalLineClears += lineClears - oldLineClears;
    this.debug['totalLineClears'] = this.totalLineClears;

    this.demoPlayer.clearEvents(clearAmount);
    this.demoPlayer.addEvents(demoEntries);
  }

  private getFirstMoveDirection(
    demoEntries: DemoEntry[],
    currFrame: number
  ): FirstMoveDirectionT {
    if (demoEntries.length === 0 || demoEntries[0].frame !== currFrame) {
      return 'NONE';
    }
    switch (demoEntries[0].button) {
      case DemoButton.BUTTON_LEFT:
        return 'LEFT';
      case DemoButton.BUTTON_RIGHT:
        return 'RIGHT';
      default:
        return 'NONE';
    }
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
    this.demoPlayer.addEvents(demoEvents);
  }
}
