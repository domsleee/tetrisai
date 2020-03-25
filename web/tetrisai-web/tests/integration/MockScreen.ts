import { ScreenT } from "@/types";
import { IScreen } from '@/components/Game/IScreen';

const SCREEN_HEIGHT = 240;
const SCREEN_WIDTH = 256;

export class MockScreen implements IScreen {
  private buffer = new ArrayBuffer(SCREEN_HEIGHT * SCREEN_WIDTH * 32);
  public screen: ScreenT = {
    buf32: new Uint32Array(this.buffer)
  }
  
  public setBuffer(buffer: any) {
    //console.log('setBuffer called.');
    let i = 0;
    for (let y = 0; y < SCREEN_HEIGHT; ++y) {
      for (let x = 0; x < SCREEN_WIDTH; ++x) {
        i = y * 256 + x;
        // Convert pixel from NES BGR to canvas ABGR
        this.screen.buf32[i] = 0xff000000 | buffer[i]; // Full alpha
      }
    }
  }

  public writeBuffer() {
    //this.imageData.data.set(this.buf8);
    //this.context.putImageData(this.imageData, 0, 0);
  }
}

