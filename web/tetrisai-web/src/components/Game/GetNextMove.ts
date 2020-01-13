import { IGetNextMove, FirstMoveDirectionT } from './GameRunner';
import { DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { Board, IBoard } from './common/Board';
import axios from 'axios';
import { ExtraInformation } from './ExtraInformation';

const URL = 'http://localhost:5000/get-moves';
export class GetNextMove implements IGetNextMove {
  public async getNextMoveEntries(board: IBoard, nextPiece: Piece, firstMoveDirection?: FirstMoveDirectionT):
    Promise<[DemoEntry[], IBoard, ExtraInformation]> {
    const data: any = {
      board: board.getBitstring(),
      piece: nextPiece.toString(),
    };
    if (firstMoveDirection) { data.first_move_direction = firstMoveDirection; }
    const resp = await axios.post(URL, data);
    const nxBoard = new Board(resp.data.board);
    let lastFrame = 0;
    const demoEntries = [];
    console.log("resp.data");
    console.log(resp.data);
    if (firstMoveDirection) { resp.data.demo_entries = resp.data.demo_entries.slice(1); }
    for (const str of resp.data.demo_entries) {
      const [frameStr, action, ...rest] = str.split(' ');
      lastFrame = parseInt(frameStr, 10);
      if (action === 'DOWN') { continue; }
      let button = DemoButton.BUTTON_A;
      switch (action) {
        case 'LEFT': button = DemoButton.BUTTON_LEFT; break;
        case 'RIGHT': button = DemoButton.BUTTON_RIGHT; break;
        case 'ROTATE_C': button = DemoButton.BUTTON_A; break;
        case 'ROTATE_AC': button = DemoButton.BUTTON_B; break;
        default: throw new Error("no such button");
      }

      let frame = parseInt(frameStr);
      let fr1 = frame-1, fr2 = frame;
      if (frame > 1) { fr1 = frame - 3, fr2 = frame - 2; }
      demoEntries.push({
        frame: fr1,
        button,
        isDown: true,
      });
      demoEntries.push({
        frame: fr2,
        button,
        isDown: false,
      });
    }
    demoEntries.sort((a: DemoEntry, b: DemoEntry) => {
      return a.frame - b.frame;
    });
    return [demoEntries, nxBoard, {
      lastFrame,
    }];
  }
}