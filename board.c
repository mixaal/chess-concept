#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "board.h"
//#include "chess_control.h"

static char *internal_cmd[] = {FN_WHITE_KING_CHECK, FN_BLACK_KING_CHECK, TOGGLE_TRACE, TOGGLE_FIELD_CONTROL, DISPLAY_HELP, SHOW_BOARD};

static _Bool is_internal_cmd(char *got)
{
  if (got[0]=='l' && got[1]=='e')  return True;
  for(int i=0; i<6; i++) if (!strcmp(got, internal_cmd[i])) return True;
  return False;
}


static int _trace = 0;
static int _display_field_control = 0;

_Bool replay_as_parameter = False;

static wchar_t chess_figures_symbols[] = {
  FULL_BLOCK_CHAR, 
  W_PAWN_CHAR, W_ROOK_CHAR, W_KNIGHT_CHAR, W_BISHOP_CHAR, W_QUEEN_CHAR, W_KING_CHAR,
  B_PAWN_CHAR, B_ROOK_CHAR, B_KNIGHT_CHAR, B_BISHOP_CHAR, B_QUEEN_CHAR, B_KING_CHAR
};

static FILE *chess_log = NULL;
static FILE *input_file = NULL;

static void chess_close(void)
{
  if(chess_log!=NULL)
    fclose(chess_log);
}
void write_move(int x0, int y0, int x1, int y1)
{
   if(chess_log==NULL) return;
   fprintf(chess_log, "%c%d%c%d\n", x0+'a', y0+1, x1+'a', y1+1);
   fflush(chess_log);
}

void init_input_file(const char *replay_file)
{
   chess_log = fopen("chess.log", "wb");
   if(chess_log!=NULL) atexit(chess_close);
   if(replay_file!=NULL) {
      replay_as_parameter = True;
      input_file = fopen(replay_file, "rb");
   }
   if(input_file == NULL) {
      replay_as_parameter = False;
      if(replay_file!=NULL) {
        wprintf(L"Error opening: %s\n", replay_file);
      }
      input_file = stdin;
   }
}

void print_field_control(int *field_control)
{
  for(int y=7; y>=0; y--) {
    wprintf(L"%d  ", y+1);
    for(int x=0; x<8; x++) {
      switch(field_control[x+8*y]&3) {
      case CTRL_NOBODY:
        wprintf(L"%s%lc%lc%lc%lc", ANSI_COLOR_RESET, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR);
        break;
      case CTRL_WHITE:
        wprintf(L"%s%lc%lc%lc%lc", ANSI_COLOR_BLUE, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR);
        break;
      case CTRL_BLACK:
        wprintf(L"%s%lc%lc%lc%lc", ANSI_COLOR_RED, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR);
        break;
      case CTRL_BOTH:
        wprintf(L"%s%lc%lc%lc%lc", ANSI_COLOR_YELLOW, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR, FULL_BLOCK_CHAR);
        break;
      }
    }
    wprintf(L"%s\n", ANSI_COLOR_RESET);
  }
}

static void print_empty(_Bool display_control, int idx, int control)
{
        if(!display_control) 
          wprintf(L"%lc", (idx&1) ? SPACE_CHAR : FULL_BLOCK_CHAR); 
        else {
          if(control!=CTRL_NOBODY) {
            wprintf(L"%lc", FULL_BLOCK_CHAR); 
          } else 
            wprintf(L"%lc", (idx&1) ? SPACE_CHAR : FULL_BLOCK_CHAR); 
        }
 
}

void board_print(chess_t *chess)
{
  int display_control = _display_field_control;
  chess_figure_t *chess_board = chess -> board;
  int *field_control = chess -> field_control;
  int white_on_the_move = chess->white_on_move;
  if(white_on_the_move) {
    wprintf(L"WHITE is playing:"); 
    if(chess->game_over) wprintf(L"  !!! white king check-mate !!!");
  } 
  else {
    wprintf(L"BLACK is playing:"); 
    if(chess->game_over) wprintf(L"  !!! black king check-mate !!!");
  }
  if(!chess->game_over) {
    if(white_king_check(chess)) {
      wprintf(L"  !!! white king check !!!");
    } 
    if(black_king_check(chess)) {
      wprintf(L"  !!! black king check !!!");
    }
  }
  for(int y=7; y>=0; y--) {
     int idx = y&1;
     wprintf(L"%lc", NEW_LINE_CHAR);
     wprintf(L"%d  ", y+1);
     for (int x=0; x<8; x++) {
        int control = field_control[y*8+x]&3;
        if(display_control) {
        switch(control) {
          case CTRL_NOBODY: wprintf(L"%s", ANSI_COLOR_RESET); break;
          case CTRL_BOTH:   wprintf(L"%s", ANSI_COLOR_YELLOW); break;
          case CTRL_WHITE:  wprintf(L"%s", ANSI_COLOR_BLUE); break;
          case CTRL_BLACK:  wprintf(L"%s", ANSI_COLOR_RED); break;
        };
        }
        chess_figure_t figure = chess_board[y*8+x];
        print_empty(display_control, idx, control);
        switch(figure) {
        case EMPTY: 
          print_empty(display_control, idx, control);
          print_empty(display_control, idx, control);
          break;
        default:
          wprintf(L"%lc", chess_figures_symbols[figure]);
          wprintf(L"%lc", SPACE_CHAR); 
        }
        print_empty(display_control, idx, control);
        wprintf(L"%s", ANSI_COLOR_RESET); 
        idx++;
     }
     int rows = chess->captured_white_idx / 5;
     if(y<=rows) {
       int _min = chess->captured_white_idx;
       //wprintf(L"y=%d rows=%d _min=%d\n", y, rows, _min); 
       if(_min>5+y*5) _min = 5+y*5;
       for(int i=y*5;i<_min;i++) {
         if(chess->captured_white_figures[i]!=0)
            wprintf(L" %lc ", chess_figures_symbols[chess->captured_white_figures[i]]);
       }
     }
     rows = chess->captured_black_idx / 5;
     int jj = 7 - y;;
     if(y>=7-rows) {
       int _min = chess->captured_black_idx;
       if(_min>5+jj*5) _min = 5+jj*5;
       //wprintf(L"y=%d rows=%d _min=%d\n", y, rows, _min); 
       for(int i=jj*5;i<_min;i++) {
         if(chess->captured_black_figures[i]!=0)
            wprintf(L" %lc ", chess_figures_symbols[chess->captured_black_figures[i]]);
       }
     }
  }
  wprintf(L"%lc", NEW_LINE_CHAR);
  wprintf(L"    a   b   c   d    e   f   g   h\n");
  //if(chess->game_over) exit(EXIT_SUCCESS);
}

