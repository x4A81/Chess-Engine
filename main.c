#include "include/board.h"
#include "include/move.h"
#include "tests/test.h"

BOARD_T board;

int main() {
    initialise_sliding_move_tables(0);
    initialise_sliding_move_tables(1);
    int depth = 6;

    printf("pos 1\n");
    setup_board(START_POS);
    perft_test(1, 6, 1);
    // Passes

    printf("pos 2\n");
    setup_board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    perft_test(1, 5, 1);
    // Passes

    printf("pos 3\n");
    setup_board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    perft_test(1, 7, 1);
    // Passes at depth 5
    printf("pos 4\n");
    
    setup_board("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    perft_test(1, 6, 1);
    // Passes
    
    printf("pos 5\n");
    setup_board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    perft_test(1, 5, 1);
    // Passes

    printf("pos 6\n");
    setup_board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    perft_test(1, 5, 1);
    // Passes
    return 0;
}
