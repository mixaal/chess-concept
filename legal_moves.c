#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "chess_types.h"
#include "legal_moves.h"
#include "chess_control.h"

static _Bool white_king_moved = False;
static _Bool black_king_moved = False;

static _Bool white_king_checked = False;
static _Bool black_king_checked = False;

static int white_king_x = 4;
static int white_king_y = 0;

static int black_king_x = 4;
static int black_king_y = 7;

void set_white_king(int x, int y, int tx, int ty, _Bool real_board)
{
  if(real_board && x!=tx && y!=tx)  
    white_king_moved = True;
  white_king_x = tx;
  white_king_y = ty;
}

void set_black_king(int x, int y, int tx, int ty, _Bool real_board)
{
  if(real_board && x!=tx && y!=tx)  
    black_king_moved = True;
  black_king_x = tx;
  black_king_y = ty;
}

_Bool is_white(chess_figure_t figure)
{
  return figure >= W_PAWN && figure <= W_KING;
}

_Bool is_black(chess_figure_t figure)
{
  return figure >= B_PAWN && figure <= B_KING;
}

_Bool white_king_check(int *field_control_copy, _Bool real_board)
{
  int control = field_control_copy[white_king_x + 8*white_king_y] & CTRL_MASK;
  if (control&CTRL_BLACK) {
    if(real_board) white_king_checked = True;
    return True;
  }
  return False;
}

_Bool black_king_check(int *field_control_copy, _Bool real_board)
{
  int control = field_control_copy[black_king_x + 8*black_king_y] & CTRL_MASK;
  if (control&CTRL_WHITE) {
    if(real_board) black_king_checked = True;
    return True;
  }
  return False;
}

static chess_figure_t chess_board_copy[64];
static int field_control_copy[64];

void do_chess_move(chess_figure_t *chess_board, int x0, int y0, int x1, int y1, _Bool real_board)
{
   //wprintf(L"do_chess_move: chess_board=%p x0=%d y0=%d x1=%d y1=%d %c%d\n", chess_board, x0, y0, x1, y1, x1+'a', 1+y1);
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

   /** Update king's position */
   if(figure==W_KING) set_white_king(x0, y0, x1, y1, real_board);
   if(figure==B_KING) set_black_king(x0, y0, x1, y1, real_board);


   chess_board[8*y0 + x0] = EMPTY;
   chess_board[8*y1 + x1] = figure;
}

static void add_legal_move(move_t *legal_moves, chess_figure_t *chess_board_copy, int *field_control, chess_figure_t my, int orig_x, int orig_y, int tx, int ty, _Bool control)
{
   if(control) goto ADD_MOVE;
   do_chess_move(chess_board_copy, orig_x, orig_y, tx, ty, False);
   compute_field_control(chess_board_copy, field_control);
   _Bool legal = (is_white(my) && !white_king_check(field_control, False)) || (is_black(my) && !black_king_check(field_control, False)) ;

  if(!legal) return;
ADD_MOVE:
  legal_moves->x[legal_moves->N] = tx;
  legal_moves->y[legal_moves->N] = ty;
  legal_moves->N ++;

}

static inline void copy_chess_board(chess_figure_t * chess_board)
{
   memcpy(chess_board_copy, chess_board, 64*sizeof(chess_figure_t));
}

static _Bool figure_move(move_t *legal_moves, chess_figure_t *chess_board, chess_figure_t my, int orig_x, int orig_y, int tx, int y, _Bool control)
{
     /**
      * Create a copy of a chess board a and try the move.
      */
     copy_chess_board(chess_board);
     if(tx<0 || tx>7 || y<0 || y>7) return 0;
     chess_figure_t other = chess_board[tx+8*y];
     int idx = legal_moves->N;
     //wprintf(L"check move: %d %d other=%d\n", tx, y, other);
     //wprintf(L"check move: is_white(other)=%d is_white(my)=%d is_black(other)=%d is_black(my)=%d\n", is_white(other), is_white(my), is_black(other), is_black(my));
     if(other!=EMPTY) {
       if(!control) {
         if(is_white(other)==is_white(my)) return 1;
         if(is_black(other)==is_black(my)) return 1;
       }
       else {
         if(is_white(other)==is_white(my) || is_black(other) == is_black(my)) {
           legal_moves->N ++;
           legal_moves->x[idx] = tx;
           legal_moves->y[idx] = y;
           return 1;
         }
       }
       //wprintf(L"check_move: passed own figure");
       if((is_white(other)==is_black(my)) || (is_black(other)==is_white(my))) {
         //wprintf(L"check_move: capture figure");
         //legal move, but break
         add_legal_move(legal_moves, chess_board_copy, field_control_copy, my, orig_x, orig_y, tx, y, control);
         return 1;
       }
     }
     if(other!=EMPTY) {
        wprintf(L"Error: other==%d, while expecting empty\n", other);
        exit(EXIT_FAILURE);
     }
     //wprintf(L"legal move: %d %d\n", tx, y);
     add_legal_move(legal_moves, chess_board_copy, field_control_copy, my, orig_x, orig_y, tx, y, control);
     return 0;
}
static void __rook_legal_moves(move_t *legal_moves, chess_figure_t *chess_board, chess_figure_t rook, int x, int y, _Bool control)
{
  
   // to the right
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     if(tx>7) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, rook, x, y, tx, y, control)) break;
   }
   //wprintf(L"check the right: %d\n", legal_moves.N);

   //to the left
   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     if(tx<0) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, rook, x, y, tx, y, control)) break;
   }
   //wprintf(L"check the left: %d\n", legal_moves.N);

   // up 
   for(int dx=1; dx<=7; dx++) {
     int ty=y-dx;
     if(ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, rook, x, y, x, ty, control)) break;
   }
   //wprintf(L"check the up: %d\n", legal_moves.N);
   // down
   for(int dx=1; dx<=7; dx++) {
     int ty=y+dx;
     if(ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, rook, x, y, x, ty, control)) break;
   }
   //wprintf(L"check the down: %d\n", legal_moves.N);
} 


