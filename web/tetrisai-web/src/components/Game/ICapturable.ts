export interface ICapturable<T> {
  capture(): T;
  restoreFromCapture(capture: T): void;
}
