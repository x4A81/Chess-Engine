#ifndef BOARD_UTILS
#define BOARD_UTILS

#include <stdint.h>
#include <string.h>

#define SET_BIT(bb, sq) (bb |= (1ULL << sq))
#define GET_BIT(bb, sq) (bb & (1ULL << sq))
#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

// Little Endian Rank-File Mapping
typedef enum SQUARES {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
} SQUARES;

typedef enum PIECES {
    p, n, b, r, q, k, P, N, B, R, Q, K
} PIECES;

typedef enum COLOURS {
    black, white
} COLOURS;

typedef enum CASTLING_RIGHTS {
    wkingside = 1, wqueenside, bkingside = 4, bqueenside = 8
} CASTLING_RIGHTS;

typedef struct BOARDS_T {
    __uint64_t bitboards[15];
    int side;
    int castling;
    int enpassant;
    int halfmove;
    int fullmove;
} BOARDS_T;

#define CLEAR_ALL_BITBOARDS() (memset(board.bitboards, 0ULL, 120))
#define RESET_BOARD() do { \
    CLEAR_ALL_BITBOARDS(); \
    board.side = -1; \
    board.castling = 0; \
    board.enpassant = -1; \
    board.halfmove = 0; \
    board.fullmove = 1; \
} while (0)

#define SAVE_BOARD() (BOARDS_T saved_board = board)
#define RESTORE_BOARD() (board = saved_board)

extern BOARDS_T board;

void print_board();
void print_bitboard(uint64_t bitboard);
void setup_board(char *fen);

#endif