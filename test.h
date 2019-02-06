#ifndef __MIX_CHESS_TEST_H__
#define __MIX_CHESS_TEST_H__ 1

static char *check_mate[]={
"e2e4", "d7d5", "d1g4", "h7h5", "d2d3", "b8c6", "b1c3", "g8f6", "g4f4", "e7e6", "f4f6",
"d8f6", "c1h6", "h8h6", "c3d5", "e6d5", "e4d5", "f6e6", "e1d1", "e6e5", "f1e2", "e5b2",
"d1d2", "b2a1", "e2h5", "h6h5", "f2f3", "h5e5", "c2c4", "a1e1", "d2c2", "f8c5", "g1h3",
"e1h1", "g2g3", "e5e2", "c2c3", "h1b1", "g3g4", "e2c2"
};

static char *check_on_white[] = {
"e2e4", "d7d5", "d1g4", "h7h5", "d2d3", "b8c6", "b1c3", "g8f6", "g4f4", "e7e6", "f4f6",
"d8f6", "c1h6", "h8h6", "c3d5", "e6d5", "e4d5", "f6e6" 
}; 

static char *castling_possible_for_white[] = {
"e2e4", "e7e5", "f1c4", "g8f6", "g1f3", "b8c6"
};
static char *castling_not_possible_for_white[] = { 
"e2e4", "e7e5", "f1c4", "g8f6", "g1f3", "b8c6", "e1e2", /* white king moved */
"h7h5", "e2e1", "h5h4"
};

static char *castling_still_possible_for_black[] = {
"e2e4", "e7e5", "f1c4", "g8f6", "g1f3", "b8c6", "e1e2", "h7h5", "e2e1", "h5h4",
"f3h4", "f8b4"
};

static char *castling_not_possible_for_black[] = {
/* black king checked */
"e2e4", "d7d6", "c2c3", "e7e5", "d1a4", "d8d7", "h2h4", "g8h6", "g2g3", "f8e7",
"f1b5"
};

#endif /* __MIX_CHESS_TEST_H__ */
