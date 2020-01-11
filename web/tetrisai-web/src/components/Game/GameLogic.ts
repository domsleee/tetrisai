import { EmulatorFactory } from './Emulator/EmulatorFactory';
import { DemoPlayer } from './DemoPlayer';
import { GameBootstrap } from './GameBootstrap';
import { GameRunnerFactory } from './GameRunnerFactory';
import { PixelChecker } from './PixelGetter';
import { PieceAwaiter } from './PieceAwaiter';
import { FrameAwaiter } from './FrameAwaiter';
import { DemoButton } from './IDemoPlayer';

export class GameLogic {
  public async run(screen: any, rom: any, tableBoard: any, debug: any) {
    debug['ok'] = "DFLSJFLSK";
    const factory = new EmulatorFactory(screen, rom);
    const nes = factory.getInstance();
    // @ts-ignore
    window['nes'] = nes;


    const demoPlayer = new DemoPlayer(nes);
    demoPlayer.addFrameListener((frame: number) => {
      if (frame == 1105) {
        //demoPlayer.timer.setFps(0.5);
      }
      debug['fps'] = demoPlayer.timer.getFps();
      debug['frame'] = frame.toString();
    });
    const bs = new GameBootstrap(demoPlayer);
    const frameAwaiter = new FrameAwaiter(demoPlayer);

    await bs.setupFromNewCanvas(frameAwaiter);

    console.log("setup??");

    const pixelChecker = new PixelChecker(screen);
    // @ts-ignore
    window['pixelChecker'] = pixelChecker;

    const gr = GameRunnerFactory.getInstance(demoPlayer, pixelChecker, debug);
    const pa = new PieceAwaiter(pixelChecker, demoPlayer, frameAwaiter);
    console.log('first piece appeared!!');
    gr.setTableBoard(tableBoard);
    await gr.onFirstPieceAppear();
    pa.init();

    let x = 500;
    while (x--) {
      await pa.awaitPiece();
      await gr.onNextPieceAppear();
    }
  }
}
