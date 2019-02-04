#include <string.h>
#include "legal_moves.h"
#include "chess_types.h"
#include "chess_control.h"

void compute_field_control(chess_figure_t *chess_board, int *field_control)
{
  //memset(field_control, 0, 64*sizeof(int));
  for(int i=0; i<64; i++) {
      if(is_white(chess_board[i])) { field_control[i]=PLACE_WHITE;  continue;}
      if(is_black(chess_board[i])) { field_control[i]=PLACE_BLACK;  continue;}
      field_control[i]=CTRL_NOBODY;
  }
  for(int i=0; i<64; i++) {
    int mask = 0;
    // Skip empty fields
    if(chess_board[i] == EMPTY) continue;
    // Setup masks
    if(is_white(chess_board[i])) mask=CTRL_WHITE;
    if(is_black(chess_board[i])) mask=CTRL_BLACK;
    int x = i % 8;
    int y = i / 8;
    move_t legal_moves = get_legal_moves(chess_board, chess_board[i], x , y, True);
    for(int j=0; j<legal_moves.N; j++) {
       int offset = legal_moves.x[j] + legal_moves.y[j] * 8;
       field_control[offset] |= mask;
    }
  }
}


