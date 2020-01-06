import { EmulatorFactory } from './Emulator/EmulatorFactory';
import { DemoPlayer } from './DemoPlayer';
import { GameBootstrap } from './GameBootstrap';
import { GameRunnerFactory } from './GameRunnerFactory';
import { PixelChecker } from './PixelGetter';
import { PieceAwaiter } from './PieceAwaiter';
import { FrameAwaiter } from './FrameAwaiter';

export class GameLogic {
  public async run(screen: any, rom: any) {
    const factory = new EmulatorFactory(screen, rom);
    const nes = factory.getInstance();
    // @ts-ignore
    window['nes'] = nes;

    const demoPlayer = new DemoPlayer(nes);
    const bs = new GameBootstrap(demoPlayer);
    const frameAwaiter = new FrameAwaiter(demoPlayer);

    await bs.setupFromNewCanvas(frameAwaiter);

    const pixelChecker = new PixelChecker(screen);
    // @ts-ignore
    window['pixelChecker'] = pixelChecker;

    const gr = GameRunnerFactory.getInstance(demoPlayer, pixelChecker);
    const pa = new PieceAwaiter(pixelChecker, demoPlayer, frameAwaiter);
    console.log('first piece appeared!!');
    await gr.onFirstPieceAppear();
    pa.init();

    let x = 15;
    while (x--) {
      await pa.awaitPiece();
      await gr.onNextPieceAppear();
    }
  }
}
