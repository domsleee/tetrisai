import { expect } from 'chai';
import { Piece } from '@/components/Game/common/Enums';
import { setupWithScreenshot, SCREENSHOTS_FOLDER } from './ScreenshotUtility';
import { IReadBoard, ReadBoard } from '@/components/GameRunner/ReadBoard';
import { shallowMount } from '@vue/test-utils';


// ffs.

describe('ReadBoard', async function() {
  it('Z-PIECE test', async function() {
    const file = SCREENSHOTS_FOLDER + '/starting/start-Z-piece.png';
    const readBoard: IReadBoard = new ReadBoard(await setupWithScreenshot(file));
    expect(readBoard.vacant(0, 3)).to.equal(true);
    expect(readBoard.vacant(0, 4)).to.equal(false);
    expect(readBoard.vacant(0, 5)).to.equal(false);
    expect(readBoard.vacant(0, 6)).to.equal(true);
    expect(readBoard.vacant(1, 5)).to.equal(false);
    expect(readBoard.vacant(1, 6)).to.equal(false);
  });
});
