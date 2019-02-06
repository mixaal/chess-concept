#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "chess_types.h"
#include "legal_moves.h"

#include "board.h" //FIXME: Remove me!!!!

static volatile int _trace = False;

void volatile set_trace(int t)
{
  _trace = t;
}

_Bool is_white(chess_figure_t figure)
{
  return figure >= W_PAWN && figure <= W_KING;
}

_Bool is_black(chess_figure_t figure)
{
  return figure >= B_PAWN && figure <= B_KING;
}

_Bool white_king_check(chess_t *chess)
{
  int king_x, king_y;
#if 0
  for(int i=0; i<64; i++) {
     if(chess->board[i]==W_KING) {
        king_x = i % 8;
        king_y = i / 8;
        break;
     }
  }
#endif
  king_x = chess -> white_king_x;
  king_y = chess -> white_king_y;
  if(_trace) wprintf(L"white_king_check(): chess=%p [%c%d]\n", chess, king_x+'a', king_y+1); 
  int control = chess -> field_control[king_x + 8*king_y] & CTRL_MASK;
  if (control&CTRL_BLACK) {
    if(_trace) wprintf(L"white_king_checked!!! for chess=%p\n", chess);
    chess -> white_king_checked = True;
    return True;
  }
  return False;
}

_Bool black_king_check(chess_t *chess)
{
  int king_x, king_y;
#if 0
  for(int i=0; i<64; i++) {
     if(chess->board[i]==B_KING) {
        king_x = i % 8;
        king_y = i / 8;
        break;
     }
  }
#endif

  king_x = chess -> black_king_x;
  king_y = chess -> black_king_y;
  int control = chess -> field_control[king_x + 8*king_y] & CTRL_MASK;
  if (control&CTRL_WHITE) {
    chess -> black_king_checked = True;
    return True;
  }
  return False;
}


void set_white_king(chess_t *chess, int x0, int y0, int x1, int y1)
{
   if(x0==x1 && y0==y1) return;
   chess -> board[x1 + 8*y1] = W_KING;
   chess -> white_king_x = x1;
   chess -> white_king_y = y1;
   chess -> white_king_moved  = True;
}

void set_black_king(chess_t *chess, int x0, int y0, int x1, int y1)
{
   if(x0==x1 && y0==y1) return;
   chess -> board[x1 + 8*y1] = B_KING;
   chess -> black_king_x = x1;
   chess -> black_king_y = y1;
   chess -> black_king_moved  = True;
}

void do_chess_move(chess_t *chess, int x0, int y0, int x1, int y1, _Bool real_board)
{
   chess_figure_t *chess_board = chess -> board;
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
   if(figure==W_KING) set_white_king(chess, x0, y0, x1, y1);
   if(figure==B_KING) set_black_king(chess, x0, y0, x1, y1);


   chess_board[8*y0 + x0] = EMPTY;
   chess_board[8*y1 + x1] = figure;
}

static void add_legal_move(move_t *legal_moves, chess_t *chess, chess_figure_t my, int orig_x, int orig_y, int tx, int ty, _Bool control)
{
   if(control) goto ADD_MOVE;
   if(_trace) wprintf(L"add_legal_move(): do_chess_move(): [%c%d] => [%c%d]\n", orig_x+'a', orig_y+1, tx+'a', ty+1);
   do_chess_move(chess, orig_x, orig_y, tx, ty, False);
   if(_trace) wprintf(L"add_legal_move(): compute_field_control\n");
   compute_field_control(chess);
   if(_trace) {
     wprintf(L"add_legal_move(): print_field_control: chess=%p\n", chess);
     print_field_control(chess->field_control);
     wprintf(L"add_legal_move(): determine legality\n");
     wprintf(L"add_legal_move(): white_king_check=%d\n", white_king_check(chess));
     wprintf(L"add_legal_move(): is_white(my)=%d\n", is_white(my));
     wprintf(L"add_legal_move(): black_king_check=%d\n", black_king_check(chess));
     wprintf(L"add_legal_move(): is_black(my)=%d\n", is_black(my));
    }
   _Bool legal = (is_white(my) && !white_king_check(chess)) || (is_black(my) && !black_king_check(chess)) ;

   if(_trace) wprintf(L"add_legal_move(): legal=%d\n", legal);
  if(!legal) return;
ADD_MOVE:
  legal_moves->x[legal_moves->N] = tx;
  legal_moves->y[legal_moves->N] = ty;
  legal_moves->N ++;

}

