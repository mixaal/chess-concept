#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "board.h"

static wchar_t chess_figures_symbols[] = {
  FULL_BLOCK_CHAR, 
  W_PAWN_CHAR, W_ROOK_CHAR, W_KNIGHT_CHAR, W_BISHOP_CHAR, W_QUEEN_CHAR, W_KING_CHAR,
  B_PAWN_CHAR, B_ROOK_CHAR, B_KNIGHT_CHAR, B_BISHOP_CHAR, B_QUEEN_CHAR, B_KING_CHAR
};

static int field_control[64];
static chess_figure_t chess_board[64];
static chess_figure_t captured_white_figures[16];
static chess_figure_t captured_black_figures[16];
static int captured_white_idx = 0;
static int captured_black_idx = 0;

static int white_on_the_move = 1;

void print_field_control(void)
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

void compute_field_control(void)
{
  for(int i=0; i<64; i++) {
      if(is_white(chess_board[i])) { field_control[i]=PLACE_WHITE;  }
      if(is_black(chess_board[i])) { field_control[i]=PLACE_BLACK;  }
      field_control[i]|=CTRL_NOBODY;
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

void board_init(void)
{
   memset(chess_board, 0, sizeof(chess_board) * sizeof(chess_figure_t));
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

void board_print(_Bool display_control)
{
  setlocale(LC_CTYPE, "en_US.UTF-8");
  if(white_on_the_move) wprintf(L"WHITE is playing:"); 
  else wprintf(L"BLACK is playing:"); 
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
}

static char *get_console_input(void)
{
  char input[100], *got = NULL;
  for(;;) {
    do {
      wprintf(L">");
      got = fgets(input, sizeof(input), stdin);
    } while (got != NULL && strlen(got) != 5);
    got[4]= '\0';
    if(!strcmp(got, "quit") || !strcmp(got, "exit")) exit(EXIT_SUCCESS);
    char x0 = got[0];
    char y0 = got[1];
    char x1 = got[2];
    char y1 = got[3];
    wprintf(L"%c %c %c %c\n", x0, y0, x1, y1);
    if(x0>='a' && x0<='h' && x1>='a' && x1<='h' && y0 >= '1' && y0 <= '8' && y1 >= '1' && y1 <= '8') return got;
    else wprintf(L"invalid range\n");
  }
}

static char *is_valid_move(int x0, int y0, int x1, int y1)
{
  
  chess_figure_t figure = chess_board[8*y0 + x0];
  if(figure == EMPTY) {
    return "nobody standing there";
  }
  if(white_on_the_move) {
    if(!is_white(figure)) return "white is on the move, this figure is a black one"; 
  } else {
    if(!is_black(figure)) return "black is on the move, this figure is a white one"; 
  }
  move_t legal_moves = get_legal_moves(chess_board, figure, x0, y0, False);
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

void next_move(void)
{
   char *err = NULL;
   int x0, y0, x1, y1;
   do {
     char *next_move = get_console_input();
     x0 = next_move[0] - 'a';
     x1 = next_move[2] - 'a';
     y0 = next_move[1] - '1';
     y1 = next_move[3] - '1';
     err = is_valid_move(x0, y0, x1, y1);
     if(err!=NULL) {
       wprintf(L"Invalid move: %s\n", err);
     }
   } while (err != NULL);
   
   // Process to the next state
   chess_figure_t figure = chess_board[8*y0 + x0];
   if(figure==W_KING) {
     if(x0==4 && y0==0 && x1==6 && y1==0) {//castling
        chess_board[5] = chess_board[7];
        chess_board[7] = EMPTY;
     }
     if(x0==4 && y0==0 && x1==1 && y1==0) {//castling
        chess_board[2] = chess_board[0];
        chess_board[0] = EMPTY;
     }
   }
   if(figure==B_KING) {
     if(x0==4 && y0==7 && x1==6 && y1==7) {//castling
        chess_board[61] = chess_board[63];
        chess_board[63] = EMPTY;
     }
     if(x0==4 && y0==7 && x1==1 && y1==7) {//castling
        chess_board[58] = chess_board[56];
        chess_board[56] = EMPTY;
     }
   }

   chess_figure_t other  = chess_board[8*y1 + x1];
   if(other!=EMPTY) {
      if(other<=W_KING) captured_black_figures[captured_black_idx++] = other;
      else              captured_white_figures[captured_white_idx++] = other;
   }
   chess_board[8*y0 + x0] = EMPTY;
   chess_board[8*y1 + x1] = figure;
   white_on_the_move ^= 1;
}

int main(void)
{
  board_init();
  for(;;) {
    compute_field_control();
    board_print(True);
    print_field_control();
    next_move();
  }
  return EXIT_SUCCESS;
}
