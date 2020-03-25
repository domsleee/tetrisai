import { ScreenT } from '@/types';

export interface IScreen {
  setBuffer(buffer: any): void;
  writeBuffer(): void;
  screen: ScreenT;
}
