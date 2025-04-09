#ifndef MOVE_H_INCLUDE
#define MOVE_H_INCLUDE

#include <stdint.h>
#include "board.h"

/*
Moves are encoded with the 4 bits as the type of move
and the last 12 bits as the to and from squares respectively.
e.g.
code to     from
0001 011100 001100

this is a double pawn push from e2 to e4
the codes are as follows:

code | Promo | capt | special 1 | sp 2 | type
0    | 0     | 0    | 0         | 0    | quiet
1    | 0     | 0    | 0         | 1    | dbp pawn
2    | 0     | 0    | 1         | 0    | king castle
3    | 0     | 0    | 1         | 1    | q castle
4    | 0     | 1    | 0         | 0    | capture
5    | 0     | 1    | 0         | 1    | ep capture
8    | 1     | 0    | 0         | 0    | knight promo
9    | 1     | 0    | 0         | 1    | bishop promo
10   | 1     | 0    | 1         | 0    | rook promo
11   | 1     | 0    | 1         | 1    | queen promo
12   | 1     | 1    | 0         | 0    | knight promo capt
13   | 1     | 1    | 0         | 1    | bishop promo capt
14   | 1     | 1    | 1         | 0    | rook promo capt
15   | 1     | 1    | 1         | 1    | queen promo capt

to get the code of the move:
(move >> 12) & code = code 
*/

typedef struct MOVE_LIST_T {
    uint16_t moves[256];
    int count;
} MOVE_LIST_T;

void initialise_sliding_move_tables(int bishop_f);
void add_move(MOVE_LIST_T *list, uint16_t move);
int is_square_attacked(int sq, int side);
void generate_moves(MOVE_LIST_T *move_list);
void make_move(uint16_t move);
#endif