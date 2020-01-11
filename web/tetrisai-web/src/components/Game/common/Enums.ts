
export enum Piece {
  I_PIECE = 'I_PIECE',
  O_PIECE = 'O_PIECE',
  L_PIECE = 'L_PIECE',
  J_PIECE = 'J_PIECE',
  T_PIECE = 'T_PIECE',
  Z_PIECE = 'Z_PIECE',
  S_PIECE = 'S_PIECE',
}

export const ALL_PIECE_KEYS: string[] =  Object.keys(Piece).filter((x) => !(parseInt(x, 10) >= 0));
export const ALL_PIECE_VALUES: Piece[] = ALL_PIECE_KEYS.map((k: string) => (Piece as any)[k]);
