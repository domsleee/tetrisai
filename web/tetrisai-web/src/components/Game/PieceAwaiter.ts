import { IPixelChecker } from './PixelGetter';
import { IDemoPlayer, DemoButton } from './IDemoPlayer';
import { IFrameAwaiter } from './FrameAwaiter';

export class PieceAwaiter {
  private readonly pixelChecker: IPixelChecker;
  private readonly demoPlayer: IDemoPlayer;
  private readonly frameAwaiter: IFrameAwaiter;

  private readonly rRange = [73, 204];
  private readonly cRange = [45, 89];
  private matrix: number[][] = [];

  constructor(pixelChecker: IPixelChecker, demoPlayer: IDemoPlayer, frameAwaiter: IFrameAwaiter) {
    this.pixelChecker = pixelChecker;
    this.demoPlayer = demoPlayer;
    this.frameAwaiter = frameAwaiter;
  }

  public init() {
    for (let r = this.rRange[0]; r <= this.rRange[1]; ++r) {
      while (this.matrix.length < r + 1) { this.matrix.push([]); }
      const row = this.matrix[r];
      for (let c = this.cRange[0]; c <= this.cRange[1]; ++c) {
        while (row.length < c + 1) { row.push(0); }
        row[c] = this.pixelChecker.getPixel(r, c);
      }
    }
  }

  public async awaitPiece() {
    console.log("awaiting piece...");
    let frame = this.demoPlayer.getFrame();
    while (true) {
      let diff = false;
      for (let r = this.rRange[0]; r <= this.rRange[1]; ++r) {
        const row = this.matrix[r];
        for (let c = this.cRange[0]; c <= this.cRange[1]; ++c) {
          const newV = this.pixelChecker.getPixel(r, c);
          if (newV !== row[c]) { diff = true; }
          row[c] = newV;
        }
      }
      if (diff) {
        console.log('diff!');
        this.demoPlayer.goBack(1);
        break;
      }
      if (this.demoPlayer.isEmpty()) {
        this.demoPlayer.addEvent({
          button: DemoButton.BUTTON_SELECT,
          frame: this.demoPlayer.getFrame(),
          isDown: false,
        });
      }
      await this.frameAwaiter.awaitFrame(frame + 1);
      frame++;
    }
  }
}