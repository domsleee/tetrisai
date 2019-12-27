
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

export interface IDemoPlayer {
  addEvent(entry: DemoEntry): void;
  addEvents(entry: DemoEntry[]): void;
  destroy(): void;
}
