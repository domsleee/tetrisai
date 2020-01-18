import { FrameTimer } from './DemoPlayer';
import { ICapturable } from './ICapturable';

// from jsnes controller constants
export enum DemoButton {
  BUTTON_A = 0,
  BUTTON_B = 1,
  BUTTON_SELECT = 2,
  BUTTON_START = 3,
  BUTTON_UP = 4,
  BUTTON_DOWN = 5,
  BUTTON_LEFT = 6,
  BUTTON_RIGHT = 7,
}

export interface DemoEntry {
  frame: number;
  button: DemoButton;
  isDown: boolean;
}

export interface IDemoPlayer extends ICapturable<string> {
  timer: FrameTimer;
  addEvent(entry: DemoEntry): void;
  addEvents(entry: DemoEntry[]): void;
  goBack(numFrames: number): void;
  destroy(): void;
  getFrame(): number;
  getLatestFrame(): number;
  isEmpty(): boolean;
  addFrameListener(fn: ((frame: number) => void)): void;
  removeFrameListener(fn: ((frame: number) => void)): void;
}
