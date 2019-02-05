#ifndef __MIX_LEGAL_MOVES_H__ 
#define __MIX_LEGAL_MOVES_H__  1

#include "chess_types.h"

int get_white_king_y(void) ;
int get_white_king_x(void) ;
int get_black_king_y(void) ;
int get_black_king_x(void) ;
void set_white_king(int x, int y, int tx, int ty, _Bool real_board);
void set_black_king(int x, int y, int tx, int ty, _Bool real_board);
_Bool is_white(chess_figure_t figure);
_Bool is_black(chess_figure_t figure);
_Bool white_king_check(int *field_control_copy, _Bool real_board);
_Bool black_king_check(int *field_control_copy, _Bool real_board);
move_t pawn_legal_moves(chess_figure_t *chess_board, chess_figure_t pawn, int x, int y, _Bool control);
move_t rook_legal_moves(chess_figure_t *chess_board, chess_figure_t rook, int x, int y, _Bool control);
move_t bishop_legal_moves(chess_figure_t *chess_board, chess_figure_t bishop, int x, int y, _Bool control);
move_t queen_legal_moves(chess_figure_t *chess_board, chess_figure_t bishop, int x, int y, _Bool control);
move_t king_legal_moves(chess_figure_t *chess_board, chess_figure_t king, int x, int y, _Bool control);
move_t knight_legal_moves(chess_figure_t *chess_board, chess_figure_t knight, int x, int y, _Bool control);
move_t get_legal_moves(chess_figure_t *chess_board, chess_figure_t figure_t, int x0, int y0, _Bool control);
void do_chess_move(chess_figure_t *chess_board, int x0, int y0, int x1, int y1, _Bool real_board);

#endif /* __MIX_LEGAL_MOVES_H__ */
