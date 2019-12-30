import { IDemoPlayer } from './IDemoPlayer';

export interface IFrameAwaiter {
  awaitFrame(frame: number): Promise<void>;
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
}