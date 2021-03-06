#include "fitness.h"
#include "legal_moves.h"

static float material_evaluation(chess_t *chess);

float evaluate_position(chess_t *chess)
{
  float s = 0.0f;
  s+=material_evaluation(chess);
  return (chess->white_on_move)  ? s : -s;
}

float mobility_evaluation(chess_t *chess) 
{
   int total_moves = 0;
   for(int i=0; i<64; i++) {
    if(!is_white(chess->board[i]) && !is_black(chess->board[i])) continue;
    if(is_white(chess->board[i])) {
       if(!chess->white_on_move) continue;
       move_t legal_moves  = get_legal_moves(chess, chess->board[i], i%8, i/8, False);
       total_moves += legal_moves.N;
    }
    else {
       if(chess->white_on_move) continue;
       move_t legal_moves  = get_legal_moves(chess, chess->board[i], i%8, i/8, False);
       total_moves += legal_moves.N;
    }
   }
   chess -> mobility = total_moves;
   return total_moves;
}

static float material_evaluation(chess_t *chess)
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
    switch(chess->board[i]) {
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
