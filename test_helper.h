#ifndef __MIX_TEST_HELPER_H__
#define __MIX_TEST_HELPER_H__ 1

#include "chess_types.h"

int get_rc(void);
void assert_ieq(int expected, int got, const char *msg);
void test_play(chess_t *chess, char *moves[], size_t move_no, _Bool display);

#endif /* __MIX_TEST_HELPER_H__ */
