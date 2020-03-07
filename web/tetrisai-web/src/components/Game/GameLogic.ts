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
import { getDemoEntry, getDemoEntryWithStartFrame } from './DemoEntryHelpers';

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

    const pixelChecker = new PixelChecker(screen.screen);
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

    //this.demoPlayer.addEvent(getDemoEntryWithStartFrame(this.demoPlayer.getFrame() + 1, 0, DemoButton.BUTTON_LEFT, true))
    this.pa.init();

    const press = (frame: number) => {
      this.demoPlayer.addEvent(getDemoEntryWithStartFrame(frame, 0, DemoButton.BUTTON_LEFT, true))
      this.demoPlayer.addEvent(getDemoEntryWithStartFrame(frame + 1, 0, DemoButton.BUTTON_LEFT, false))
    }

    press(465);
    press(494);

    this.enterSloMo();

    while (true) {
      await this.pa.awaitPiece();
    }

    await this.gr.onFirstPieceAppear();

    while (true) {
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
    this.demoPlayer.timer.stop();
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
  static slowMoKeyHandler: any = null;

  enterSloMo = () => {
    this.demoPlayer.timer.freeze();
    this.gr.disableFpsControl();
    if (GameLogic.slowMoKeyHandler) {
      document.removeEventListener('keydown', GameLogic.slowMoKeyHandler);
      GameLogic.slowMoKeyHandler = false;
    }
    GameLogic.slowMoKeyHandler = (e: KeyboardEvent) => {
      console.log('KEYPRESS', e.code);
      if (e.code === 'ArrowRight') {
        this.demoPlayer.timer.onTick(true);
      }
      if (e.code === 'Space') {
        this.demoPlayer.timer.toggleFreeze();
      }
    };
    document.addEventListener('keydown', GameLogic.slowMoKeyHandler);
  };

  private async considerCaptureAndRestore() {
    if (false && this.demoPlayer.getFrame() >= 2200) {
      download('problematic_placement', this.capture());
      throw ErrorHandler.fatal('end.');
    }
    if (false && GameLogic.myfirst) {
      console.log('RESTORING....');
      this.restoreFromCapture(
        await new GetOldCapture().getProblematicPlacement()
      );
      GameLogic.myfirst = false;
    }
    if (GameLogic.myfirst) {
      GameLogic.myfirst = false;
      //enterSloMo();
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
