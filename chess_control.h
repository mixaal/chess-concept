#ifndef __MIX_CHESS_CONTROL_H__
#define __MIX_CHESS_CONTROL_H__ 1

#include "chess_types.h"

void chess_init(chess_t *chess);
void is_game_over(chess_t *chess);
void board_init(chess_figure_t *chess);
void next_move(chess_t *chess, get_input_t read_input);

#endif /* __MIX_CHESS_CONTROL_H__ */
