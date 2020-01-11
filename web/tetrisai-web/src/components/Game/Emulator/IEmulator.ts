import { DemoButton } from '../IDemoPlayer';

export interface IEmulator {
  frame(): void;
  getFrame(): number;
  buttonDown(button: DemoButton): void;
  buttonUp(button: DemoButton): void;
  revertFromJSON(jsonStr: any, numFrames: number): void;
  toJSON(): void;
}
