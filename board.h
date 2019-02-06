#ifndef __MIX_BOARD_H__
#define __MIX_BOARD_H__ 1


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define FULL_BLOCK_CHAR L'\u2588'
#define SPACE_CHAR      L'\x20'
#define NEW_LINE_CHAR   L'\x0a'
#define W_KING_CHAR     L'\u2654'
#define W_QUEEN_CHAR    L'\u2655'
#define W_ROOK_CHAR     L'\u2656'
#define W_BISHOP_CHAR   L'\u2657'
#define W_KNIGHT_CHAR   L'\u2658'
#define W_PAWN_CHAR     L'\u2659'
#define B_KING_CHAR     L'\u265a'
#define B_QUEEN_CHAR    L'\u265b'
#define B_ROOK_CHAR     L'\u265c'
#define B_BISHOP_CHAR   L'\u265d'
#define B_KNIGHT_CHAR   L'\u265e'
#define B_PAWN_CHAR     L'\u265f'

#define FN_WHITE_KING_CHECK "fwkc"
#define FN_BLACK_KING_CHECK "fbkc"
#define TOGGLE_TRACE        "trce"

typedef enum { MOVE, CMD } cmd_t;
typedef struct { 
  char  *next_move;
  cmd_t type ;
} input_t;

_Bool get_trace(void) ;
void print_field_control(int *field_control);
void board_init(chess_figure_t *chess, const char *replay_file);
void board_print(chess_t *chess, _Bool display_control);
input_t get_console_input(void);
void next_move(chess_t *chess);
int who_is_playing(void);
#endif
