
import { expect } from 'chai';
import * as TypeMoq from 'typemoq';
import { MockScreen } from './MockScreen';
import { GameLogic, GameLogicMode } from '@/components/Game/GameLogic';
import { testRom } from '@/testData/testRom';
import { Features } from '@/components/Game/Features';



describe('GameLogic', async function() {
  this.timeout(300 * 1000);
  it('run an entire game', async function() {
    Features.adaptBasedOnNextPiece = true;
    const screen = new MockScreen();
    const gl = new GameLogic(screen, testRom, {}, {});
    const result = await gl.run(GameLogicMode.NORMAL, 1);
    expect(result).to.not.equal(undefined);
    expect(result!.lineClears).to.be.greaterThan(0);
    console.log(result);
  });
});

describe('GameLogic - adaptBasedOnNextPiece disabled', async function() {
  this.timeout(300 * 1000);
  it('run an entire game', async function() {
    Features.adaptBasedOnNextPiece = false;
    const screen = new MockScreen();
    const gl = new GameLogic(screen, testRom, {}, {});
    const result = await gl.run(GameLogicMode.NORMAL, 1);
    expect(result).to.not.equal(undefined);
    expect(result!.lineClears).to.be.greaterThan(0);
    console.log(result);
  });
});