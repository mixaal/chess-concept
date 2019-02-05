#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "board.h"
#include "chess_control.h"
#include "fitness.h"

static int field_control[64];
static chess_figure_t chess_board[64];

int main(int argc, char *argv[])
{
  setlocale(LC_CTYPE, "en_US.UTF-8");
  if(argc == 2) {
    board_init(chess_board, argv[1]);
  }
  else {
    board_init(chess_board, NULL);
  }
  for(;;) {
    float f = evaluate_position(chess_board, who_is_playing());
    float mobility = mobility_evaluation(chess_board, who_is_playing());
    compute_field_control(chess_board, field_control);
    board_print(chess_board, field_control, True);
    //print_field_control(field_control);
    wprintf(L"fitness=%.3f mobility=%.3f\n", f, mobility);
    next_move(chess_board, field_control);
  }
  return EXIT_SUCCESS;
}