static void __bishop_legal_moves(move_t *legal_moves, chess_figure_t *chess_board, chess_figure_t bishop, int x, int y, _Bool control) {
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     int ty=y+dx;
     if(tx>7 || ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, bishop, x, y, tx, ty, control)) break;
   }

   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     int ty=y-dx;
     if(tx<0 || ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, bishop, x, y, tx, ty, control)) break;
   }
 
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     int ty=y-dx;
     if(tx>7 || ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, bishop, x, y, tx, ty, control)) break;
   }

   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     int ty=y+dx;
     if(tx<0 || ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess_board, bishop, x, y, tx, ty, control)) break;
   }
}

move_t knight_legal_moves(chess_figure_t *chess_board, chess_figure_t knight, int x, int y, _Bool control)
{
   move_t legal_moves;
   legal_moves.N = 0;
   figure_move(&legal_moves, chess_board, knight, x, y, x+1, y-2, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x-1, y-2, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x+2, y-1, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x-2, y-1, control);

   figure_move(&legal_moves, chess_board, knight, x, y, x+1, y+2, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x-1, y+2, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x+2, y+1, control);
   figure_move(&legal_moves, chess_board, knight, x, y, x-2, y+1, control);
   return legal_moves;
}

move_t king_legal_moves(chess_figure_t *chess_board, chess_figure_t king, int x, int y, _Bool control)
{

   move_t legal_moves;
   legal_moves.N = 0;
   figure_move(&legal_moves, chess_board, king, x, y, x+1, y, control);
   figure_move(&legal_moves, chess_board, king, x, y, x, y+1, control);
   figure_move(&legal_moves, chess_board, king, x, y, x-1, y, control);
   figure_move(&legal_moves, chess_board, king, x, y, x, y-1, control);

   figure_move(&legal_moves, chess_board, king, x, y, x-1, y-1, control);
   figure_move(&legal_moves, chess_board, king, x, y, x-1, y+1, control);
   figure_move(&legal_moves, chess_board, king, x, y, x+1, y-1, control);
   figure_move(&legal_moves, chess_board, king, x, y, x+1, y+1, control);

   if(control) return legal_moves; // FIXME: is this correct?

   //FIXME rook moved?
   //wprintf(L"white_king_checked=%d white_king_moved=%d\n", white_king_checked, white_king_moved);
   if(!white_king_checked && !white_king_moved && is_white(king)) {
      if(x==4 && y==0 && chess_board[5]==chess_board[6] && chess_board[5]==EMPTY && chess_board[7]==W_ROOK) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_KING, 4, 0, 6, 0, control);
     }
      if(x==4 && y==0 && chess_board[1]==chess_board[2] && chess_board[2]==chess_board[3] && chess_board[1]==EMPTY && chess_board[0]==W_ROOK) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_KING, 4, 0, 1, 0, control);
       }
   }
   if(!black_king_checked && !black_king_moved && is_black(king)) {
      if(x==4 && y==7 && chess_board[61]==chess_board[62] && chess_board[61]==EMPTY && chess_board[63]==B_ROOK) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_KING, 4, 7, 6, 7, control);
     }
      if(x==4 && y==7 && chess_board[57]==chess_board[58] && chess_board[58]==chess_board[59] && chess_board[58]==EMPTY && chess_board[56]==B_ROOK) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_KING, 4, 7, 1, 7, control);
     }
   }

 
   //FIXME check for white_king_check/black_king_check

   return legal_moves;
}


move_t queen_legal_moves(chess_figure_t *chess_board, chess_figure_t queen, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __bishop_legal_moves(&legal_moves, chess_board, queen, x, y, control);  
   __rook_legal_moves(&legal_moves, chess_board, queen, x, y, control);  
   return legal_moves;
}


