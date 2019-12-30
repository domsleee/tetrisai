import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { IFrameAwaiter } from './FrameAwaiter';


export class GameBootstrap {
  private demoPlayer: IDemoPlayer;

  constructor(demoPlayer: IDemoPlayer) {
    this.demoPlayer = demoPlayer;
  }

  public async setupFromNewCanvas(frameAwaiter: IFrameAwaiter) {
    const buttonAdder = new ButtonAdder(this.demoPlayer);

    for (let i = 0; i < 3; ++i) { buttonAdder.addButtonPress(DemoButton.BUTTON_START); }
    for (let i = 0; i < 3; ++i) { buttonAdder.addButtonPress(DemoButton.BUTTON_RIGHT); }
    buttonAdder.addButtonPress(DemoButton.BUTTON_DOWN);

    this.demoPlayer.addEvent({button: DemoButton.BUTTON_A, frame: buttonAdder.frame + 10, isDown: true});
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_START, frame: buttonAdder.frame + 15 + 2, isDown: true});
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_START, frame: buttonAdder.frame + 22, isDown: false});

    await frameAwaiter.awaitFrame(buttonAdder.frame + 22);
  }

  public setupFromGameover() {
    // todo.
    // this.setupFromNewCanvas();
  }
}


class ButtonAdder {
  public frame: number = 270;

  private demoPlayer: IDemoPlayer;
  constructor(demoPlayer: IDemoPlayer) {
    this.demoPlayer = demoPlayer;
  }

  public addButtonPress(button: DemoButton) {
    this.demoPlayer.addEvent({button, frame: this.frame, isDown: true});
    this.demoPlayer.addEvent({button, frame: this.frame + 1, isDown: false});
    this.frame += 5;
  }
}
