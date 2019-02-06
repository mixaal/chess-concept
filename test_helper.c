#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "test_helper.h"
#include "board.h"
#include "fitness.h"
#include "chess_types.h"
#include "chess_control.h"
#include "legal_moves.h"

static char **current_queue = NULL;
static int queue_idx = 0;
static int rc = 0;

static void setptr(char **moves);
static input_t get_next(void);

int get_rc(void)
{
  return rc;
}

void assert_ieq(int expected, int got, const char *msg)
{
if(expected==got) {
  wprintf(L"%s ..... %s[%sOK%s%s]%s\n", msg, ANSI_BOLD, ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, ANSI_BOLD, ANSI_COLOR_RESET);
}
else {
  rc = -1;
  wprintf(L"%s ..... %s[%sFAIL%s%s]%s [expected=%d, got=%d]\n", msg, ANSI_BOLD, ANSI_COLOR_RED, ANSI_COLOR_RESET, ANSI_BOLD, ANSI_COLOR_RESET, expected, got);
}
}

static void setptr(char **moves)
{
  current_queue = moves;
  queue_idx = 0;
}

static input_t get_next(void)
{
   input_t r;
   r.type = MOVE;
   r.next_move = current_queue[queue_idx++];
   //wprintf(L"playing: %s\n", r.next_move);
   return r;
}

void test_play(chess_t *chess, char *moves[], size_t move_no, _Bool display)
{
  setlocale(LC_CTYPE, "en_US.UTF-8");
  chess_init(chess);
  setptr(moves);
  for(int i=0; i<=move_no; i++) {
    float f = evaluate_position(chess);
    float mobility = mobility_evaluation(chess);
    compute_field_control(chess);
    is_game_over(chess);
    if(display) {
      board_print(chess);
      //print_field_control(field_control);
      wprintf(L"fitness=%.3f mobility=%.3f\n", f, mobility);
    }
    if(i==move_no) return;
    next_move(chess, get_next);
  }
}


