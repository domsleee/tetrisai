import BitSet from '@marsraptor/bitset';
import { IReadBoard } from '@/components/GameRunner/ReadBoard';
import { Coord } from '@/components/GameRunner/PieceCoords';

export const NUM_COLUMNS = 10;
export const NUM_ROWS = 22;

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
    console.log(coords);
    for (let coord of coords) {
      const actR = coord.r - 2;
      if (!(actR in ignore)) {
        ignore[actR] = new Set();
      }
      ignore[actR].add(coord.c);
    }

    for (let r = 0; r < NUM_ROWS; r++) {
      for (let c = 0; c < NUM_COLUMNS; ++c) {
        if (r in ignore && ignore[r].has(c)) {
          str += "0";
        } else str += boardReader.vacant(r, c) ? "0" : "1";
      }
    }
    return new Board(str.slice(-20) + str.slice(0, -20));
  }

  private b: BitSet = new BitSet(NUM_ROWS * NUM_COLUMNS);
  private originalString: string = "0".repeat(NUM_ROWS * NUM_COLUMNS);

  public constructor(bitstring?: string) {
    if (!bitstring) {
      return;
    }
    if (bitstring.length !== NUM_ROWS * NUM_COLUMNS) {
      throw new Error(`bitstring must be of length ${NUM_ROWS * NUM_COLUMNS}`);
    }
    for (let i = 0; i < NUM_ROWS * NUM_COLUMNS; ++i) {
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