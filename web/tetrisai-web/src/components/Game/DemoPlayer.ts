import { IDemoPlayer, DemoEntry, DemoButton, DemoPlayerCaptureLast } from './IDemoPlayer';
import { IEmulator, EmulatorCaptureLast } from './Emulator/IEmulator';
import SortedSet from 'sortedset';
import { ErrorHandler } from './common/ErrorHandler';
import { FrameTimer } from './FrameTimer';
import { IDemoPlayerObserver } from './IDemoPlayerObserver';

type ListenerFn = (frame: number) => void;

interface WrappedDemoEntry {
  entry: DemoEntry;
  startFrame: number;
}

export class DemoPlayer implements IDemoPlayer {
  public timer: FrameTimer;
  private emu: IEmulator;
  private events: any = new SortedSet([], (a: DemoEntry, b: DemoEntry) => {
    if (a.frame !== b.frame) {
      return a.frame - b.frame;
    }
    if (a.startFrame !== b.startFrame) {
      return a.startFrame - b.startFrame;
    }
    // up is preferred
    if (a.isDown !== b.isDown) {
      const boolOrder = (b: boolean) => (b ? 1 : 0);
      return boolOrder(a.isDown) - boolOrder(b.isDown);
    }
    // higher button (e.g. LEFT/RIGHT preferred)
    if (a.button !== b.button) {
      return -(a.button - b.button);
    }
    return 0;
  });
  private lastNesJSONStr: any = null;
  private latestFrame: number = 0;
  private frameListeners: Set<ListenerFn>;
  private buttonIsDown: any = {};
  private observers: IDemoPlayerObserver[] = [];

  public constructor(emu: any) {
    this.emu = emu;
    this.frameListeners = new Set();

    // budget timer
    this.timer = new FrameTimer(() => {
      this.tick();
    });
    window.addEventListener('message', (e: any) => {
      if (
        e.data &&
        typeof e.data === 'string' &&
        e.data.match(/webpackHotUpdate/)
      ) {
        this.timer.stop();
      }
    });
  }

  public addEvent(entry: DemoEntry): void {
    this.events.add(entry);
    if (entry.frame === 1409) {
      console.log(entry);
      // return ErrorHandler.fatal("what");
    }
    this.latestFrame = Math.max(this.latestFrame, entry.frame);
  }

  public addEvents(entries: DemoEntry[]): void {
    console.log('Adding entries', entries.length, entries);
    for (const event of entries) {
      this.addEvent(event);
    }
    this.printEvents();
  }

  public deleteAll() {
    this.deleteAllExceptFirstN(0);
  }

  public deleteAllExceptFirstN(amount?: number) {
    console.log('CLEARNING EVENTS', this.getFrame());
    console.log('before...');
    this.printEvents();
    if (amount === undefined) this.events.clear();
    else {
      const toDelete = [];
      for (const event of this.events) {
        if (amount-- <= 0) toDelete.push(event);
      }
      for (const event of toDelete) this.events.delete(event);
    }
    console.log('after...');
    this.printEvents();
  }

  public deleteEvents(events: DemoEntry[]) {
    for (const event of events) {
      this.events.delete(event);
    }
  }

  public deleteAllExcept(events: DemoEntry[]) {
    let toDelete = [];
    let s = new Set<DemoEntry>();
    for (const event of events) s.add(event);

    for (const event of this.events) {
      if (!s.has(event)) {
        toDelete.push(event);
      }
    }
    this.deleteEvents(toDelete);
  }

  public getEventsRep() {
    let ret: DemoEntry[] = [];
    for (let event of this.events) {
      ret.push(event);
    }
    return ret;
  }

  public destroy() {
    this.timer.stop();
  }

  public goBack(numFrames: number) {
    if (numFrames > 1) {
      throw new Error('not implemented');
    }
    const jsonStr = JSON.parse(this.lastNesJSONStr);

    if (jsonStr === null) {
      throw new Error('cannot undo on first frame');
    }
    this.emu.revertFromJSON(jsonStr, 1);
    this.lastNesJSONStr = null;
    this.tick();
  }

