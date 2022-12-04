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
import { default as loglevel } from 'loglevel';
import { ScoreManager } from './ScoreManager';

const log = loglevel.getLogger('GameRunner');
loglevel.setDefaultLevel('info');
log.setLevel('info');

function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export interface GameRunnerResult {
  lineClears: number;
  level: number;
  score: number;
  pieces: Array<Piece>;
};

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
  private readonly scoreManager: ScoreManager = new ScoreManager();

  private debug: any;
  private tableBoard: any = null;
  private currentPiece: Piece;
  private currentBoard: IBoard;

  public pieces: Array<Piece> = [];

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

  public getResult(): GameRunnerResult {
    return {
      lineClears: this.totalLineClears,
      score: this.scoreManager.getScore(),
      level: this.scoreManager.getLevel(),
      pieces: this.pieces
    };
  }

  public setTableBoard(tableBoard: any) {
    this.tableBoard = tableBoard;
  }

  public async onFirstPieceAppear() {
    this.demoPlayer.timer.stop();
    const currFrame = this.demoPlayer.getFrame();
    log.debug("onFirstPieceAppear", currFrame + 2);

    const currPiece = this.readCurrentPieceHandler.getPieceFromEmulator();
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    this.addPiece(currPiece);
    this.addPiece(nextPiece);
    if (this.tableBoard) {
      this.tableBoard['board'] = this.nextMoveBoard;
    }
  
    if (Features.adaptBasedOnNextPiece) {
      const isGameOver = await this.adaptBasedOnNextPieceAndUpdateExtraInformation(
        new Board(),
        currPiece,
        nextPiece,
        0
      );
      if (isGameOver) return isGameOver;
    } else {
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
          totalLineClears: 0
        }
      );
  
      this.addLineClears(this.extraInformation.lineClears);
      this.addRawDemoEventsToDemoPlayer(moveEntries);
    }

    this.currentBoard = this.nextMoveBoard;
    this.currentPiece = nextPiece;
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    this.demoPlayer.timer.resume();
  }

  private addPiece(piece: Piece) {
    this.pieces.push(piece);
    this.debug['numPieces']++;
  }

  /**
   * pre: nextMoveEntries & nextMoveBoard are set
   * post: nextMoveEntries and nextMoveBoard are set correctly
   */

  public async onNextPieceAppear(): Promise<boolean> {
    this.demoPlayer.timer.stop();

    const currFrame = this.demoPlayer.getFrame();
    log.info("APPEARED ON FRAME", currFrame);

    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    const nextPiece = this.readNextPieceHandler.getCurrentPieceFromEmulator();
    this.addPiece(nextPiece);
    this.debug['currentPiece'] = this.currentPiece;
    this.debug['nextPiece'] = nextPiece;

    if (Features.adaptBasedOnNextPiece) {
      const isGameOver = await this.adaptBasedOnNextPieceAndUpdateExtraInformation(
        this.currentBoard,
        this.currentPiece,
        nextPiece,
        this.totalLineClears
      );
      if (isGameOver) return isGameOver;
    } else {
      let isGameOver, moveEntries;
      [
        isGameOver,
        moveEntries,
        this.nextMoveBoard,
        this.extraInformation
      ] = await this.getNextMoveHandler.getNextMoveEntries(
        this.currentBoard,
        this.currentPiece,
        {
          totalLineClears: this.totalLineClears
        }
      );
      if (isGameOver) return isGameOver;
      this.addLineClears(this.extraInformation.lineClears);
      this.addRawDemoEventsToDemoPlayer(moveEntries);
    }

    this.assertBoardIsCorrect();
    this.expFrame = currFrame + (this.extraInformation.lastFrame || 0);
    this.addLineClears(this.extraInformation.lineClears);

    this.currentBoard = this.nextMoveBoard;
    this.currentPiece = nextPiece;
    this.demoPlayer.timer.resume();
    return false;
  }

  private addLineClears(clears: number) {
    this.totalLineClears += clears;
    this.scoreManager.addLines(clears);
    this.debug['totalLineClears'] = this.totalLineClears;
    this.debug['scoreManager'] = this.scoreManager;
  }

  private assertBoardIsCorrect() {
    const actBoard = Board.fromBoardReaderMinusCoords(this.boardReader, PieceCoords.getStartingCoords(this.currentPiece));
    const expBoard = this.currentBoard;
    if (actBoard.getBitstring() != expBoard.getBitstring()) {
      log.error(actBoard.getBitstring());
      log.error(expBoard.getBitstring());
      ErrorHandler.fatal("ree");
    }
  }

  
  private async adaptBasedOnNextPieceAndUpdateExtraInformation(
    currentBoard: IBoard,
    currentPiece: Piece,
    nextPiece: Piece,
    currentLineClears: number
  ): Promise<boolean> {

    const res = await this.getNextMoveHandler.getNextMoveEntriesGivenNextPiece(
      currentBoard,
      currentPiece,
      nextPiece,
      currentLineClears,
    );
    if (res.isGameOver) return true;

    let demoEntries = res.demoEntries;

    this.nextMoveBoard = res.board;
    if (this.tableBoard) this.tableBoard['board'] = this.nextMoveBoard;
    this.demoPlayer.deleteAll();
    this.addRawDemoEventsToDemoPlayer(demoEntries);
    this.extraInformation = res.extraInformation;
    return false;
  }

  private addRawDemoEventsToDemoPlayer(
    demoEvents: DemoEntry[]
  ) {
    const currFrame = this.demoPlayer.getFrame();
    log.debug("ADDING RAW EVENTS ON FRAME", currFrame);
    this.fixDemoEvents(demoEvents);
    this.demoPlayer.addEvents(demoEvents);
  }

  private fixDemoEvents(demoEvents: DemoEntry[]) {
    const currFrame = this.demoPlayer.getFrame();
    for (const demoEntry of demoEvents) {
      if (demoEntry.startFrame <= 1) {
        ErrorHandler.fatal("??");
      }
      else demoEntry.frame += currFrame - 2;
    }
  }

  public capture(): any {
    return JSON.stringify({
      demoPlayer: this.demoPlayer.capture(),
      nextMoveBoardString: this.nextMoveBoard.getBitstring(),
      currentBoardString: this.currentBoard.getBitstring(),
      currentPiece: this.currentPiece,
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
    this.currentBoard = new Board(json.currentBoardString);
    // @ts-ignore
    this.currentPiece = Piece[json.currentPiece];
  }

}
