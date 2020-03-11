import BitSet from '@marsraptor/bitset';
import { IReadBoard } from '@/components/GameRunner/ReadBoard';
import { Coord } from '@/components/GameRunner/PieceCoords';

export const NUM_COLUMNS = 10;
export const NUM_ROWS = 20;

export interface IBoard {
  vacant(r: number, c: number): boolean;
  getBitstring(): string;
}

export class Board implements IBoard {
  public static fromNormalBoard(vs: string[]): Board {
    const bitstring = vs.join('');
    return new Board(bitstring);
  }

  public static fromBoardReader(boardReader: IReadBoard): Board {
    return Board.fromBoardReaderMinusCoords(boardReader, []);
  }

  public static fromBoardReaderMinusCoords(boardReader: IReadBoard, coords: Coord[]): Board {
    let str = "";
    let ignore: Record<number, Set<number>> = {};
    for (let coord of coords) {
      if (!(coord.r in ignore)) {
        ignore[coord.r] = new Set();
      }
      ignore[coord.r].add(coord.c);
    }

    for (let r = 0; r < NUM_ROWS; r++) {
      for (let c = 0; c < NUM_COLUMNS; ++c) {
        if (r in ignore && ignore[r].has(c)) {
          str += "0";
        } else str += boardReader.vacant(r, c) ? "0" : "1";
      }
    }
    return new Board(str);
  }

  private b: BitSet = new BitSet(200);
  private originalString: string = "0".repeat(200);

  public constructor(bitstring?: string) {
    if (!bitstring) {
      return;
    }
    if (bitstring.length !== 200) {
      throw new Error('bitstring must be of length 200');
    }
    for (let i = 0; i < 200; ++i) {
      this.b.set(i, bitstring[i] === '1' ? 1 : 0);
    }
    this.originalString = bitstring;
  }

  public getBitstring(): string {
    return this.originalString;
  }

  public vacant(r: number, c: number): boolean {
    if (r < 0 || r >= NUM_ROWS) {
      throw new Error('row out of range');
    }
    if (c < 0 || c >= NUM_COLUMNS) {
      throw new Error('column out of range');
    }
    return this.b.get((r * NUM_COLUMNS) + c) === 1;
  }
}