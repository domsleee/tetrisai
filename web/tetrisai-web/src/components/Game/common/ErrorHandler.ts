import { IDemoPlayer } from '../IDemoPlayer';

export class ErrorHandler {
  public static fatal(msg: string): never {
    if (this.demoPlayer !== null) {
      this.demoPlayer.timer.stop();
    }
    throw new Error(msg);
  }

  public static setup(demoPlayer: IDemoPlayer) {
    this.demoPlayer = demoPlayer;
  }

  private static demoPlayer: IDemoPlayer | null = null;
}
