import { EmulatorFactory } from './Emulator/EmulatorFactory';
import { DemoPlayer } from './DemoPlayer';
import { GameBootstrap } from './GameBootstrap';
import { GameRunnerFactory } from './GameRunnerFactory';
import { PixelChecker } from './PixelGetter';
import { PieceAwaiter } from './PieceAwaiter';
import { FrameAwaiter } from './FrameAwaiter';
import { DemoButton, IDemoPlayer } from './IDemoPlayer';
import { ErrorHandler } from './common/ErrorHandler';
import { GetOldCapture } from './GetOldCapture';
import { GameRunner } from './GameRunner';
import { IEmulator } from './Emulator/IEmulator';
import { ICapturable } from './ICapturable';

function download(filename: any, text: any) {
  const FileSaver = require('file-saver');
  const blob = new Blob([text], { type: 'text/plain;charset=utf-8' });
  FileSaver.saveAs(blob, filename);
}

export class GameLogic implements ICapturable<string> {
  private pa: PieceAwaiter | null = null;
  private gr: GameRunner | null = null;
  // todo: work out how to find capture and restoreFromCapture on screen
  private screen: any | null = null;
  private demoPlayer: IDemoPlayer | null = null;

  // todo: move to constructor
  public async run(screen: any, rom: any, tableBoard: any, debug: any) {
    const factory = new EmulatorFactory(screen, rom);
    const emu = factory.getInstance();
    const demoPlayer = new DemoPlayer(emu);
    this.demoPlayer = demoPlayer;
    ErrorHandler.setup(demoPlayer);
    const bs = new GameBootstrap(demoPlayer);
    const frameAwaiter = new FrameAwaiter(demoPlayer);
    await bs.setupFromNewCanvas(frameAwaiter);
    console.log('setup??');

    const pixelChecker = new PixelChecker(screen);
    const gr = GameRunnerFactory.getInstance(demoPlayer, pixelChecker, debug);
    this.addFluff(demoPlayer, emu, gr, debug);

    const pa = new PieceAwaiter(pixelChecker, demoPlayer, frameAwaiter);
    this.gr = gr;
    this.pa = pa;
    this.screen = screen;
    console.log('first piece appeared!!');
    gr.setTableBoard(tableBoard);
    demoPlayer.addEvent({
      frame: demoPlayer.getFrame() + 1,
      button: DemoButton.BUTTON_LEFT,
      isDown: true
    });
    await gr.onFirstPieceAppear();
    pa.init();

    let x = 500;
    while (x--) {
      await this.considerCaptureAndRestore(pa, gr, demoPlayer);
      await pa.awaitPiece();
      console.log('PIECE HERE');
      await gr.onNextPieceAppear();
    }
  }

  public capture(): string {
    if (this.gr == null || this.pa == null || this.screen == null) {
      throw ErrorHandler.fatal('invalid call');
    }
    return JSON.stringify({
      gr: this.gr.capture(),
      pa: this.pa.capture(),
      screen: this.screen.capture()
    });
  }

  public restoreFromCapture(capture: string) {
    if (this.gr == null || this.pa == null || this.demoPlayer == null) {
      throw ErrorHandler.fatal('invalid call');
    }
    const oldFps = this.demoPlayer.timer.getFps();
    this.demoPlayer.timer.setFps(0);
    console.log('no fps anymore...');
    const json = JSON.parse(capture);
    this.gr.restoreFromCapture(json.gr);
    this.pa.restoreFromCapture(json.pa);
    this.screen.restoreFromCapture(json.screen);
    this.pa.countDiff();
    this.demoPlayer.timer.setFps(oldFps);
    console.log('GAMELOGIC RESTORED');
  }

  static myfirst = true;
  public async considerCaptureAndRestore(
    pa: PieceAwaiter,
    gr: GameRunner,
    demoPlayer: IDemoPlayer
  ) {
    if (demoPlayer.getFrame() >= 1e9 * 21200) {
      download('new_transition', this.capture());
      throw ErrorHandler.fatal('end.');
    }
    if (GameLogic.myfirst) {
      console.log('RESTORING....');
      this.restoreFromCapture(
        await new GetOldCapture().getJustBeforeTransition()
      );
      GameLogic.myfirst = false;
    }
  }

  private addFluff(
    demoPlayer: IDemoPlayer,
    emu: IEmulator,
    gr: GameRunner,
    debug: any
  ) {
    debug['ok'] = 'DFLSJFLSK';

    // @ts-ignore
    window['emu'] = emu;

    // @ts-ignore
    window['demoPlayer'] = demoPlayer;

    // @ts-ignore
    window['download_cap'] = () => {
      const cap = gr.capture();
      // @ts-ignore
      window['download']('my_download', cap);
    };

    demoPlayer.addFrameListener((frame: number) => {
      debug['fps'] = demoPlayer.timer.getFps();
      debug['frame'] = frame.toString();
    });
  }
}
