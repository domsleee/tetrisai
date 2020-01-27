export class FrameTimer {
  public onTick: () => void;
  private fps: number = 60.0;
  private timer: any = null;
  private frozen: boolean = false;

  public constructor(onTick: () => void) {
    this.onTick = () => {
      if (!this.frozen) onTick();
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

  public freeze() {
    this.frozen = true;
  }

  public unfreeze() {
    this.frozen = false;
  }
}
