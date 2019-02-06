#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "board.h"
#include "chess_control.h"

static FILE *chess_log = NULL;
static FILE *input_file = NULL;

static int _trace = 0;

_Bool replay_as_parameter = False;

static wchar_t chess_figures_symbols[] = {
  FULL_BLOCK_CHAR, 
  W_PAWN_CHAR, W_ROOK_CHAR, W_KNIGHT_CHAR, W_BISHOP_CHAR, W_QUEEN_CHAR, W_KING_CHAR,
  B_PAWN_CHAR, B_ROOK_CHAR, B_KNIGHT_CHAR, B_BISHOP_CHAR, B_QUEEN_CHAR, B_KING_CHAR
};

static chess_figure_t captured_white_figures[16];
static chess_figure_t captured_black_figures[16];
static int captured_white_idx = 0;
static int captured_black_idx = 0;

static int white_on_the_move = 1;

int who_is_playing(void)
{
  return white_on_the_move;
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

static void chess_close(void)
{
  if(chess_log!=NULL)
    fclose(chess_log);
}

void board_init(chess_figure_t *chess_board, const char *replay_file)
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
   memset(chess_board, 0, 64 * sizeof(chess_figure_t));
   memset(captured_white_figures, 0, sizeof(captured_white_figures) * sizeof(chess_figure_t));
   memset(captured_black_figures, 0, sizeof(captured_black_figures) * sizeof(chess_figure_t));

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

void board_print(chess_t *chess, float mobility, _Bool display_control)
{
  chess_figure_t *chess_board = chess -> board;
  int *field_control = chess -> field_control;
  _Bool game_over = False;
  if(white_on_the_move) {
    wprintf(L"WHITE is playing:"); 
    if(white_king_check(chess) && (int)(mobility) == 0) {
       wprintf(L"  !!! white king check-mate !!!");
       game_over = True;
    }
  } 
  else {
    wprintf(L"BLACK is playing:"); 
    if(black_king_check(chess) && (int)(mobility) == 0) {
       wprintf(L"  !!! black king check-mate !!!");
       game_over = True;
    }
  }
  if(!game_over) {
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
     if(y==0) for(int i=0;i<captured_white_idx;i++) {
        if(captured_white_figures[i]!=0)
          wprintf(L" %lc ", chess_figures_symbols[captured_white_figures[i]]);
     }
     if(y==7) for(int i=0;i<captured_black_idx;i++) {
        if(captured_black_figures[i]!=0)
          wprintf(L" %lc ", chess_figures_symbols[captured_black_figures[i]]);
     }
  }
  wprintf(L"%lc", NEW_LINE_CHAR);
  wprintf(L"    a   b   c   d    e   f   g   h\n");
  if(game_over) exit(EXIT_SUCCESS);
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
    if(!strcmp(got, TOGGLE_TRACE) || !strcmp(got, FN_WHITE_KING_CHECK) || !strcmp(got, FN_BLACK_KING_CHECK) || (got[0]=='l' && got[1]=='e' )) {
      r.type=CMD; 
      return r;
    }
    char x0 = got[0];
    char y0 = got[1];
    char x1 = got[2];
    char y1 = got[3];
    wprintf(L"%c %c %c %c\n", x0, y0, x1, y1);
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

static char *is_valid_move(chess_t *chess, int x0, int y0, int x1, int y1)
{
  
  chess_figure_t figure = chess->board[8*y0 + x0];
  if(figure == EMPTY) {
    return "nobody standing there";
  }
  if(white_on_the_move) {
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

void next_move(chess_t *chess)
{
   char *err = NULL;
   int x0, y0, x1, y1;
AGAIN:
   do {
     input_t  r = get_console_input();
     char *next_move = r.next_move;
     if(r.type==CMD) {
       if(next_move[0] == 'l' && next_move[1] == 'e'){
          wprintf(L"debug: legal moves for: %c%c: start\n", next_move[2], next_move[3]);
          int xx = next_move[2] - 'a';
          int yy = next_move[3] - '1';
          chess_figure_t figure = chess->board[xx + yy*8];
          show_legal_moves(chess, figure, xx, yy);
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
      if(other<=W_KING) captured_black_figures[captured_black_idx++] = other;
      else              captured_white_figures[captured_white_idx++] = other;
   }
   //wprintf(L"valid move2: %d %d\n", x1, y1);
   fprintf(chess_log, "%c%d%c%d\n", x0+'a', y0+1, x1+'a', y1+1);
   fflush(chess_log);
   do_chess_move(chess, x0, y0, x1, y1, True);
   white_on_the_move ^= 1;
}
