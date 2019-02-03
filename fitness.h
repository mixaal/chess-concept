#ifndef __MIX_CHESS_FITNESS_H__
#define __MIX_CHESS_FITNESS_H__ 1

PAWN_VALUE                                 100
KNIGHT_VALUE                               521
BISHOP_VALUE                               572
ROOK_VALUE                                 824
QUEEN_VALUE                               1710
PAWN_ADVANCE_A                               3
PAWN_ADVANCE_B                               6
PASSED_PAWN_MULT                            10
DOUBLED_PAWN_PENALTY                        14
ISOLATED_PAWN_PENALTY                        8
BACKWARD_PAWN_PENALTY                        3
WEAK_SQUARE_PENALTY                          5
PASSED_PAWN_ENEMY_KING_DIST                  7
KNIGHT_SQ_MULT                               6
KNIGHT_OUTPOST_MULT                          9
BISHOP_MOBILITY                              4
BISHOP_PAIR                                 28
ROOK_ATTACK_KING_FILE                       51
ROOK_ATTACK_KING_ADJ_FILE                    8
ROOK_ATTACK_KING_ADJ_FILE_ABGH              26
ROOK_7TH_RANK                               30
ROOK_CONNECTED                               6
ROOK_MOBILITY                                4
ROOK_BEHIND_PASSED_PAWN                     40
ROOK_OPEN_FILE                              27
ROOK_SEMI_OPEN_FILE                         11
ROOK_ATCK_WEAK_PAWN_OPEN_COLUMN             15
ROOK_COLUMN_MULT                             6
QUEEN_MOBILITY                               2
KING_NO_FRIENDLY_PAWN                       35
KING_NO_FRIENDLY_PAWN_ADJ                   10
KING_FRIENDLY_PAWN_ADVANCED1                 6
KING_NO_ENEMY_PAWN                          17
KING_NO_ENEMY_PAWN_ADJ                       9
KING_PRESSURE_MULT                           4

#endif /* __MIX_CHESS_FITNESS_H__ */
