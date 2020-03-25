
import { DemoEntry } from './IDemoPlayer';

export interface IDemoPlayerObserver {
  observeEvent(demoEntry: DemoEntry): void;
  observeNote(frame:number, note: string): void;
};
