import { DemoButton } from '../IDemoPlayer';

export interface IEmulator {
  frame(): void;
  getFrame(): number;
  buttonDown(button: DemoButton): void;
  buttonUp(button: DemoButton): void;
}
