import { expect } from 'chai';
import jimp from 'jimp';
import { HEIGHT, WIDTH, PixelChecker } from '@/components/Game/PixelGetter';
import { ReadNextPiece } from '@/components/GameRunner/ReadNextPiece';
import { IReadNextPiece } from '@/components/Game/GameRunner';
import { Piece } from '@/components/Game/common/Enums';
import { itParam } from 'mocha-param';

// ffs.
const SCREENSHOTS_FOLDER = '/Users/dom/Documents/git/tetrisAI/web/tetrisai-web/tests/data/screenshots/';

async function setupWithScreenshot(filepath: string): Promise<IReadNextPiece> {
  const mockScreen = {
    buf32: new Uint32Array(HEIGHT * WIDTH),
  };
  const img = await jimp.read(filepath);
  for (let r = 0; r < HEIGHT; ++r) {
    for (let c = 0; c < WIDTH; ++c) {
      mockScreen.buf32[r * WIDTH + c] = img.getPixelColor(c, r);
    }
  }
  const pixelChecker = new PixelChecker(mockScreen);
  return new ReadNextPiece(pixelChecker);
}


describe('readnextpiece', async () => {
  const pieces = Object.keys(Piece).filter((k) => typeof Piece[k as any] === 'number');
  itParam('verify all pieces are correctly matching ${value}', pieces, async (piece: string) => {
    const transformFn = (s: string) => s[0] + '-' + s.slice(2).toLowerCase();
    const transformed = transformFn(piece);
    const readNextPiece: IReadNextPiece = await setupWithScreenshot(SCREENSHOTS_FOLDER + '/' + transformed + '.png');
    // @ts-ignore
    expect(readNextPiece.getCurrentPieceFromEmulator()).to.equal(Piece[piece]);
  });
});
