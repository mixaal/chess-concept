#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "legal_moves.h"
#include "chess_types.h"
#include "chess_control.h"
#include "board.h"

void chess_init(chess_t *chess)
{
  chess->white_on_move = 1;
  chess->white_king_x = 4;
  chess->black_king_x = 4;
  chess->white_king_y = 0;
  chess->black_king_y = 7;
  chess->white_king_moved = False;
  chess->white_king_checked= False;
  chess->black_king_moved = False;
  chess->black_king_checked= False;
  chess->game_over = False;
  memset(chess->captured_white_figures, 0, 16*sizeof(chess_figure_t));
  memset(chess->captured_black_figures, 0, 16*sizeof(chess_figure_t));
  chess->captured_black_idx = chess->captured_white_idx = 0;
  board_init(chess->board);
}

void is_game_over(chess_t *chess)
{
int white_on_the_move = chess->white_on_move;
if(white_on_the_move) {
    if(white_king_check(chess) && (int)(chess->mobility) == 0) {
       chess->game_over = True;
    }
  } 
  else {
    if(black_king_check(chess) && (int)(chess->mobility) == 0) {
       chess->game_over = True;
    }
  }

}

void board_init(chess_figure_t *chess_board)
{
   memset(chess_board, 0, 64 * sizeof(chess_figure_t));
   //memset(captured_white_figures, 0, sizeof(captured_white_figures) * sizeof(chess_figure_t));
   //memset(captured_black_figures, 0, sizeof(captured_black_figures) * sizeof(chess_figure_t));

   for(int i=0; i<8; i++) {
      chess_board[8+i] = W_PAWN;
      chess_board[48+i] = B_PAWN;
   }
   chess_board[0] = chess_board[7] = W_ROOK;
   chess_board[56] = chess_board[63] = B_ROOK;
   chess_board[1] = chess_board[6] = W_KNIGHT;
   chess_board[57] = chess_board[62] = B_KNIGHT;
   chess_board[2] = chess_board[5] = W_BISHOP;
   chess_board[58] = chess_board[61] = B_BISHOP;
   chess_board[3] = W_QUEEN;
   chess_board[4] = W_KING;
   chess_board[59] = B_QUEEN;
   chess_board[60] = B_KING;
}

static char *is_valid_move(chess_t *chess, int x0, int y0, int x1, int y1)
{
  
  chess_figure_t figure = chess->board[8*y0 + x0];
  if(figure == EMPTY) {
    return "nobody standing there";
  }
  if(chess->white_on_move) {
    if(!is_white(figure)) return "white is on the move, this figure is a black one"; 
  } else {
    if(!is_black(figure)) return "black is on the move, this figure is a white one"; 
  }
  move_t legal_moves = get_legal_moves(chess, figure, x0, y0, False);
  if(legal_moves.N==0) {
    return "no legal moves for this figure";
  }
  for(int i=0; i<legal_moves.N; i++) {
    if(x1==legal_moves.x[i] && y1==legal_moves.y[i]) return NULL;
  }
  
  wprintf(L"Legal move: [");
  for(int i=0; i<legal_moves.N; i++) {
    wprintf(L"%c%d,", 'a'+legal_moves.x[i], 1+legal_moves.y[i]);
  }
  wprintf(L"]\n");
  return "your move is not in a set of legal moves";
}

void next_move(chess_t *chess, get_input_t read_input)
{
   char *err = NULL;
   int x0, y0, x1, y1;
AGAIN:
   do {
     input_t  r = read_input();
     char *next_move = r.next_move;
     if(r.type==CMD) {
      dispatch_to_helpers(chess, next_move);
      goto AGAIN;
     }
     x0 = next_move[0] - 'a';
     x1 = next_move[2] - 'a';
     y0 = next_move[1] - '1';
     y1 = next_move[3] - '1';
     err = is_valid_move(chess, x0, y0, x1, y1);
     if(err!=NULL) {
       wprintf(L"Invalid move: %s\n", err);
     }
   } while (err != NULL);
   //wprintf(L"valid move: %d %d\n", x1, y1);
   
   // Process to the next state
   chess_figure_t other  = chess->board[8*y1 + x1];
   if(other!=EMPTY) {
      if(other<=W_KING) chess->captured_black_figures[chess->captured_black_idx++] = other;
      else              chess->captured_white_figures[chess->captured_white_idx++] = other;
   }
   //wprintf(L"valid move2: %d %d\n", x1, y1);
   write_move(x0, y0, x1, y1);
   do_chess_move(chess, x0, y0, x1, y1, True);
   chess->white_on_move ^= 1;
}
