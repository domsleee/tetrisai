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

    // 17 - 84k
    // 22 - trash
    // 23 - 355k

    // 21 -- good for 19
    // 22 - 346k @ 19
    const chosenFrame = 23;
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_A, frame: buttonAdder.frame + chosenFrame - 1, isDown: true});
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_START, frame: buttonAdder.frame + chosenFrame, isDown: true});
    const framesToSetup = 7;
    const dF = chosenFrame + framesToSetup;
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_A, frame: buttonAdder.frame + dF - 1, isDown: false});
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_START, frame: buttonAdder.frame + dF - 1, isDown: false});
    this.demoPlayer.addEvent({button: DemoButton.BUTTON_SELECT, frame: buttonAdder.frame + dF, isDown: false});

    await frameAwaiter.awaitFrame(buttonAdder.frame + dF);
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
