export class FrameTimer {
  public onTick: (force?: boolean) => void;
  private fps: number = 2000.0;
  private timer: any = null;
  private frozen: boolean = false;

  public constructor(onTick: () => void) {
    this.onTick = (force: boolean = false) => {
      if (force || !this.frozen) onTick();
    };
    this.setFps(this.fps);
  }

  public setFps(fps: number) {
    if (this.timer) {
      clearInterval(this.timer);
    }
    this.timer = setInterval(() => {
      this.onTick();
    }, 1000.0 / fps);
    this.fps = fps;
  }

  public getFps() {
    return this.fps;
  }

  public stop() {
    if (this.timer === null) {
      return;
    }
    clearInterval(this.timer);
    this.timer = null;
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
}
