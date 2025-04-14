#include "include/board.h"
#include "include/move.h"
#include "tests/test.h"

BOARD_T board;

int main() {
    setup_board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    print_board();
    initialise_sliding_move_tables(1);
    initialise_sliding_move_tables(0);
    int depth = 1;
    perft_test(1, depth, 0); 
    return 0;
}
