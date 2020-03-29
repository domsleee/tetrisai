
import { expect } from 'chai';
import * as TypeMoq from 'typemoq';
import { MockScreen } from '../integration/MockScreen';
import { GameLogic, GameLogicMode } from '@/components/Game/GameLogic';
import { testRom } from '@/testData/testRom';
import { Features } from '@/components/Game/Features';


async function getResults(numGames: number) {
  let results = [];
  for (let i = 1; i <= numGames; i++) {
    const screen = new MockScreen();
    const gl = new GameLogic(screen, testRom, {}, {});
    const result = await gl.run(GameLogicMode.NORMAL, i);
    console.log(result);
    results.push(result);
  }
  return results;
}

describe('Generate Report', async function() {
  this.timeout(50 * 100 * 600 * 1000);
  it('adaptBasedOnNextPiece = true', async function() {
    Features.adaptBasedOnNextPiece = true;
    const results = await getResults(250);
    console.log(results);
  });

  it('adaptBasedOnNextPiece = false', async function() {
    Features.adaptBasedOnNextPiece = false;
    const results = await getResults(25);
    console.log(results);
  });
});