  public getFrame(): number {
    return this.emu.getFrame();
  }

  public getLatestFrame(): number {
    return this.latestFrame;
  }

  public isEmpty(): boolean {
    return this.events.size === 0;
  }

  public addFrameListener(fn: (frame: number) => void): void {
    this.frameListeners.add(fn);
  }

  public removeFrameListener(fn: (frame: number) => void): void {
    this.frameListeners.delete(fn);
  }

  public captureLast(): DemoPlayerCaptureLast {
    return {
      emu: this.emu.captureLast()
    }
  }

  public restoreLast(a: DemoPlayerCaptureLast) {
    console.log(this.emu.getFrame());
    this.emu.restoreLast(a.emu);
    console.log(this.emu.getFrame());
    console.log("^restoreLast");
  }

  public capture(): string {
    const eventsArr: DemoEntry[] = [];
    for (const event of this.events) {
      eventsArr.push(event);
    }
    const myjson = {
      version: 2,
      eventsArr: JSON.stringify(eventsArr),
      emu: this.emu.capture(),
      buttonIsDown: this.buttonIsDown
    };
    return JSON.stringify(myjson);
  }

  public restoreFromCapture(capture: string) {
    const myjson = JSON.parse(capture);
    if (!('version' in myjson)) {
      return ErrorHandler.fatal('no version in demoplayer capture');
    }

    const version = myjson.version;
    switch (version) {
      case 2:
        {
          this.emu.restoreFromCapture(myjson.emu);
          this.buttonIsDown = myjson.buttonIsDown;
          this.events.clear();
          this.addEvents(JSON.parse(myjson.eventsArr));
        }
        break;
      default:
        throw new Error('unknown capture version');
    }
  }

  private tick() {
    const eventsProcessed = this.forwardEventsToEmulator();
    if (!this.isEmpty() || eventsProcessed > 0) {
      this.emu.frame();
      for (const listener of this.frameListeners) {
        listener(this.emu.getFrame());
      }
    }
  }

  private forwardEventsToEmulator() {
    let eventsProcessed = 0;
    const toDelete: DemoEntry[] = [];
    for (const currentEvent of this.events) {
      // play BEFORE currentEvent.frame (atm its after)
      if (currentEvent.frame <= this.emu.getFrame()) {
        this.processEvent(currentEvent);
        eventsProcessed++;
        toDelete.push(currentEvent);
      } else {
        break;
      }
    }
    for (const event of toDelete) {
      this.events.delete(event);
    }
    return eventsProcessed;
  }

  private processEvent(event: DemoEntry) {
    if (event.isDown) {
      if (
        event.button in this.buttonIsDown &&
        this.buttonIsDown[event.button]
      ) {
        console.log(event);
        console.warn(
          `button ${
            DemoButton[event.button]
          } was down, cannot go down again! (frame: ${this.getFrame()})`
        );
        this.buttonIsDown[event.button] = false;
        this.emu.buttonUp(event.button);
        for (const observer of this.observers) observer.observeNote(event.frame, "cannot go down again" + event.button);
        return;
      }
      this.buttonIsDown[event.button] = true;
      this.emu.buttonDown(event.button);
      // console.log(`${event.frame}: D: ${DemoButton[event.button]}`);
    } else {
      this.buttonIsDown[event.button] = false;
      this.emu.buttonUp(event.button);
      // if (event.button !== DemoButton.BUTTON_SELECT) { console.log(`${event.frame}: U: ${DemoButton[event.button]}`); }
    }
    for (const observer of this.observers) observer.observeEvent(event);

  }

  public attachObserver(observer: IDemoPlayerObserver) {
    this.observers.push(observer);
  }

  private printEvents() {
    const arr = [];
    for (const event of this.events) arr.push(event);
    console.log(arr);
  }
}
