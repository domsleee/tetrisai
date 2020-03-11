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
import { ErrorHandler } from './common/ErrorHandler';
import { ReadBoard, IReadBoard } from '../GameRunner/ReadBoard';
import { PieceCoords } from '../GameRunner/PieceCoords';
import { arraySubtract } from './utils';

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
  private readonly boardReader: IReadBoard;

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
    boardReader: IReadBoard,
    debug: any
  ) {
    this.demoPlayer = demoPlayer;
    this.getNextMoveHandler = getNextMoveHandler;
    this.readNextPieceHandler = readNextPieceHandler;
    this.readCurrentPieceHandler = readCurrentPieceHandler;
    this.boardReader = boardReader;
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
    let moveEntries, isGameOver;
    [
      isGameOver,
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
      isGameOver,
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
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */

  public async onNextPieceAppear() {
    this.demoPlayer.timer.stop();

    const currFrame = this.demoPlayer.getFrame();
    console.log("APPEARED ON FRAME", currFrame);

    this.addRawDemoEventsToDemoPlayer(
      this.nextMoveEntries,
      this.extraInformation
    );
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();

    this.assertBoardIsCorrect();

    //this.debug['board'] = this.nextMoveBoard.getBitstring();
    this.debug['nextPiece'] = nextPiece;
    console.log('next Piece', nextPiece);

    const oldPiece = this.oldPiece;
    const oldLineClears = this.extraInformation.lineClears;
    console.log("oldLineClears", oldLineClears);
    let oldExtraInformation: ExtraInformation = JSON.parse(JSON.stringify(this.extraInformation));
    console.log(this.extraInformation);
    await this.adaptBasedOnNextPiece(
      this.oldBoard,
      oldPiece,
      nextPiece,
      oldLineClears
    );
    this.oldBoard = this.nextMoveBoard;
    let isGameOver = await this.prepareEntriesForNextPieceAppear(nextPiece);
    if (isGameOver) return true;

    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    this.oldPiece = nextPiece;
    this.demoPlayer.timer.resume();
  }

  private assertBoardIsCorrect() {
    console.log(this.oldPiece);
    const actBoard = Board.fromBoardReaderMinusCoords(this.boardReader, PieceCoords.getStartingCoords(this.oldPiece));
    const expBoard = this.oldBoard;
    if (actBoard.getBitstring() != expBoard.getBitstring()) {
      console.log(actBoard.getBitstring());
      console.log(expBoard.getBitstring());
      ErrorHandler.fatal("ree");
    }
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
    let isGameOver;
    this.nextMoveEntries = [];
    [
      isGameOver,
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
    return isGameOver;
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
    );

    console.log(firstMoveDirection);

    const res = await this.getNextMoveHandler.getNextMoveEntriesGivenNextPiece(
      oldBoard,
      oldPiece,
      nextPiece,
      this.totalLineClears,
      firstMoveDirection
    );

    if (res.isGameOver) return true;

    const clearAmount = firstMoveDirection !== 'NONE' ? 2 : 0;
    let demoEntries = res.demoEntries;

    this.nextMoveBoard = res.board;
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    //this.debug['board'] = this.nextMoveBoard.getBitstring();

    const lineClears = res.extraInformation.lineClears;
    this.totalLineClears += lineClears - oldLineClears;
    this.debug['totalLineClears'] = this.totalLineClears;

    console.log("FIRSTMOVEDIRECTION", firstMoveDirection, clearAmount);
    console.log("before adapting...");
    console.log(this.demoPlayer.getEventsRep());
    if (clearAmount > 0) {
      const events = this.demoPlayer.getEventsRep();
      let delEvents = this.getDelEvents(events);
      this.demoPlayer.deleteAllExcept(delEvents);

      let otherDelEvents = this.getDelEvents(demoEntries);
      demoEntries = arraySubtract(demoEntries, otherDelEvents);
    } else {
      this.demoPlayer.deleteAll();
    }
    //this.demoPlayer.clearEvents(clearAmount);
    this.fixDemoEvents(demoEntries);
    this.demoPlayer.addEvents(demoEntries);

    console.log("after adapting");
    console.log(this.demoPlayer.getEventsRep());
  }

  private getDelEvents(events: DemoEntry[]) {
    const ev1 = events[0];
    let delEvents = [ev1];
    for (let i = 1; i < events.length; ++i) {
      if (events[i].button === ev1.button) {
        delEvents.push(events[i]);
        break;
      }
    }
    if (delEvents.length !== 2) {
      throw ErrorHandler.fatal("unexpected deleting of events");
    }
    return delEvents;
  }

  private getFirstMoveDirection(
    demoEntries: DemoEntry[]
  ): FirstMoveDirectionT {
    console.log("GETFIRSTMOVEDIRECTION");
    console.log(demoEntries[0]);
    if (demoEntries.length === 0 || demoEntries[0].startFrame !== 2) {
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
    console.log("ADDING RAW EVENTS ON FRAME", currFrame);
    this.fixDemoEvents(demoEvents);
    this.demoPlayer.addEvents(demoEvents);
  }

  private fixDemoEvents(demoEvents: DemoEntry[]) {
    const currFrame = this.demoPlayer.getFrame();
    for (const demoEntry of demoEvents) {
      if (demoEntry.startFrame == 1) {
        demoEntry.frame += currFrame - 1;
      }
      else demoEntry.frame += currFrame - 2;
    }
  }
}