static inline chess_t copy_chess_board(chess_t * chess)
{
   chess_t copy = *chess;
   //wprintf(L"copy_chess_board: chess->white_king_checked=%d copy=%d\n", chess->white_king_checked, copy.white_king_checked);
   memcpy(copy.board, chess->board, 64 * sizeof(chess_figure_t));
   memcpy(copy.field_control, chess->field_control, 64*sizeof(int));
   return copy;
}

static _Bool figure_move(move_t *legal_moves, chess_t *chess, chess_figure_t my, int orig_x, int orig_y, int tx, int y, _Bool control)
{
     /**
      * Create a copy of a chess board a and try the move.
      */
     if(!control && _trace) wprintf(L"figure_move: [%c%d] => [%c%d]\n", 'a'+orig_x,1+ orig_y, 'a'+tx, 1+y);
     chess_t chess_copy = copy_chess_board(chess);
     if(tx<0 || tx>7 || y<0 || y>7) return 0;
     chess_figure_t other = chess->board[tx+8*y];
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
         add_legal_move(legal_moves, &chess_copy, my, orig_x, orig_y, tx, y, control);
         return 1;
       }
     }
     if(other!=EMPTY) {
        wprintf(L"Error: other==%d, while expecting empty\n", other);
        exit(EXIT_FAILURE);
     }
     //wprintf(L"legal move: %d %d\n", tx, y);
     add_legal_move(legal_moves, &chess_copy, my, orig_x, orig_y, tx, y, control);
     return 0;
}
static void __rook_legal_moves(move_t *legal_moves, chess_t *chess, chess_figure_t rook, int x, int y, _Bool control)
{
  
   // to the right
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     if(tx>7) continue; // out-of-range
     if(figure_move(legal_moves, chess, rook, x, y, tx, y, control)) break;
   }
   //wprintf(L"check the right: %d\n", legal_moves.N);

   //to the left
   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     if(tx<0) continue; // out-of-range
     if(figure_move(legal_moves, chess, rook, x, y, tx, y, control)) break;
   }
   //wprintf(L"check the left: %d\n", legal_moves.N);

   // up 
   for(int dx=1; dx<=7; dx++) {
     int ty=y-dx;
     if(ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess, rook, x, y, x, ty, control)) break;
   }
   //wprintf(L"check the up: %d\n", legal_moves.N);
   // down
   for(int dx=1; dx<=7; dx++) {
     int ty=y+dx;
     if(ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess, rook, x, y, x, ty, control)) break;
   }
   //wprintf(L"check the down: %d\n", legal_moves.N);
} 


static void __bishop_legal_moves(move_t *legal_moves, chess_t *chess, chess_figure_t bishop, int x, int y, _Bool control) {
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     int ty=y+dx;
     if(tx>7 || ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess, bishop, x, y, tx, ty, control)) break;
   }

   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     int ty=y-dx;
     if(tx<0 || ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess, bishop, x, y, tx, ty, control)) break;
   }
 
   for(int dx=1; dx<=7; dx++) {
     int tx=x+dx;
     int ty=y-dx;
     if(tx>7 || ty<0) continue; // out-of-range
     if(figure_move(legal_moves, chess, bishop, x, y, tx, ty, control)) break;
   }

   for(int dx=1; dx<=7; dx++) {
     int tx=x-dx;
     int ty=y+dx;
     if(tx<0 || ty>7) continue; // out-of-range
     if(figure_move(legal_moves, chess, bishop, x, y, tx, ty, control)) break;
   }
}

move_t knight_legal_moves(chess_t *chess, chess_figure_t knight, int x, int y, _Bool control)
{
   move_t legal_moves;
   legal_moves.N = 0;
   figure_move(&legal_moves, chess, knight, x, y, x+1, y-2, control);
   figure_move(&legal_moves, chess, knight, x, y, x-1, y-2, control);
   figure_move(&legal_moves, chess, knight, x, y, x+2, y-1, control);
   figure_move(&legal_moves, chess, knight, x, y, x-2, y-1, control);

   figure_move(&legal_moves, chess, knight, x, y, x+1, y+2, control);
   figure_move(&legal_moves, chess, knight, x, y, x-1, y+2, control);
   figure_move(&legal_moves, chess, knight, x, y, x+2, y+1, control);
   figure_move(&legal_moves, chess, knight, x, y, x-2, y+1, control);
   return legal_moves;
}

