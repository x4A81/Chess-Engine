#include "include/board.h"
#include "include/move.h"
#include <stdio.h>

BOARD_T board;

int main() {
    //"r1bqk2r/ppp2ppp/2n5/1B1pP3/1b2n3/2N2Q2/PPPPN1PP/R1B1K2R b KQkq - 5 7"
    setup_board("4Q3/2B5/6N1/8/5P2/2Q5/8/4R3 w - - 0 1");
    print_board();
    initialise_sliding_move_tables(1);
    initialise_sliding_move_tables(0);
    is_square_attacked(e5, white);
    
    return 0;
}
