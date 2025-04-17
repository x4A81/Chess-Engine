#include "include/board.h"
#include "include/move.h"
#include "tests/test.h"
#include <stdio.h>

BOARD_T board;

int main() {
    initialise_sliding_move_tables(0);
    initialise_sliding_move_tables(1);
    return 0;
}
