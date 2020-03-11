export class FrameTimer {
  public onTick: (ignoreFreeze?: boolean) => void;
  private fps: number = 2000;
  private timer: any = null;
  private frozen: boolean = false;
  private stopped: boolean = false;

  public constructor(onTick: () => void) {
    this.onTick = (ignoreFreeze: boolean = false) => {
      if (this.stopped) return;
      if (this.timer && (ignoreFreeze || !this.frozen)) onTick();
    };
    this.setFps(this.fps);
  }

  public setFps(fps: number) {
    this.clearTimer();
    this.timer = setInterval(() => {
      this.onTick();
    }, 1000.0 / fps);
    this.fps = fps;
  }

  public getFps() {
    return this.fps;
  }

  public stop() {
    this.stopped = true;
    this.clearTimer();
  }

  public resume() {
    this.stopped = false;
    if (!this.timer) {
      this.setFps(this.fps);
    }
  }

  public isFrozen() {
    return this.frozen;
  }

  public toggleFreeze() {
    if (this.frozen) this.unfreeze();
    else this.freeze();
  }

  public freeze() {
    this.frozen = true;
  }

  public unfreeze() {
    this.frozen = false;
  }

  private clearTimer() {
    if (this.timer) {
      clearInterval(this.timer);
    }
    this.timer = null;
  }
}
