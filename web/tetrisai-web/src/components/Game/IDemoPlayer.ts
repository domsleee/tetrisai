import { ICapturable } from './ICapturable';
import { FrameTimer } from './FrameTimer';
import { IDemoPlayerObserver } from './IDemoPlayerObserver';
import { EmulatorCaptureLast } from './Emulator/IEmulator';


// from jsnes controller constants
export enum DemoButton {
  BUTTON_A = 0,
  BUTTON_B = 1,
  BUTTON_SELECT = 2,
  BUTTON_START = 3,
  BUTTON_UP = 4,
  BUTTON_DOWN = 5,
  BUTTON_LEFT = 6,
  BUTTON_RIGHT = 7
}

export interface DemoEntry {
  frame: number;
  startFrame: number;
  button: DemoButton;
  isDown: boolean;
}

export interface DemoPlayerCaptureLast {
  emu: EmulatorCaptureLast;
};

export interface IDemoPlayer extends ICapturable<string> {
  timer: FrameTimer;
  addEvent(entry: DemoEntry): void;
  addEvents(entry: DemoEntry[]): void;
  getEventsRep(): DemoEntry[];
  deleteAll(): void;
  deleteAllExceptFirstN(amount?: number): void;
  deleteEvents(entries: DemoEntry[]): void;
  deleteAllExcept(events: DemoEntry[]): void;
  goBack(numFrames: number): void;
  destroy(): void;
  getFrame(): number;
  getLatestFrame(): number;
  isEmpty(): boolean;
  addFrameListener(fn: (frame: number) => void): void;
  removeFrameListener(fn: (frame: number) => void): void;
  attachObserver(observer: IDemoPlayerObserver): void;

  captureLast(): DemoPlayerCaptureLast;
  restoreLast(a: DemoPlayerCaptureLast): void;
}
