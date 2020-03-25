import { ErrorHandler } from "./common/ErrorHandler";

const lineClearToPoints = [
  0,
  40,
  100,
  300,
  1200
];

export class ScoreManager {
  private numLines: number = 0;
  private level: number = 18;
  private score: number = 0;

  public addLines(numLines: number) {
    if (numLines > 4) throw ErrorHandler.fatal("invalid add lines");
    this.numLines += numLines;
    this.updateLevel();
    this.score += (this.level + 1) * lineClearToPoints[numLines];
  }

  public getScore() {
    return this.score;
  }

  public getLevel() {
    return this.level;
  }

  public getLineClears() {
    return this.numLines;
  }

  private updateLevel() {
    if (this.numLines >= 130) {
      this.level = 19 + Math.floor((this.numLines - 130)/10);
    } else {
      this.level = Math.max(this.level, 18);
    }
  }


}