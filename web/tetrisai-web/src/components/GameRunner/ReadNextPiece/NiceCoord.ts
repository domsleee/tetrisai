export class NiceCoord {
  public readonly r: number;
  public readonly c: number;

  constructor(r: number, c: number) {
    this.r = r;
    this.c = c;
  }

  public move(dr: number, dc: number) {
    return new NiceCoord(this.r + 8 * dr, this.c + 8 * dc);
  }
}
