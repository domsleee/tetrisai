import { DemoButton } from "../IDemoPlayer";
import { IEmulator } from './IEmulator';

export class Emulator implements IEmulator {
  private nes: any;
  private frameCt: number;

  constructor(nes: any) {
    this.nes = nes;
    this.frameCt = 0;
  }

  public frame() {
    this.nes.frame();
    this.frameCt++;
  }

  public getFrame() {
    return this.frameCt;
  }

  public buttonDown(button: DemoButton) {
    this.nes.buttonDown(1, button);
  }

  public buttonUp(button: DemoButton) {
    this.nes.buttonUp(1, button);
  }

  public revertFromJSON(jsonStr: any, numFrames: number) {
    this.nes.fromJSON(jsonStr);
    this.frameCt -= numFrames;
    console.log("GONE BACK. NEW FRAMECT", this.frameCt);
  }

  public toJSON() {
    return this.nes.toJSON();
  }
}
