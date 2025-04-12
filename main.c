#include "include/board.h"
#include "include/move.h"
#include <stdio.h>

BOARD_T board;

int main() {
    //"r1bqk2r/ppp2ppp/2n5/1B1pP3/1b2n3/2N2Q2/PPPPN1PP/R1B1K2R b KQkq f5 5 7" 8/8/8/K2Pp2r/8/8/8/8 w - e6 0 11
    setup_board("6b1/8/8/3pP3/8/1K6/8/8 w - d6 0 1");
    print_board();
    initialise_sliding_move_tables(1);
    initialise_sliding_move_tables(0);
    MOVE_LIST_T moves;
    generate_moves(&moves);
    printf("%d\n", moves.count);
    for (int i = 0; i < moves.count; i++) {
        print_move(moves.moves[i]);
    }
    
    return 0;
}
