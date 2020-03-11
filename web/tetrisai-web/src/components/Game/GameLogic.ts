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
import { DemoPlayerObserver } from './DemoPlayerObserver';
import { get19_9HighRight } from './cases/events199HighRight';
import { getRightVsDown } from './cases/eventsRightVsDown';
import { start } from 'repl';
import { getDownVsRotate } from './cases/eventsDownVsRotate';
import { getSidewaysVsRotate } from './cases/sidewaysVsRotate';


export enum GameLogicMode {
  NORMAL,
  FREE_PLAY,
  PLAYBACK
};

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
  private onStop?: (() => void) = undefined;

  constructor(screen: any, rom: any, tableBoard: any, debug: any) {
    this.screen = screen;
    this.debug = debug;

    const factory = new EmulatorFactory(screen, rom);
    this.emu = factory.getInstance();
    this.demoPlayer = new DemoPlayer(this.emu);
    this.setupObserver(this.demoPlayer);
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

  public async run(gameLogicMode: GameLogicMode = GameLogicMode.NORMAL, startFrame: number = 1) {
    this.addFluff(startFrame);

    if (gameLogicMode === GameLogicMode.PLAYBACK) {
      return await this.doPlayback(get19_9HighRight());
      
    }

    const bs = new GameBootstrap(this.demoPlayer);
    await bs.setupFromNewCanvas(this.frameAwaiter, startFrame);

    this.demoPlayer.timer.setFps(2000);
    this.pa.init();

    if (gameLogicMode === GameLogicMode.FREE_PLAY) {
      this.activateKeyboardControls();
      while (true) await this.pa.awaitPiece();
      return;
    }

    await this.gr.onFirstPieceAppear();

    while (true) {
      await this.considerCaptureAndRestore();
      await this.pa.awaitPiece();
      let isGameOver = await this.gr.onNextPieceAppear();
      if (isGameOver) {
        return this.stop();
      }
    }
  }
  

  private async doPlayback(myEvents: any) {
    this.demoPlayer.addEvents(myEvents);
    this.demoPlayer.timer.setFps(2000);
    await this.frameAwaiter.awaitFrame(myEvents[myEvents.length-1].frame);
    this.demoPlayer.timer.setFps(60);
    this.demoPlayer.timer.freeze();
    this.activateKeyboardControls();
    this.pa.init();
    while (true) await this.pa.awaitPiece();
  }

  public capture(): string {
    return JSON.stringify({
      gr: this.gr.capture(),
      pa: this.pa.capture(),
      screen: this.screen.capture()
    });
  }

  public restoreFromCapture(capture: string) {
    this.demoPlayer.timer.stop();
    console.log('no fps anymore...');
    const json = JSON.parse(capture);
    this.gr.restoreFromCapture(json.gr);
    this.pa.restoreFromCapture(json.pa);
    this.screen.restoreFromCapture(json.screen);
    this.pa.countDiff();
    this.demoPlayer.timer.resume();
    console.log('GAMELOGIC RESTORED');
  }

  static myfirst = true;
  slowMoKeyHandler: any = null;


  activateSlowMoKeyboard = () => {
    this.slowMoKeyHandler = (e: KeyboardEvent) => {
      console.log('KEYPRESS', e.code);
      if (e.code === 'KeyK') {
        this.demoPlayer.timer.onTick(true);
      }
      else if (e.code === 'Space') {
        this.demoPlayer.timer.toggleFreeze();
      }
      else if (e.code === 'ShiftLeft') {
        this.demoPlayer.timer.setFps(15);
      }
      else if (e.code === 'ShiftRight') {
        this.demoPlayer.timer.setFps(2000);
      }
    };
    document.addEventListener('keydown', this.slowMoKeyHandler);
  }


  keyboardUpControls: any = null;
  keyboardDownControls: any = null;

  private activateKeyboardControls() {
    const keyMap: Record<string, DemoButton> = {
      'ArrowLeft': DemoButton.BUTTON_LEFT,
      'ArrowRight': DemoButton.BUTTON_RIGHT,
      'KeyA': DemoButton.BUTTON_B,
      'KeyS': DemoButton.BUTTON_A
      
    }
    this.keyboardDownControls = (e: KeyboardEvent) => {
      const currentFrame = this.demoPlayer.getFrame();
      if (e.code in keyMap) {
        this.demoPlayer.addEvent(
          getDemoEntry(currentFrame+1, keyMap[e.code], true)
        );
      }
    };
    this.keyboardUpControls = (e: KeyboardEvent) => {
      const currentFrame = this.demoPlayer.getFrame();
      if (e.code in keyMap) {
        this.demoPlayer.addEvent(
          getDemoEntry(currentFrame+1, keyMap[e.code], false)
        );
      }
    };

    document.addEventListener('keydown', this.keyboardDownControls);
    document.addEventListener('keyup', this.keyboardUpControls);
  }

  enterSloMo = () => {
    this.demoPlayer.timer.freeze();
    this.gr.disableFpsControl();
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
      //this.enterSloMo();
    }
  }

  private addFluff(startFrame: number) {
    this.debug['ok'] = 'DFLSJFLSK';
    this.debug['startFrame'] = startFrame;

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
      this.debug['keyboardControls'] = (this.keyboardDownControls ? true : false);
      this.debug['slowMo'] = (this.slowMoKeyHandler ? true : false)
    });
    

    this.activateSlowMoKeyboard();
  }

  private setupObserver(demoPlayer: IDemoPlayer) {
    const observer = new DemoPlayerObserver();
    // @ts-ignore
    window['observer'] = observer;
    demoPlayer.attachObserver(observer);
  }

  public setOnStop(fn: (() => void)) {
    this.onStop = fn;
  }

  public stop() {
    console.log("stop called");
    this.demoPlayer.destroy();
    if (this.onStop) this.onStop();
  }
}
