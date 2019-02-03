#ifndef __MIX_CHESS_TYPES_H__
#define __MIX_CHESS_TYPES_H__ 1

typedef enum { 
  EMPTY,
  W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, 
  B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING
} chess_figure_t;

typedef struct {
  int N;
  int x[64], y[64];
} move_t;



#endif /* __MIX_CHESS_TYPES_H__ */
