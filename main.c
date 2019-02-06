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

static chess_t chess;

void chess_init(chess_t *chess, const char *replay_file)
{
  chess->white_king_x = 4;
  chess->black_king_x = 4;
  chess->white_king_y = 0;
  chess->black_king_y = 7;
  chess->white_king_moved = False;
  chess->white_king_checked= False;
  chess->black_king_moved = False;
  chess->black_king_checked= False;
  board_init(chess->board, replay_file);
}



int main(int argc, char *argv[])
{
  setlocale(LC_CTYPE, "en_US.UTF-8");
  if(argc == 2) {
    chess_init(&chess, argv[1]);
  }
  else {
    chess_init(&chess, NULL);
  }
  for(;;) {
    float f = evaluate_position(&chess, who_is_playing());
    float mobility = mobility_evaluation(&chess, who_is_playing());
    compute_field_control(&chess);
    board_print(&chess, True);
    //print_field_control(field_control);
    wprintf(L"fitness=%.3f mobility=%.3f\n", f, mobility);
    next_move(&chess);
  }
  return EXIT_SUCCESS;
}
