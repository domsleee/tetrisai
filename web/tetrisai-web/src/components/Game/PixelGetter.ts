export const WIDTH = 256;
export const HEIGHT = 240;

export class PixelChecker implements IPixelChecker {
  private screen: any;

  constructor(screen: any) {
    this.screen = screen;
  }

  public getPixel(r: number, c: number) {
    return this.screen.buf32[r * WIDTH + c];
  }
}

export interface IPixelChecker {
  getPixel(r: number, c: number): number;
}
