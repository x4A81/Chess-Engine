#ifndef MOVE_H_INCLUDE
#define MOVE_H_INCLUDE

#include <stdint.h>
#include "board.h"

/*
Moves are encoded with 6 bits as a special mod, 4 bits as the type of move
and the last 12 bits as the to and from squares respectively.
e.g.
special code to     from
000000  0001 011100 001100

this is a double pawn push from e2 to e4

captued pieces are encoded as follows, and are encoded in the special mod.
p  0000
n  0001
b  0010
r  0011
q  0100
k  0101
P  0110
N  0111
B  1000
R  1001
Q  1010
K  1011

c_piece will only be read if the move is a capture, and will be ignored otherwise.
if the move is a castle, the previous castling rights are stored in the special mod:
KQkq = 1111

if the move is an enpassant, the previous enpassant square is stored in the special mod.


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

*/

typedef struct MOVE_LIST_T {
    uint32_t moves[256];
    int count;
} MOVE_LIST_T;

void initialise_sliding_move_tables(int bishop_f);
void add_move(MOVE_LIST_T *move_list, int from, int to, int code, int mod);
uint64_t get_psuedo_attackers(int sq, int side);
void generate_moves(MOVE_LIST_T *move_list);
void make_move(uint32_t *move);
void unmake_move(uint32_t move);
void print_move(uint32_t move);
#endif