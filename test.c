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
#include "test.h"
#include "test_helper.h"

static chess_t chess;

int main(int argc, char *argv[])
{
  
  setlocale(LC_CTYPE, "en_US.UTF-8");
  _Bool display = False;

  test_play(&chess, check_mate, sizeof(check_mate)/sizeof(char*), display);
  assert_ieq(1, chess.game_over, "check-mate scenario");

  test_play(&chess, check_on_white, sizeof(check_on_white)/sizeof(char*), display);
  assert_ieq(1, white_king_check(&chess), "white king checked");
  assert_ieq(0, black_king_check(&chess), "black king not checked");
  move_t m = get_legal_moves(&chess, W_KING, 4, 0, False);
  assert_ieq(2, m.N, "only two positions allowed [d1, d2]");
  assert_ieq(3, m.x[0], "d1 position");
  assert_ieq(0, m.y[0], "d1 position");
  assert_ieq(3, m.x[1], "d2 position");
  assert_ieq(1, m.y[1], "d2 position");

  test_play(&chess, castling_possible_for_white, sizeof(castling_possible_for_white)/sizeof(char*), display);
  assert_ieq(0, white_king_check(&chess), "white king not checked");
  assert_ieq(0, black_king_check(&chess), "black king not checked");
  m = get_legal_moves(&chess, W_KING, 4, 0, False);
  assert_ieq(3, m.N, "white king: 3 moves still possible (means castling possible)");

  test_play(&chess, castling_not_possible_for_white, sizeof(castling_not_possible_for_white)/sizeof(char*), display);
  assert_ieq(0, white_king_check(&chess), "white king not checked");
  assert_ieq(0, black_king_check(&chess), "black king not checked");
  m = get_legal_moves(&chess, W_KING, 4, 0, False);
  assert_ieq(2, m.N, "white king: 2 moves possible (means castling not possible)");

  test_play(&chess, castling_still_possible_for_black, sizeof(castling_still_possible_for_black)/sizeof(char*), display);
  assert_ieq(0, white_king_check(&chess), "white king not checked");
  assert_ieq(0, black_king_check(&chess), "black king not checked");
  m = get_legal_moves(&chess, B_KING, 4, 7, False);
  assert_ieq(3, m.N, "black king: 3 moves possible (means castling possible)");

  test_play(&chess, castling_not_possible_for_black, sizeof(castling_not_possible_for_black)/sizeof(char*), display);
  assert_ieq(0, white_king_check(&chess), "white king not checked");
  assert_ieq(0, black_king_check(&chess), "black king not checked");
  m = get_legal_moves(&chess, B_KING, 4, 7, False);
  assert_ieq(2, m.N, "black king: 2 moves possible (means castling not possible)");
  
  return get_rc();
}
