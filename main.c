#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "board.h"
#include "chess_control.h"

static int field_control[64];
static chess_figure_t chess_board[64];

int main(void)
{
  board_init(chess_board);
  for(;;) {
    compute_field_control(chess_board, field_control);
    board_print(chess_board, field_control, True);
    print_field_control(field_control);
    next_move(chess_board);
  }
  return EXIT_SUCCESS;
}
