import { IGetNextMove } from './GameRunner';
import { DemoEntry, DemoButton } from './IDemoPlayer';
import { Piece } from './common/Enums';
import { Board, IBoard } from './common/Board';
import axios from 'axios';
import { ExtraInformation } from './ExtraInformation';

const URL = 'http://localhost:5000/get-moves';
export class GetNextMove implements IGetNextMove {
  public async getNextMoveEntries(board: IBoard, nextPiece: Piece): Promise<[DemoEntry[], IBoard, ExtraInformation]> {
    const resp = await axios.post(URL, {
      board: board.getBitstring(),
      piece: nextPiece.toString(),
    });
    const nxBoard = new Board(resp.data.board);
    let lastFrame = 0;
    const demoEntries = [];
    console.log("resp.data");
    console.log(resp.data);
    for (const str of resp.data.demo_entries) {
      let [frameStr, action, ...rest] = str.split(' ');
      lastFrame = parseInt(frameStr);
      if (action == 'DOWN') continue;
      let button = DemoButton.BUTTON_A;
      switch (action) {
        case 'LEFT': button = DemoButton.BUTTON_LEFT; break;
        case 'RIGHT': button = DemoButton.BUTTON_RIGHT; break;
        case 'ROTATE_C': button = DemoButton.BUTTON_A; break;
        case 'ROTATE_AC': button = DemoButton.BUTTON_B; break;
        default: throw new Error("no such button");
      }

      let frame = parseInt(frameStr);
      if (frame === 0) frame++;
      demoEntries.push({
        frame,
        button,
        isDown: true,
      });
      demoEntries.push({
        frame: frame + 1,
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