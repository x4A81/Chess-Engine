#ifndef BOARD_UTILS
#define BOARD_UTILS

#include <stdint.h>

#define SET_BIT(bb, sq) (bb |= (1ULL << sq))
#define GET_BIT(bb, sq) (bb & (1ULL << sq))

void print_board();
void print_bitboard(uint64_t bitboard);
void setup_board(char *fen);

#endif