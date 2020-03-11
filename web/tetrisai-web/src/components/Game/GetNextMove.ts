import { DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { Board, IBoard } from './common/Board';
import axios, { AxiosResponse } from 'axios';
import { ExtraInformation } from './ExtraInformation';
import {
  IGetNextMove,
  OptionalNextMoveParams,
  FirstMoveDirectionT,
  NextTwoPiecesReturnT
} from './IGetNextMove';

const URL = 'http://localhost:5000/get-moves';
const URL2 = 'http://localhost:5000/get-moves-given-piece';
/* get-moves-given-piece
 */

interface GetMovesRequestT {
  board: string;
  piece: string;
  first_move_direction?: FirstMoveDirectionT;
  line_clears?: number;
}

interface GetMovesGivenPieceRequestT {
  board: string;
  piece: string;
  next_piece: string;
  first_move_direction: FirstMoveDirectionT;
  line_clears: number;
}

interface GetMovesGivenPieceResponseT {
  board: string;
  demo_entries: string[];
  line_clears: number;
}

export class GetNextMove implements IGetNextMove {
  public async getNextMoveEntries(
    board: IBoard,
    nextPiece: Piece,
    optional: OptionalNextMoveParams
  ): Promise<[boolean, DemoEntry[], IBoard, ExtraInformation]> {
    const data: GetMovesRequestT = {
      board: board.getBitstring(),
      piece: nextPiece.toString()
    };
    if (optional.firstMoveDirection) {
      data.first_move_direction = optional.firstMoveDirection;
    }
    if (optional.totalLineClears) {
      data.line_clears = optional.totalLineClears;
    }
    const resp = await axios.post(URL, data);
    if (!('board' in resp.data)) {
      return [true, [], new Board(), {lineClears: 0}];
    }

    const nxBoard = new Board(resp.data.board);
    console.log('REQUEST');
    console.log(data);
    //console.log('resp.data');
    //console.log(resp.data);
    if (optional.firstMoveDirection) {
      resp.data.demo_entries = resp.data.demo_entries.slice(1);
    }
    const [demoEntries, lastFrame] = this.parseDemoEntries(
      resp.data.demo_entries
    );
    return [
      false,
      demoEntries,
      nxBoard,
      {
        lastFrame,
        lineClears: parseInt(resp.data.line_clears, 10)
      }
    ];
  }

  public async getNextMoveEntriesGivenNextPiece(
    board: IBoard,
    piece: Piece,
    nextPiece: Piece,
    totalLineClears: number,
    firstMoveDirection: FirstMoveDirectionT
  ): Promise<NextTwoPiecesReturnT> {
    const req: GetMovesGivenPieceRequestT = {
      board: board.getBitstring(),
      piece: piece.toString(),
      next_piece: nextPiece.toString(),
      first_move_direction: firstMoveDirection,
      line_clears: totalLineClears
    };
    const resp: AxiosResponse<GetMovesGivenPieceResponseT> = await axios.post(
      URL2,
      req
    );
    if (!('board' in resp.data)) {
      return {isGameOver: true, demoEntries: [], board: new Board(), extraInformation: { lineClears: 0 }};
    }
    const [demoEntries, lastFrame] = this.parseDemoEntries(
      resp.data.demo_entries
    );
    const nxBoard = new Board(resp.data.board);
    return {
      isGameOver: false,
      demoEntries,
      board: nxBoard,
      extraInformation: {
        lineClears: resp.data.line_clears
      }
    };
  }

  private parseDemoEntries(demo_entries: string[]): [DemoEntry[], number] {
    let demoEntries: DemoEntry[] = [];
    let lastFrame = 0;
    console.log(demo_entries);
    for (const str of demo_entries) {
      const [frameStr, action, ...rest] = str.split(' ');
      lastFrame = parseInt(frameStr, 10);
      if (action === 'DOWN') {
        continue;
      }
      let button = DemoButton.BUTTON_A;
      switch (action) {
        case 'LEFT':
          button = DemoButton.BUTTON_LEFT;
          break;
        case 'RIGHT':
          button = DemoButton.BUTTON_RIGHT;
          break;
        case 'ROTATE_C':
          button = DemoButton.BUTTON_A;
          break;
        case 'ROTATE_AC':
          button = DemoButton.BUTTON_B;
          break;
        default:
          throw new Error('no such button');
      }

      const frame = parseInt(frameStr, 10);
      const fr1 = frame;
      const fr2 = frame+1;
      demoEntries.push({
        frame: fr1,
        startFrame: fr1,
        button,
        isDown: true
      });
      demoEntries.push({
        frame: fr2,
        startFrame: fr1,
        button,
        isDown: false
      });
    }
    demoEntries.sort((a: DemoEntry, b: DemoEntry) => {
      return a.frame - b.frame;
    });
    return [demoEntries, lastFrame];
  }
}
