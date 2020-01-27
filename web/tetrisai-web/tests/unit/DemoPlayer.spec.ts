import { expect } from 'chai';
import { DemoEntry, DemoButton } from '@/components/Game/IDemoPlayer';
import * as TypeMoq from 'typemoq';

import { IEmulator } from '@/components/Game/Emulator/IEmulator';
import { DemoPlayer } from '@/components/Game/DemoPlayer';
import {
  getDemoEntry,
  getDemoEntryWithStartFrame
} from '@/components/Game/DemoEntryHelpers';

describe('DemoPlayer', async function() {
  it('adjacent order of events', async function() {
    // todo: refactor into class. Logic seems cooked, imo typemoq is limited af
    const emu = TypeMoq.Mock.ofType<IEmulator>();
    const buttonCalls: Array<{ button: DemoButton; isDown: boolean }> = [];
    let myEmuFrame = 0;

    emu
      .setup(myEmu => myEmu.buttonDown(TypeMoq.It.isAny()))
      .callback(button => {
        buttonCalls.push({ button, isDown: true });
      });
    emu
      .setup(myEmu => myEmu.buttonUp(TypeMoq.It.isAny()))
      .callback(button => {
        buttonCalls.push({ button, isDown: false });
      });
    emu.setup(myEmu => myEmu.frame()).callback(() => myEmuFrame++);
    emu.setup(myEmu => myEmu.getFrame()).returns(() => myEmuFrame);

    const entries: DemoEntry[] = [
      getDemoEntry(0, DemoButton.BUTTON_RIGHT, true),
      getDemoEntryWithStartFrame(1, 0, DemoButton.BUTTON_RIGHT, false),
      getDemoEntry(1, DemoButton.BUTTON_A, true),
      getDemoEntryWithStartFrame(2, 1, DemoButton.BUTTON_A, false)
    ];

    const demoPlayer = new DemoPlayer(emu.object);
    demoPlayer.addEvents(entries);

    for (let i = 0; i < 5; ++i) demoPlayer.timer.onTick();
    const expButtonCalls = [
      { button: DemoButton.BUTTON_RIGHT, isDown: true },
      { button: DemoButton.BUTTON_RIGHT, isDown: false },
      { button: DemoButton.BUTTON_A, isDown: true },
      { button: DemoButton.BUTTON_A, isDown: false }
    ];
    expect(buttonCalls[0]).to.eql(expButtonCalls[0]);
    expect(buttonCalls[1]).to.eql(expButtonCalls[1]);
    expect(buttonCalls[2]).to.eql(expButtonCalls[2]);
    expect(buttonCalls[3]).to.eql(expButtonCalls[3]);
  });
});
