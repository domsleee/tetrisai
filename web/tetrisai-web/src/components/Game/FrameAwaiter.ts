import { IDemoPlayer, DemoButton } from './IDemoPlayer';
import { getDemoEntry } from './DemoEntryHelpers';

export interface IFrameAwaiter {
  awaitFrame(frame: number): Promise<void>;
  awaitFrameForced(frame: number): Promise<void>;
}

export class FrameAwaiter implements IFrameAwaiter {
  demoPlayer: IDemoPlayer;
  constructor(demoPlayer: IDemoPlayer) {
    this.demoPlayer = demoPlayer;
  }

  public async awaitFrame(frame: number): Promise<void> {
    return new Promise((resolve) => {
      const fn = (myframe: number) => {
        if (myframe >= frame) {
          resolve();
          this.demoPlayer.removeFrameListener(fn);
        }
      }
      this.demoPlayer.addFrameListener(fn);
    });
  }

  public async awaitFrameForced(frame: number): Promise<void> {
    const pr = this.awaitFrame(frame);
    this.demoPlayer.addEvent(
      getDemoEntry(
        frame-1,
        DemoButton.BUTTON_SELECT,
        false
      )
    );
    return await pr;
  }
}