import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { IEmulator } from './Emulator/IEmulator';

type ListenerFn = ((frame: number) => void);

export class DemoPlayer implements IDemoPlayer {
  private nes: IEmulator;
  private events: DemoEntry[] = [];
  private eventIndex: number = 0;
  private timer: any;
  private latestFrame: number = 0;
  private frameListeners: Set<ListenerFn>;

  public constructor(nes: any) {
    this.nes = nes;
    this.frameListeners = new Set();

    // budget timer
    this.timer = setInterval(() => {
      this.tick();
    }, 1000.00 / 60);
    window.addEventListener('message', (e) => {
      if (e.data && typeof e.data === 'string' && e.data.match(/webpackHotUpdate/)) {
       clearInterval(this.timer);
      }
    });
  }

  public addEvent(entry: DemoEntry): void {
    this.events.push(entry);
    this.latestFrame = Math.max(this.latestFrame, entry.frame);
  }

  public addEvents(entries: DemoEntry[]): void {
    for (const event of entries) {
      this.addEvent(event);
    }
  }

  public destroy() {
    clearInterval(this.timer);
  }

  public getFrame(): number {
    return this.nes.getFrame();
  }

  public getLatestFrame(): number {
    return this.latestFrame;
  }

  public addFrameListener(fn: ((frame: number) => void)): void {
    this.frameListeners.add(fn);
  }

  public removeFrameListener(fn: ((frame: number) => void)): void {
    this.frameListeners.delete(fn);
  }

  private tick() {
    this.forwardEventsToEmulator();
    if (this.eventIndex < this.events.length) {
      this.nes.frame();
      for (const listener of this.frameListeners) { listener(this.nes.getFrame()); }
    }
  }

  private forwardEventsToEmulator() {
    let eventsProcessed = 0;
    while (this.eventIndex < this.events.length) {
      const currentEvent = this.events[this.eventIndex];
      if (currentEvent.frame <= this.nes.getFrame()) {
        this.processEvent(currentEvent);
        this.eventIndex++;
        eventsProcessed++;
      } else { break; }
    }
    return eventsProcessed;
  }

  private processEvent(event: DemoEntry) {
    if (event.isDown) {
      this.nes.buttonDown(event.button);
      console.log('buttonDown', event.button);
    } else {
      this.nes.buttonUp(event.button);
      // console.log('buttonUp', event.button);
    }
  }
}
