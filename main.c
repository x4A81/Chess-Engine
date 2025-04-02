#include "include/defs.h"
#include "include/board_utils.h"
#include <stdio.h>

BOARDS_T board;

int main() {
    for (int bb = p; bb <= K; bb++) {
        board.bitboards[bb] = 0ULL;
    }
    
    board.bitboards[p] = SET_BITBOARD(board.bitboards[p], a4);
    // CLEAR_ALL_BITBOARDS();

    print_bitboard(board.bitboards[p]);
    print_board();
    return 0;
}
