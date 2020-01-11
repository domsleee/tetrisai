import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { IEmulator } from './Emulator/IEmulator';
import SortedSet from 'sortedset';
import { ErrorHandler } from './common/ErrorHandler';

type ListenerFn = ((frame: number) => void);


export class FrameTimer {
  private fps: number = 60.0;
  private timer: any = null;
  private onTick: () => void;

  public constructor(onTick: (() => void)) {
    this.onTick = onTick;
    this.setFps(this.fps);
  }

  public setFps(fps: number) {
    if (this.timer) { clearInterval(this.timer); }
    this.timer = setInterval(() => {
      this.onTick();
    }, 1000.0 / fps);
    this.fps = fps;
  }

  public getFps() {
    return this.fps;
  }

  public stop() {
    if (this.timer === null) { return; }
    clearInterval(this.timer);
    this.timer = null;
  }
}

export class DemoPlayer implements IDemoPlayer {
  private nes: IEmulator;
  private events: any = new SortedSet([], (a: DemoEntry, b: DemoEntry) => {
    if (a.frame !== b.frame) { return a.frame - b.frame; }
    // up is preferred
    if (a.isDown !== b.isDown) { return (a.isDown ? 0 : 1) - (b.isDown ? 0 : 1); }
    if (a.button !== b.button) { return a.button - b.button; }
    return 0;
  });
  private lastNesJSONStr: any = null;
  public timer: FrameTimer;
  private latestFrame: number = 0;
  private frameListeners: Set<ListenerFn>;
  private buttonIsDown: any = {};

  public constructor(nes: any) {
    this.nes = nes;
    this.frameListeners = new Set();

    // budget timer
    this.timer = new FrameTimer(() => {
      this.tick();
    });
    window.addEventListener('message', (e) => {
      if (e.data && typeof e.data === 'string' && e.data.match(/webpackHotUpdate/)) {
       this.timer.stop();
      }
    });
  }

  public addEvent(entry: DemoEntry): void {
    this.events.add(entry);
    this.latestFrame = Math.max(this.latestFrame, entry.frame);
  }

  public addEvents(entries: DemoEntry[]): void {
    console.log("Adding entries", entries.length, entries);
    for (const event of entries) {
      this.addEvent(event);
    }
  }

  public destroy() {
    this.timer.stop();
  }

  public goBack(numFrames: number) {
    if (numFrames > 1) { throw new Error("not implemented"); };
    const jsonStr = JSON.parse(this.lastNesJSONStr);

    if (jsonStr === null) { throw new Error("cannot undo on first frame"); }
    this.nes.revertFromJSON(jsonStr, 1);
    this.lastNesJSONStr = null;
    this.tick();

  }

  public getFrame(): number {
    return this.nes.getFrame();
  }

  public getLatestFrame(): number {
    return this.latestFrame;
  }

  public isEmpty(): boolean {
    return this.events.size === 0;
  }

  public addFrameListener(fn: ((frame: number) => void)): void {
    this.frameListeners.add(fn);
  }

  public removeFrameListener(fn: ((frame: number) => void)): void {
    this.frameListeners.delete(fn);
  }

  private tick() {
    const eventsProcessed = this.forwardEventsToEmulator();
    if (!this.isEmpty() || eventsProcessed > 0) {
      //this.lastNesJSONStr = JSON.stringify(this.nes.toJSON());
      this.nes.frame();
      for (const listener of this.frameListeners) { listener(this.nes.getFrame()); }
    }
  }

  private forwardEventsToEmulator() {
    let eventsProcessed = 0;
    const toDelete: DemoEntry[] = [];
    for (const currentEvent of this.events) {
      // play BEFORE currentEvent.frame (atm its after)
      if (currentEvent.frame <= this.nes.getFrame()) {
        this.processEvent(currentEvent);
        eventsProcessed++;
        toDelete.push(currentEvent);
      } else { break; }
    }
    for (const event of toDelete) {
      this.events.delete(event);
    }
    return eventsProcessed;
  }

  private processEvent(event: DemoEntry) {
    //if (event.frame >= 490) { return; }
    if (event.isDown) {
      if (event.button in this.buttonIsDown && this.buttonIsDown[event.button]) {
        console.log(event);
        ErrorHandler.fatal(`button ${DemoButton[event.button]} was down, cannot go down again!`);
      }
      this.buttonIsDown[event.button] = true;
      this.nes.buttonDown(event.button);
      console.log(`${event.frame}: D: ${DemoButton[event.button]}`);
    } else {
      this.buttonIsDown[event.button] = false;
      this.nes.buttonUp(event.button);
      if (event.button != DemoButton.BUTTON_SELECT) { console.log(`${event.frame}: U: ${DemoButton[event.button]}`); }
    }
  }
}
