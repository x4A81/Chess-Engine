#include "include/board.h"
#include "include/move.h"
#include <stdio.h>

BOARDS_T board;

int main() {
    setup_board("r1bqk2r/ppp2ppp/2n5/1B1pP3/1b2n3/2N2Q2/PPPPN1PP/R1B1K2R b KQkq - 5 7");
    // generate_pawn_move_table();
    for (int i = 8; i < 56; i++) {
        print_bitboard(pawn_move_table[i][0]);
        print_bitboard(pawn_move_table[i][1]);
    }

    return 0;
}
