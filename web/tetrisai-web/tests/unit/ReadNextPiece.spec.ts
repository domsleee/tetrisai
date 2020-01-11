import { expect } from 'chai';
import { ReadNextPiece } from '@/components/GameRunner/ReadNextPiece';
import { IReadNextPiece } from '@/components/Game/GameRunner';
import { Piece } from '@/components/Game/common/Enums';
import { itParam } from 'mocha-param';
import { setupWithScreenshot, SCREENSHOTS_FOLDER } from './ScreenshotUtility';


describe('ReadNextPiece', async function() {
  const pieces = Object.keys(Piece).filter((x) => !(parseInt(x, 10) >= 0));
  itParam('verify all pieces are correctly matching ${value}', pieces, async function(piece: string) {
    const transformFn = (s: string) => s[0] + '-' + s.slice(2).toLowerCase();
    const transformed = transformFn(piece);
    const file = SCREENSHOTS_FOLDER + '/' + transformed + '.png';
    const readNextPiece: IReadNextPiece = new ReadNextPiece(await setupWithScreenshot(file));
    // @ts-ignore
    expect(readNextPiece.getCurrentPieceFromEmulator()).to.equal(Piece[piece]);
  });
});
