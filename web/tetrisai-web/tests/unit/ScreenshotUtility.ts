import { HEIGHT, WIDTH, PixelChecker, IPixelChecker } from '@/components/Game/PixelGetter';
import jimp from 'jimp';
const pkgDir = require('pkg-dir');
export const SCREENSHOTS_FOLDER = pkgDir.sync() + '/tests/data/screenshots';

export async function setupWithScreenshot(filepath: string): Promise<IPixelChecker> {
  const mockScreen = {
    buf32: new Uint32Array(HEIGHT * WIDTH),
  };
  const img = await jimp.read(filepath);
  for (let r = 0; r < HEIGHT; ++r) {
    for (let c = 0; c < WIDTH; ++c) {
      mockScreen.buf32[r * WIDTH + c] = img.getPixelColor(c, r);
    }
  }
  return new PixelChecker(mockScreen);
}
