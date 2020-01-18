import { DemoButton } from '../IDemoPlayer';
import { ICapturable } from '../ICapturable';

export interface IEmulator extends ICapturable<string> {
  frame(): void;
  getFrame(): number;
  buttonDown(button: DemoButton): void;
  buttonUp(button: DemoButton): void;
  revertFromJSON(jsonStr: any, numFrames: number): void;
}
