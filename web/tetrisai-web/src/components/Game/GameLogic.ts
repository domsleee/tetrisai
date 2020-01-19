import { EmulatorFactory } from './Emulator/EmulatorFactory';
import { DemoPlayer } from './DemoPlayer';
import { GameBootstrap } from './GameBootstrap';
import { GameRunnerFactory } from './GameRunnerFactory';
import { PixelChecker } from './PixelGetter';
import { PieceAwaiter } from './PieceAwaiter';
import { FrameAwaiter, IFrameAwaiter } from './FrameAwaiter';
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
  private readonly pa: PieceAwaiter;
  private readonly gr: GameRunner;
  private readonly screen: any;
  private readonly demoPlayer: IDemoPlayer;
  private readonly debug: any;
  private readonly emu: IEmulator;
  private readonly frameAwaiter: IFrameAwaiter;

  constructor(screen: any, rom: any, tableBoard: any, debug: any) {
    this.screen = screen;
    this.debug = debug;

    const factory = new EmulatorFactory(screen, rom);
    this.emu = factory.getInstance();
    this.demoPlayer = new DemoPlayer(this.emu);
    ErrorHandler.setup(this.demoPlayer); // todo: rename as register

    const pixelChecker = new PixelChecker(screen);
    this.frameAwaiter = new FrameAwaiter(this.demoPlayer);
    this.gr = GameRunnerFactory.getInstance(
      this.demoPlayer,
      pixelChecker,
      debug
    );
    this.gr.setTableBoard(tableBoard);
    this.pa = new PieceAwaiter(
      pixelChecker,
      this.demoPlayer,
      this.frameAwaiter
    );
  }

  public async run() {
    const bs = new GameBootstrap(this.demoPlayer);
    await bs.setupFromNewCanvas(this.frameAwaiter);
    this.addFluff();
    this.demoPlayer.addEvent({
      frame: this.demoPlayer.getFrame() + 1,
      button: DemoButton.BUTTON_LEFT,
      isDown: true
    });
    await this.gr.onFirstPieceAppear();
    this.pa.init();

    let x = 500;
    while (x--) {
      await this.considerCaptureAndRestore();
      await this.pa.awaitPiece();
      await this.gr.onNextPieceAppear();
    }
  }

  public capture(): string {
    return JSON.stringify({
      gr: this.gr.capture(),
      pa: this.pa.capture(),
      screen: this.screen.capture()
    });
  }

  public restoreFromCapture(capture: string) {
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
  private async considerCaptureAndRestore() {
    if (this.demoPlayer.getFrame() >= 1e9 * 21200) {
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

  private addFluff() {
    this.debug['ok'] = 'DFLSJFLSK';

    // @ts-ignore
    window['emu'] = this.emu;

    // @ts-ignore
    window['demoPlayer'] = this.demoPlayer;

    // @ts-ignore
    window['download_cap'] = () => {
      const cap = this.gr.capture();
      // @ts-ignore
      window['download']('my_download', cap);
    };

    this.demoPlayer.addFrameListener((frame: number) => {
      this.debug['fps'] = this.demoPlayer.timer.getFps();
      this.debug['frame'] = frame.toString();
    });
  }
}
