#include "fitness.h"

static float material_evaluation(chess_figure_t *chess_board);

float evaluate_position(chess_figure_t *chess_board, int white_is_playing)
{
  float s = 0.0f;
  s+=material_evaluation(chess_board);
  return (white_is_playing)  ? s : -s;
}


static float material_evaluation(chess_figure_t *chess_board)
{
  int wqueen_no = 0;
  int bqueen_no = 0;
  int wrook_no = 0;
  int brook_no = 0;
  int wbishop_no = 0;
  int bbishop_no = 0;
  int wknight_no = 0;
  int bknight_no = 0;
  int wpawn_no = 0;
  int bpawn_no = 0;
  for(int i=0; i<64; i++) {
    switch(chess_board[i]) {
    case W_PAWN: wpawn_no++; continue;
    case B_PAWN: bpawn_no++; continue;
    case W_KNIGHT: wknight_no++; continue;
    case B_KNIGHT: bknight_no++; continue;
    case W_BISHOP: wbishop_no++; continue;
    case B_BISHOP: bbishop_no++; continue;
    case W_ROOK: wrook_no++; continue;
    case B_ROOK: brook_no++; continue;
    case W_QUEEN: wqueen_no++; continue;
    case B_QUEEN: bqueen_no++; continue;
    default: break;
    }
  }
  return PAWN_VALUE * (wpawn_no - bpawn_no) + 
         KNIGHT_VALUE * (wknight_no - bknight_no) +
         BISHOP_VALUE * (wbishop_no - bbishop_no) +
         ROOK_VALUE * (wrook_no - brook_no) +
         QUEEN_VALUE * (wqueen_no - bqueen_no);
}
