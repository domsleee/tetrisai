import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';

export class DemoPlayer implements IDemoPlayer {
  private nes: any;
  private events: DemoEntry[] = [];
  private eventIndex: number = 0;
  private frame: number = 0;
  private timer: any;

  public constructor(nes: any) {
    this.nes = nes;

    // budget timer
    this.timer = setInterval(() => {
      this.tick();
    }, 1000.00 / 60);
  }

  public addEvent(entry: DemoEntry): void {
    this.events.push(entry);
  }

  public addEvents(entries: DemoEntry[]): void {
    for (const event of entries) {
      this.addEvent(event);
    }
  }

  public destroy() {
    clearInterval(this.timer);
  }

  private tick() {
    this.forwardEventsToEmulator();
    if (this.eventIndex < this.events.length) {
      this.frame++;
      this.nes.frame();
    }
  }

  private forwardEventsToEmulator() {
    let eventsProcessed = 0;
    while (this.eventIndex < this.events.length) {
      const currentEvent = this.events[this.eventIndex];
      if (currentEvent.frame <= this.frame) {
        this.processEvent(currentEvent);
        this.eventIndex++;
        eventsProcessed++;
      } else { break; }
    }
    return eventsProcessed;
  }

  private processEvent(event: DemoEntry) {
    if (event.isDown) {
      this.nes.buttonDown(1, event.button);
      console.log('buttonDown', event.button);
    } else {
      this.nes.buttonUp(1, event.button);
      console.log('buttonUp', event.button);
    }
  }
}
