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

int main(int argc, char *argv[])
{
  setlocale(LC_CTYPE, "en_US.UTF-8");
  if(argc == 2) {
    init_input_file(argv[1]);
  }
  else {
    init_input_file(NULL);
  }
  chess_init(&chess);
  for(;;) {
    float f = evaluate_position(&chess);
    float mobility = mobility_evaluation(&chess);
    compute_field_control(&chess);
    is_game_over(&chess);
    board_print(&chess);
    //print_field_control(field_control);
    wprintf(L"fitness=%.3f mobility=%.3f\n", f, mobility);
    next_move(&chess, get_console_input);
  }
  return EXIT_SUCCESS;
}