move_t bishop_legal_moves(chess_figure_t *chess_board, chess_figure_t bishop, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __bishop_legal_moves(&legal_moves, chess_board, bishop, x, y, control);  
   return legal_moves;
}

move_t rook_legal_moves(chess_figure_t *chess_board, chess_figure_t rook, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __rook_legal_moves(&legal_moves, chess_board, rook, x, y, control);
   return legal_moves;
} 

move_t pawn_legal_moves(chess_figure_t *chess_board, chess_figure_t pawn, int x, int y, _Bool control)
{
   move_t legal_moves;
   legal_moves.N = 0;
   //wprintf(L"init: x=%d  y=%d\n", x, y);
   switch(pawn) {
   case W_PAWN:
     // 1 position straight
     if(!control) {
     if(chess_board[(y+1)*8+x]==EMPTY) {
      //wprintf(L"check: x=%d  y=%d\n", x, y+1);
      
      copy_chess_board(chess_board);
      add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_PAWN, x, y, x, y+1, control);
      //wprintf(L"got : x=%d  y=%d\n", x, y+1);
     }
     // 2 fields in the beginning
     
     if(y==1 && chess_board[(y+2)*8+x]==EMPTY) {
      //wprintf(L"check: x=%d  y=%d\n", x, y+2);
      copy_chess_board(chess_board);
      add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_PAWN, x, y, x, y+2, control);
      //wprintf(L"got: x=%d  y=%d\n", x, y+2);
     }
     }
     // capture - left
     if(x>0) { 
       chess_figure_t figure = chess_board[(y+1)*8+x-1];
       if(figure!=EMPTY && (control || is_black(figure))) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_PAWN, x, y, x-1, y+1, control);
       }
     }
     // capture - right
     if(x<7) { 
       chess_figure_t figure = chess_board[(y+1)*8+x+1];
       if(figure!=EMPTY && (control || is_black(figure))) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, W_PAWN, x, y, x+1, y+1, control);
       }
     }
     break;
   case B_PAWN:
     if(!control) {
     // 1 position straight
     if(chess_board[(y-1)*8+x]==EMPTY) {
      copy_chess_board(chess_board);
      add_legal_move(&legal_moves, chess_board_copy, field_control_copy, B_PAWN, x, y, x, y-1, control);
     }
     // 2 fields in the beginning
     
     if(y==6 && chess_board[(y-2)*8+x]==EMPTY) {
      copy_chess_board(chess_board);
      add_legal_move(&legal_moves, chess_board_copy, field_control_copy, B_PAWN, x, y, x, y-2, control);
     }
     // capture - left
     if(x>0) { 
       chess_figure_t figure = chess_board[(y-1)*8+x-1];
       if(figure!=EMPTY && (control || is_white(figure))) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, B_PAWN, x, y, x-1, y-1, control);
       }
     }
     }
     // capture - right
     if(x<7) { 
       chess_figure_t figure = chess_board[(y-1)*8+x+1];
       if(figure!=EMPTY && (control || is_white(figure))) {
         copy_chess_board(chess_board);
         add_legal_move(&legal_moves, chess_board_copy, field_control_copy, B_PAWN, x, y, x+1, y-1, control);
       }
     }
     break;
   default: 
      wprintf(L"Error, execpting pawn: %d\n", pawn);
      exit(EXIT_FAILURE);
   }
   return legal_moves;
}

move_t get_legal_moves(chess_figure_t *chess_board, chess_figure_t figure, int x0, int y0, _Bool control)
{
move_t legal_moves;
switch(figure) {
  case W_PAWN:
  case B_PAWN:
    legal_moves = pawn_legal_moves(chess_board, figure, x0, y0, control);
    break;   
  case W_ROOK:
  case B_ROOK:
    legal_moves = rook_legal_moves(chess_board, figure, x0, y0, control);
    break;
  case W_BISHOP:
  case B_BISHOP:
    legal_moves = bishop_legal_moves(chess_board, figure, x0, y0, control);
    break;
  case W_QUEEN:
  case B_QUEEN:
    legal_moves = queen_legal_moves(chess_board, figure, x0, y0, control);
    break;
  case W_KING:
  case B_KING:
    legal_moves = king_legal_moves(chess_board, figure, x0, y0, control);
    break;
  case W_KNIGHT:
  case B_KNIGHT:
    legal_moves = knight_legal_moves(chess_board, figure, x0, y0, control);
    break;
  }
#if 0
  for(int i=0; i<legal_moves.N; i++) {
    if(legal_moves.y[i]==2) {
       wprintf(L"figure:%d [%c%d]\n", figure, 'a'+x0, 1+y0);
       exit(-1);
    }
  }
#endif
  return legal_moves;
}


