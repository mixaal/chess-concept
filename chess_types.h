#ifndef __MIX_CHESS_TYPES_H__
#define __MIX_CHESS_TYPES_H__ 1

#ifndef True
#  define True 1
#endif

#ifndef False
#  define False 0
#endif
#define CTRL_NOBODY      0x0
#define CTRL_WHITE       0x1
#define CTRL_BLACK       0x2
#define CTRL_BOTH        0x3
#define CTRL_MASK        (CTRL_WHITE|CTRL_BLACK)

#define PLACE_WHITE      0x10
#define PLACE_BLACK      0x20

typedef enum { 
  EMPTY,
  W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, 
  B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING
} chess_figure_t;

typedef struct {
  int N;
  int x[64], y[64];
} move_t;


typedef struct {
  chess_figure_t board[64];
  int            field_control[64];
  int white_king_x, white_king_y;
  int black_king_x, black_king_y;
  _Bool white_king_moved, black_king_moved;
  _Bool white_king_checked, black_king_checked;
} chess_t;

#endif /* __MIX_CHESS_TYPES_H__ */