move_t king_legal_moves(chess_t *chess, chess_figure_t king, int x, int y, _Bool control)
{

   move_t legal_moves;
   legal_moves.N = 0;
   if(_trace) wprintf(L"king_legal_moves: start: control=%d\n", control);
   figure_move(&legal_moves, chess, king, x, y, x+1, y, control);
   figure_move(&legal_moves, chess, king, x, y, x, y+1, control);
   figure_move(&legal_moves, chess, king, x, y, x-1, y, control);
   figure_move(&legal_moves, chess, king, x, y, x, y-1, control);

   figure_move(&legal_moves, chess, king, x, y, x-1, y-1, control);
   figure_move(&legal_moves, chess, king, x, y, x-1, y+1, control);
   figure_move(&legal_moves, chess, king, x, y, x+1, y-1, control);
   figure_move(&legal_moves, chess, king, x, y, x+1, y+1, control);
   if(_trace) wprintf(L"king_legal_moves: end: control=%d\n", control);

   if(control) return legal_moves; // FIXME: is this correct?
  chess_figure_t *chess_board = chess->board;
   //FIXME rook moved?
   //wprintf(L"white_king_checked=%d white_king_moved=%d\n", white_king_checked, white_king_moved);
   if(!chess->white_king_checked && !chess->white_king_moved && is_white(king)) {
      if(x==4 && y==0 && chess_board[5]==chess_board[6] && chess_board[5]==EMPTY && chess_board[7]==W_ROOK) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, W_KING, 4, 0, 6, 0, control);
     }
      if(x==4 && y==0 && chess_board[1]==chess_board[2] && chess_board[2]==chess_board[3] && chess_board[1]==EMPTY && chess_board[0]==W_ROOK) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, W_KING, 4, 0, 1, 0, control);
       }
   }
   if(!chess->black_king_checked && !chess->black_king_moved && is_black(king)) {
      if(x==4 && y==7 && chess_board[61]==chess_board[62] && chess_board[61]==EMPTY && chess_board[63]==B_ROOK) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, B_KING, 4, 7, 6, 7, control);
     }
      if(x==4 && y==7 && chess_board[57]==chess_board[58] && chess_board[58]==chess_board[59] && chess_board[58]==EMPTY && chess_board[56]==B_ROOK) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, B_KING, 4, 7, 1, 7, control);
     }
   }
 
   return legal_moves;
}


move_t queen_legal_moves(chess_t *chess, chess_figure_t queen, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __bishop_legal_moves(&legal_moves, chess, queen, x, y, control);  
   __rook_legal_moves(&legal_moves, chess, queen, x, y, control);  
   return legal_moves;
}


move_t bishop_legal_moves(chess_t *chess, chess_figure_t bishop, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __bishop_legal_moves(&legal_moves, chess, bishop, x, y, control);  
   return legal_moves;
}

move_t rook_legal_moves(chess_t *chess, chess_figure_t rook, int x, int y, _Bool control)
{
  
   move_t legal_moves;
   legal_moves.N = 0;
   __rook_legal_moves(&legal_moves, chess, rook, x, y, control);
   return legal_moves;
} 

