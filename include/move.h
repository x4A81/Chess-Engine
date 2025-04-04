#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>

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

typedef enum MOVE_TYPES {
    quiet, dbp_pawn, king_castle, queen_castle, capture, ep_capture,
    knight_promo = 8, bishop_promo, rook_promo, queen_promo,
    knight_promo_capture, bishop_promo_capture, rook_promo_capture, queen_promo_capture
} MOVE_TYPES;

#define GET_FROM(move) ((move) & 0b111111)
#define GET_TO(move) (((move) >> 6) & 0b111111)

void add_move(MOVE_LIST_T *list, uint16_t move);

void generate_moves(int side);
void make_move(uint16_t move);

void initialise_pawn_move_table();
void initialise_knight_move_table();
void initialise_king_move_table();

extern uint64_t pawn_move_table[2][64];
extern uint64_t knight_move_table[64];
extern uint64_t king_move_table[64];

#endif