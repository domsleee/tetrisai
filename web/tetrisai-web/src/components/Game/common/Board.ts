import BitSet from '@marsraptor/bitset';

export const NUM_COLUMNS = 10;
export const NUM_ROWS = 20;

export interface IBoard {
  vacant(r: number, c: number): boolean;
  getBitstring(): string;
}

export class Board implements IBoard {
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