static void display_help(void)
{
  wprintf(L"help - this help\n");
  wprintf(L"exit - quit chess program\nquit - quit chess program\nwkcs - white king check show\n");
  wprintf(L"bkcs - black king check show\ntrce - toggle trace messages\ntofc - toggle field control display\n");
  wprintf(L"leNN - show legal moves for field NN, e.g.: lea4 - show legal moves for field a4\n");
  wprintf(L"show - show chess board\n");
}
input_t get_console_input(void)
{
  char input[100], *got = NULL;
  input_t r;
  r.next_move = NULL;
  r.type = MOVE;
  int input_len = -1;
  for(;;) {
    got = NULL;
    input_len = -1;
    while(got == NULL || input_len!=5) {
      memset(input, 0, sizeof(input));
      wprintf(L">");
      if(feof(input_file)) {
         if(replay_as_parameter) {
           replay_as_parameter = False;
           input_file = stdin;
         } 
         else exit(EXIT_SUCCESS);
      }
      got = fgets(input, sizeof(input), input_file);
      if(got!=NULL) input_len=strlen(got);
    }
    r.next_move = got;
    got[4]= '\0';
    if(!strcmp(got, "quit") || !strcmp(got, "exit")) exit(EXIT_SUCCESS);
    if (is_internal_cmd(got)) {
      r.type=CMD; 
      return r;
    }
    char x0 = got[0];
    char y0 = got[1];
    char x1 = got[2];
    char y1 = got[3];
    //wprintf(L"%c %c %c %c\n", x0, y0, x1, y1);
    if(x0>='a' && x0<='h' && x1>='a' && x1<='h' && y0 >= '1' && y0 <= '8' && y1 >= '1' && y1 <= '8') return r;
    else wprintf(L"invalid range\n");
  }
}

static void show_legal_moves(chess_t *chess, chess_figure_t figure, int x0, int y0)
{
  move_t legal_moves = get_legal_moves(chess, figure, x0, y0, False);
  if(legal_moves.N==0) {
    wprintf(L"no legal moves for this figure");
  }
  
  wprintf(L"Legal move: [");
  for(int i=0; i<legal_moves.N; i++) {
    wprintf(L"%c%d,", 'a'+legal_moves.x[i], 1+legal_moves.y[i]);
  }
  wprintf(L"]\n");
}

void dispatch_to_helpers(chess_t *chess, char *next_move)
{
       if(!strcmp(next_move, DISPLAY_HELP)) {
         display_help();
       }
       if(!strcmp(next_move, SHOW_BOARD)) {
         board_print(chess);
       }
       if(!strcmp(next_move, TOGGLE_FIELD_CONTROL)) {
         _display_field_control ^= 1;
         wprintf(L"debug: display field control set: %d\n", _display_field_control);
       }
       if(next_move[0] == 'l' && next_move[1] == 'e'){
          if(_trace) 
            wprintf(L"debug: legal moves for: %c%c: start\n", next_move[2], next_move[3]);
          int xx = next_move[2] - 'a';
          int yy = next_move[3] - '1';
          chess_figure_t figure = chess->board[xx + yy*8];
          show_legal_moves(chess, figure, xx, yy);
          if(_trace) 
            wprintf(L"debug: legal moves for: %c%c: end\n", next_move[2], next_move[3]);
       }
       if(!strcmp(next_move, TOGGLE_TRACE)) {
          _trace ^= 1;
          wprintf(L"debug: trace: %d\n", _trace);
          set_trace(_trace);
       }
       if(!strcmp(next_move, FN_BLACK_KING_CHECK)) {
          _Bool check = black_king_check(chess);
          wprintf(L"debug: black_king_check(): %d\n", check);
          wprintf(L"debug: king [%c%d]: 0x%04x\n", chess->black_king_x+'a', chess->black_king_y+1, chess->field_control[chess->black_king_y*8+chess->black_king_x]);
       }
       if(!strcmp(next_move, FN_WHITE_KING_CHECK)) {
          _Bool check = white_king_check(chess);
          wprintf(L"debug: white_king_check(): %d\n", check);
          wprintf(L"debug: king [%c%d]: 0x%04x\n", chess->white_king_x+'a', chess->white_king_y+1, chess->field_control[chess->white_king_y*8+chess->white_king_x]);
       }
}
