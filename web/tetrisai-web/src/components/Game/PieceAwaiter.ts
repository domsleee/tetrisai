import { IPixelChecker } from './PixelGetter';
import { IDemoPlayer, DemoButton, DemoPlayerCaptureLast } from './IDemoPlayer';
import { IFrameAwaiter } from './FrameAwaiter';
import { ICapturable } from './ICapturable';
import { NUM_COLUMNS } from './common/Board';
import { getDemoEntry } from './DemoEntryHelpers';
import { ErrorHandler } from './common/ErrorHandler';

import { default as loglevel } from 'loglevel';

const log = loglevel.getLogger('PieceAwaiter');

interface CaptureT {
  matrix: string;
  isTetrisingCooldown: number;
}

export class PieceAwaiter implements ICapturable<CaptureT> {
  private static readonly greyPixel = [49, 59];
  private static readonly GREY_PIXEL_COLOUR = 0xff525252;

  private readonly pixelChecker: IPixelChecker;
  private readonly demoPlayer: IDemoPlayer;
  private readonly frameAwaiter: IFrameAwaiter;

  private readonly rRange = [73, 204];
  private readonly cRange = [61, 89];
  private matrix: number[][] = [];

  private isTetrisingCooldown = 0;
  private readonly IS_TETRISING_COOLDOWN_MAX = 1;

  constructor(
    pixelChecker: IPixelChecker,
    demoPlayer: IDemoPlayer,
    frameAwaiter: IFrameAwaiter
  ) {
    this.pixelChecker = pixelChecker;
    this.demoPlayer = demoPlayer;
    this.frameAwaiter = frameAwaiter;
  }

  public capture(): CaptureT {
    return {
      matrix: JSON.stringify(this.matrix),
      isTetrisingCooldown: this.isTetrisingCooldown
    };
  }

  public restoreFromCapture(capture: CaptureT) {
    const matrix = JSON.parse(capture.matrix);
    if (matrix.length !== this.matrix.length) {
      throw new Error('invalid');
    }
    /*for (let i = 0; i < matrix.length; ++i) {
      const newRow = matrix[i];
      const row = this.matrix[i];
      if (row.length !== newRow.length) {
        throw new Error('invalid row');
      }
      for (const [j, _] of row.entries()) {
        row[j] = newRow[j];
      }
    }*/
    this.matrix = JSON.parse(capture.matrix);
    this.isTetrisingCooldown = capture.isTetrisingCooldown;
  }

  public init() {
    for (let r = this.rRange[0]; r <= this.rRange[1]; ++r) {
      while (this.matrix.length < r + 1) {
        this.matrix.push([]);
      }
      const row = this.matrix[r];
      for (let c = this.cRange[0]; c <= this.cRange[1]; ++c) {
        while (row.length < c + 1) {
          row.push(0);
        }
        row[c] = this.pixelChecker.getPixel(r, c);
      }
    }
  }

  public countDiff(mutate = true) {
    let diff = 0;
    let total = 0;
    for (let r = this.rRange[0]; r <= this.rRange[1]; ++r) {
      const row = this.matrix[r];
      for (let c = this.cRange[0]; c <= this.cRange[1]; ++c) {
        const newV = this.pixelChecker.getPixel(r, c);
        if (newV !== row[c]) diff++;
        if (mutate) row[c] = newV;
        total++;
      }
    }
    return [diff, total];
  }

  public async awaitPiece() {
    log.debug('awaiting piece...');
    let frame = this.demoPlayer.getFrame();
    while (true) {
      const [diff, total] = this.countDiff();
      const greyPixelColour = this.pixelChecker.getPixel(
        PieceAwaiter.greyPixel[0],
        PieceAwaiter.greyPixel[1]
      );
      const isFlashing = greyPixelColour !== PieceAwaiter.GREY_PIXEL_COLOUR;
      const isTetrising = isFlashing || this.isTetrisingCooldown > 0;
      this.isTetrisingCooldown = Math.max(0, this.isTetrisingCooldown - 1);
      if (isFlashing) {
        this.isTetrisingCooldown = this.IS_TETRISING_COOLDOWN_MAX;
      }
      if (diff > 0 && !isTetrising) {
        log.debug('diff!', diff);
        break;
      }
      if (this.demoPlayer.isEmpty()) {
        this.demoPlayer.addEvent(
          getDemoEntry(
            this.demoPlayer.getFrame(),
            DemoButton.BUTTON_SELECT,
            false
          )
        );
      }
      await this.frameAwaiter.awaitFrame(frame + 1);
      frame++;
    }
  }
}
