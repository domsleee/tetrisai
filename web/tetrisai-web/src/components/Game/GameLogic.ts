import { EmulatorFactory } from './Emulator/EmulatorFactory';
import { DemoPlayer } from './DemoPlayer';
import { GameBootstrap } from './GameBootstrap';
import { GameRunnerFactory } from './GameRunnerFactory';
import { PixelChecker } from './PixelGetter';
import { PieceAwaiter } from './PieceAwaiter';
import { FrameAwaiter } from './FrameAwaiter';
import { DemoButton } from './IDemoPlayer';
import { ErrorHandler } from './common/ErrorHandler';

export class GameLogic {
  public async run(screen: any, rom: any, tableBoard: any, debug: any) {
    debug['ok'] = "DFLSJFLSK";
    const factory = new EmulatorFactory(screen, rom);
    const nes = factory.getInstance();
    // @ts-ignore
    window['nes'] = nes;

    const demoPlayer = new DemoPlayer(nes);
    ErrorHandler.setup(demoPlayer);
    demoPlayer.addFrameListener((frame: number) => {
      if (frame == 3396) {
        //demoPlayer.timer.setFps(0.33);
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
    demoPlayer.addEvent({
      frame: demoPlayer.getFrame() + 1,
      button: DemoButton.BUTTON_LEFT,
      isDown: true,
    });
    await gr.onFirstPieceAppear();
    pa.init();

    let x = 500;
    while (x--) {
      await pa.awaitPiece();
      await gr.onNextPieceAppear();
    }
  }
}
