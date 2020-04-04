
import { expect } from 'chai';
import * as TypeMoq from 'typemoq';
import { MockScreen } from '../integration/MockScreen';
import { GameLogic, GameLogicMode } from '@/components/Game/GameLogic';
import { testRom } from '@/testData/testRom';
import { Features } from '@/components/Game/Features';
import * as fs from 'fs-extra';
import { GameRunnerResult } from '@/components/Game/GameRunner';



describe('Generate Report', async function() {
  this.timeout(50 * 100 * 600 * 1000);
  it('adaptBasedOnNextPiece = true', async function() {
    return;
    Features.adaptBasedOnNextPiece = true;
    const results = await getResults(250, 'adapt');
    console.log(results);
  });

  it('adaptBasedOnNextPiece = false', async function() {
    Features.adaptBasedOnNextPiece = false;
    const results = await getResults(25, 'dont');
    console.log(results);
  });
});


async function getResults(numGames: number, filename: string) {
  let results = [];
  const csvFd = await fs.open(filename+'.unsorted', 'a');
  fs.write(csvFd, 'lineClears,score,level\n');
  for (let i = 1; i <= numGames; i++) {
    const screen = new MockScreen();
    const gl = new GameLogic(screen, testRom, {}, {});
    const result = await gl.run(GameLogicMode.NORMAL, i);
    if (result === undefined) throw new Error("ree");
    console.log(result);
    results.push(result);
    if (result != null) {
      fs.write(csvFd, `${result.lineClears},${result.score},${result.level}\n`)
    }
    if (i === 1) {
      const piecesFd = await fs.open(filename+'.pieces', 'w');
      fs.writeSync(piecesFd, `score: ${result.score}\n`);
      fs.writeSync(piecesFd, `numPieces: ${result.pieces.length}\n`);
      for (const piece of result.pieces) {
        fs.writeSync(piecesFd, `${piece}\n`)
      }
    }
  }
  const csvSortedFd = await fs.open(filename, 'a');
  const fn = (g1: GameRunnerResult | undefined, g2: GameRunnerResult | undefined) => {
    if (g1 == undefined || g2 == undefined) return 0;
    return g1.score - g2.score;
  }
  results = results.sort(fn);
  fs.write(csvSortedFd, 'lineClears,score,level\n');
  for (const result of results) {
    if (result == undefined) continue;
    fs.write(csvSortedFd, `${result.lineClears},${result.score},${result.level}\n`);
  }

  return results;
}
