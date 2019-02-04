#ifndef __MIX_CHESS_FITNESS_H__
#define __MIX_CHESS_FITNESS_H__ 1

#include "chess_types.h"


#define PAWN_VALUE                                 100
#define KNIGHT_VALUE                               521
#define BISHOP_VALUE                               572
#define ROOK_VALUE                                 824
#define QUEEN_VALUE                               1710
#define PAWN_ADVANCE_A                               3
#define PAWN_ADVANCE_B                               6
#define PASSED_PAWN_MULT                            10
#define DOUBLED_PAWN_PENALTY                        14
#define ISOLATED_PAWN_PENALTY                        8
#define BACKWARD_PAWN_PENALTY                        3
#define WEAK_SQUARE_PENALTY                          5
#define PASSED_PAWN_ENEMY_KING_DIST                  7
#define KNIGHT_SQ_MULT                               6
#define KNIGHT_OUTPOST_MULT                          9
#define BISHOP_MOBILITY                              4
#define BISHOP_PAIR                                 28
#define ROOK_ATTACK_KING_FILE                       51
#define ROOK_ATTACK_KING_ADJ_FILE                    8
#define ROOK_ATTACK_KING_ADJ_FILE_ABGH              26
#define ROOK_7TH_RANK                               30
#define ROOK_CONNECTED                               6
#define ROOK_MOBILITY                                4
#define ROOK_BEHIND_PASSED_PAWN                     40
#define ROOK_OPEN_FILE                              27
#define ROOK_SEMI_OPEN_FILE                         11
#define ROOK_ATCK_WEAK_PAWN_OPEN_COLUMN             15
#define ROOK_COLUMN_MULT                             6
#define QUEEN_MOBILITY                               2
#define KING_NO_FRIENDLY_PAWN                       35
#define KING_NO_FRIENDLY_PAWN_ADJ                   10
#define KING_FRIENDLY_PAWN_ADVANCED1                 6
#define KING_NO_ENEMY_PAWN                          17
#define KING_NO_ENEMY_PAWN_ADJ                       9
#define KING_PRESSURE_MULT                           4

float evaluate_position(chess_figure_t *chess_board, int white_is_playing);
float mobility_evaluation(chess_figure_t *chess_board, int white_is_playing);


#endif /* __MIX_CHESS_FITNESS_H__ */