move_t pawn_legal_moves(chess_t *chess, chess_figure_t pawn, int x, int y, _Bool control)
{
   move_t legal_moves;
   legal_moves.N = 0;
   chess_figure_t * chess_board = chess -> board;
   //wprintf(L"init: x=%d  y=%d\n", x, y);
   switch(pawn) {
   case W_PAWN:
     // 1 position straight
     if(!control) {
     if(chess_board[(y+1)*8+x]==EMPTY) {
      //wprintf(L"check: x=%d  y=%d\n", x, y+1);
      
       chess_t chess_copy = copy_chess_board(chess);
       add_legal_move(&legal_moves, &chess_copy, W_PAWN, x, y, x, y+1, control);
      //wprintf(L"got : x=%d  y=%d\n", x, y+1);
     }
     // 2 fields in the beginning
     
     if(y==1 && chess_board[(y+2)*8+x]==EMPTY) {
      //wprintf(L"check: x=%d  y=%d\n", x, y+2);
       chess_t chess_copy = copy_chess_board(chess);
       add_legal_move(&legal_moves, &chess_copy, W_PAWN, x, y, x, y+2, control);
      //wprintf(L"got: x=%d  y=%d\n", x, y+2);
     }
     }
     // capture - left
     if(x>0) { 
       chess_figure_t figure = chess_board[(y+1)*8+x-1];
       if(figure!=EMPTY && (control || is_black(figure))) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, W_PAWN, x, y, x-1, y+1, control);
       }
     }
     // capture - right
     if(x<7) { 
       chess_figure_t figure = chess_board[(y+1)*8+x+1];
       if(figure!=EMPTY && (control || is_black(figure))) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, W_PAWN, x, y, x+1, y+1, control);
       }
     }
     break;
   case B_PAWN:
     if(!control) {
     // 1 position straight
     if(chess_board[(y-1)*8+x]==EMPTY) {
       chess_t chess_copy = copy_chess_board(chess);
       add_legal_move(&legal_moves, &chess_copy, B_PAWN, x, y, x, y-1, control);
     }
     // 2 fields in the beginning
     
     if(y==6 && chess_board[(y-2)*8+x]==EMPTY) {
       chess_t chess_copy = copy_chess_board(chess);
       add_legal_move(&legal_moves, &chess_copy, B_PAWN, x, y, x, y-2, control);
     }
     // capture - left
     if(x>0) { 
       chess_figure_t figure = chess_board[(y-1)*8+x-1];
       if(figure!=EMPTY && (control || is_white(figure))) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, B_PAWN, x, y, x-1, y-1, control);
       }
     }
     }
     // capture - right
     if(x<7) { 
       chess_figure_t figure = chess_board[(y-1)*8+x+1];
       if(figure!=EMPTY && (control || is_white(figure))) {
         chess_t chess_copy = copy_chess_board(chess);
         add_legal_move(&legal_moves, &chess_copy, B_PAWN, x, y, x+1, y-1, control);
       }
     }
     break;
   default: 
      wprintf(L"Error, execpting pawn: %d\n", pawn);
      exit(EXIT_FAILURE);
   }
   return legal_moves;
}

move_t get_legal_moves(chess_t *chess, chess_figure_t figure, int x0, int y0, _Bool control)
{
move_t legal_moves;
switch(figure) {
  case W_PAWN:
  case B_PAWN:
    legal_moves = pawn_legal_moves(chess, figure, x0, y0, control);
    break;   
  case W_ROOK:
  case B_ROOK:
    legal_moves = rook_legal_moves(chess, figure, x0, y0, control);
    break;
  case W_BISHOP:
  case B_BISHOP:
    legal_moves = bishop_legal_moves(chess, figure, x0, y0, control);
    break;
  case W_QUEEN:
  case B_QUEEN:
    legal_moves = queen_legal_moves(chess, figure, x0, y0, control);
    break;
  case W_KING:
  case B_KING:
    legal_moves = king_legal_moves(chess, figure, x0, y0, control);
    break;
  case W_KNIGHT:
  case B_KNIGHT:
    legal_moves = knight_legal_moves(chess, figure, x0, y0, control);
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
void compute_field_control(chess_t *chess)
{
  //memset(field_control, 0, 64*sizeof(int));
  chess_figure_t *chess_board = chess->board;
  int *field_control = chess -> field_control;
  for(int i=0; i<64; i++) {
      if(is_white(chess_board[i])) { field_control[i]=PLACE_WHITE;  continue;}
      if(is_black(chess_board[i])) { field_control[i]=PLACE_BLACK;  continue;}
      field_control[i]=CTRL_NOBODY;
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
    move_t legal_moves = get_legal_moves(chess, chess->board[i], x , y, True);
    for(int j=0; j<legal_moves.N; j++) {
       int offset = legal_moves.x[j] + legal_moves.y[j] * 8;
       field_control[offset] |= mask;
    }
  }
}



