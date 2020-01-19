export class FrameTimer {
  private fps: number = 60.0;
  private timer: any = null;
  private onTick: () => void;

  public constructor(onTick: () => void) {
    this.onTick = onTick;
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
}
