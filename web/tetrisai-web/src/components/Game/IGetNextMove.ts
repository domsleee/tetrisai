import { IBoard } from './common/Board';
import { Piece } from './common/Enums';
import { ExtraInformation } from './ExtraInformation';
import { DemoEntry } from './IDemoPlayer';

export type FirstMoveDirectionT = 'LEFT' | 'RIGHT' | 'NONE';
export interface OptionalNextMoveParams {
  firstMoveDirection?: FirstMoveDirectionT;
  totalLineClears?: number;
}
export interface IGetNextMove {
  getNextMoveEntries(board: IBoard, nextPiece: Piece, optional: OptionalNextMoveParams):
    Promise<[DemoEntry[], IBoard, ExtraInformation]>;
}
