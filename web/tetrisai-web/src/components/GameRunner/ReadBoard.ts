import { IPixelChecker } from '../Game/PixelGetter';
import { NUM_ROWS, NUM_COLUMNS } from '../Game/common/Board';
import { ErrorHandler } from '../Game/common/ErrorHandler';


export interface IReadBoard {
  vacant(r: number, c: number): boolean;
}

export class ReadBoard implements IReadBoard {
  private readonly anchor = [48, 96];
  private readonly BLOCK_DIST = 8;
  private readonly WHITE = 0xffffffff;
  private readonly pixelChecker: IPixelChecker;

  constructor(pixelChecker: IPixelChecker) {
    this.pixelChecker = pixelChecker;
  }

  public vacant(r: number, c: number) {
    if (r < 0 || r >= NUM_ROWS) { ErrorHandler.fatal('invalid row'); }
    if (c < 0 || c >= NUM_COLUMNS) { ErrorHandler.fatal('invalid column'); }
    const nr = this.anchor[0] + this.BLOCK_DIST * r;
    const nc = this.anchor[1] + this.BLOCK_DIST * c;
    const pixelColour = this.pixelChecker.getPixel(nr, nc);
    return pixelColour !== this.WHITE;
  }
}
