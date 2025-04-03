#include "include/board.h"
#include <stdio.h>

BOARDS_T board;

int main() {
    setup_board("r1bqk2r/ppp2ppp/2n5/1B1pP3/1b2n3/2N2Q2/PPPPN1PP/R1B1K2R b KQkq - 5 7");
    print_board();
    return 0;
}
