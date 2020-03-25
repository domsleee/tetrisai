import { IDemoPlayerObserver } from './IDemoPlayerObserver';
import { DemoEntry, DemoButton } from './IDemoPlayer';

export class DemoPlayerObserver implements IDemoPlayerObserver {
  public events: DemoEntry[] = [];
  public notes: { frame: number, note: string }[] = [];

  public observeEvent(demoEntry: DemoEntry): void {
    this.events.push(demoEntry);
  }
  public observeNote(frame:number, note: string): void {
    this.notes.push({frame, note});
  }

  public trimEvents() {
    let lastVal = -1;
    for (let i = this.events.length-1; i >= 0; i--) {
      if (this.events[i].button != DemoButton.BUTTON_SELECT) {
        lastVal = this.events[i].frame;
        break;
      }
    }
    if (lastVal == -1) return this.events;

    let myNewEvents = [];
    for (const event of this.events) {
      if (event.button != DemoButton.BUTTON_SELECT) {
        myNewEvents.push(event);
      }
      else if (event.frame >= lastVal) {
        myNewEvents.push(event);
      }
    }
    return myNewEvents;
  }
}
