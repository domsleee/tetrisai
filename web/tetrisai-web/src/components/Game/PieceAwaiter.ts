import { IPixelChecker } from './PixelGetter';
import { IDemoPlayer, DemoButton } from './IDemoPlayer';
import { IFrameAwaiter } from './FrameAwaiter';
import { ICapturable } from './ICapturable';
import { NUM_COLUMNS } from './common/Board';

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
    this.matrix = JSON.parse(capture.matrix);
    console.log('RESTORED MATRIX', this.matrix);
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

  public async awaitPiece() {
    console.log('awaiting piece...');
    let frame = this.demoPlayer.getFrame();
    let total = 0;
    while (true) {
      let diff = 0;
      let diffs = [];
      for (let r = this.rRange[0]; r <= this.rRange[1]; ++r) {
        const row = this.matrix[r];
        for (let c = this.cRange[0]; c <= this.cRange[1]; ++c) {
          const newV = this.pixelChecker.getPixel(r, c);
          if (newV !== row[c]) {
            diff++;
            diffs.push([r, c, newV, row[c]]);
          }
          row[c] = newV;
          total++;
        }
      }
      const greyPixelColour = this.pixelChecker.getPixel(
        PieceAwaiter.greyPixel[0],
        PieceAwaiter.greyPixel[1]
      );
      // console.log("grey pixel colour", greyPixelColour.toString(16));
      const isFlashing = greyPixelColour !== PieceAwaiter.GREY_PIXEL_COLOUR;
      const isTetrising = isFlashing || this.isTetrisingCooldown > 0;
      this.isTetrisingCooldown = Math.max(0, this.isTetrisingCooldown - 1);
      if (isFlashing) {
        this.isTetrisingCooldown = this.IS_TETRISING_COOLDOWN_MAX;
      }
      if (diff && !isTetrising) {
        console.log('diff!', diff);
        // this.demoPlayer.goBack(1);
        if (diff !== 196) break;
        if (diff === 196) {
          console.log(diffs);
          console.log(total);
          /*for (let diffInfo of diffs) {
            const r = diffInfo[0];
            const c = diffInfo[1];
            console.log(diffInfo, this.matrix[r][c], this.pixelChecker.getPixel(r, c));
          }*/
        }
      }
      if (this.demoPlayer.isEmpty()) {
        this.demoPlayer.addEvent({
          button: DemoButton.BUTTON_SELECT,
          frame: this.demoPlayer.getFrame(),
          isDown: false
        });
      }
      await this.frameAwaiter.awaitFrame(frame + 1);
      frame++;
    }
  }
}
