import { DemoButton } from './IDemoPlayer';
import { DemoEntry } from './IDemoPlayer';

export function getDemoEntry(
  frame: number,
  button: DemoButton,
  isDown: boolean
): DemoEntry {
  return {
    frame,
    button,
    isDown,
    startFrame: frame
  };
}

export function getDemoEntryWithStartFrame(
  frame: number,
  startFrame: number,
  button: DemoButton,
  isDown: boolean
): DemoEntry {
  return {
    frame,
    startFrame,
    button,
    isDown
  };
}
