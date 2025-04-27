#ifndef MOVE_H_INCLUDE
#define MOVE_H_INCLUDE

#include <stdint.h>
#include "board.h"

#define GET_FROM(move) (move & 0b111111)
#define GET_TO(move) ((move >> 6) & 0b111111)
#define GET_CODE(move) ((move >> 12) & 0b1111)
#define IS_CAPT(move) ((move >> 12) & 0b100)

typedef enum MOVE_TYPES {
    quiet, dbl_pawn, king_castle, queen_castle, capture, ep_capture,
    knight_promo = 8, bishop_promo, rook_promo, queen_promo,
    knight_promo_capture, bishop_promo_capture, rook_promo_capture, queen_promo_capture
} MOVE_TYPES;

/*
Moves are encoded with 4 bits as the type of move
and the last 12 bits as the to and from squares respectively.
e.g.
code to     from
000000  0001 011100 001100

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

*/

typedef struct MOVE_LIST_T {
    uint32_t moves[256];
    int count;
} MOVE_LIST_T;

// Initialises the move tables for rooks and bishops.
void initialise_sliding_move_tables(int bishop_f);

// Adds a move to a move list.
static inline void add_move(MOVE_LIST_T *move_list, int from, int to, int code);

// Returns the psuedo legal attackers of a square belonging to side.
uint64_t get_psuedo_attackers(int sq, int side);

// Returns 1 if the side is in check.
int is_check(int side);

// Generates moves to a move list and only generates captures if gen_capts is set to 1.
void generate_moves(MOVE_LIST_T *move_list);

// Makes a move on the global board.
void make_move(uint16_t move);

// Prints a move in algebraic notation.
void print_move(uint16_t move);
#endif