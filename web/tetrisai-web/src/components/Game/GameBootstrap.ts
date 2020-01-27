import { IDemoPlayer, DemoEntry, DemoButton } from './IDemoPlayer';
import { IFrameAwaiter } from './FrameAwaiter';
import { getDemoEntry } from './DemoEntryHelpers';

export class GameBootstrap {
  private demoPlayer: IDemoPlayer;

  constructor(demoPlayer: IDemoPlayer) {
    this.demoPlayer = demoPlayer;
  }

  public async setupFromNewCanvas(frameAwaiter: IFrameAwaiter) {
    const buttonAdder = new ButtonAdder(this.demoPlayer);

    for (let i = 0; i < 3; ++i) {
      buttonAdder.addButtonPress(DemoButton.BUTTON_START);
    }
    for (let i = 0; i < 3; ++i) {
      buttonAdder.addButtonPress(DemoButton.BUTTON_RIGHT);
    }
    buttonAdder.addButtonPress(DemoButton.BUTTON_DOWN);

    // 17 - 84k
    // 22 - trash
    // 23 - 355k

    // 21 -- good for 19
    // 22 - 346k @ 19
    const chosenFrame = 23;
    this.demoPlayer.addEvent(
      getDemoEntry(
        buttonAdder.frame + chosenFrame - 1,
        DemoButton.BUTTON_A,
        true
      )
    );
    this.demoPlayer.addEvent(
      getDemoEntry(
        buttonAdder.frame + chosenFrame,
        DemoButton.BUTTON_START,
        true
      )
    );
    const framesToSetup = 7;
    const dF = chosenFrame + framesToSetup;
    this.demoPlayer.addEvent(
      getDemoEntry(buttonAdder.frame + dF - 1, DemoButton.BUTTON_A, false)
    );
    this.demoPlayer.addEvent(
      getDemoEntry(buttonAdder.frame + dF - 1, DemoButton.BUTTON_START, false)
    );
    this.demoPlayer.addEvent(
      getDemoEntry(buttonAdder.frame + dF, DemoButton.BUTTON_SELECT, false)
    );

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
    this.demoPlayer.addEvent(getDemoEntry(this.frame, button, true));
    this.demoPlayer.addEvent(getDemoEntry(this.frame + 1, button, false));
    this.frame += 5;
  }
}
