#include "include/defs.h"
#include "include/board_utils.h"
#include <stdio.h>

BOARDS_T board;

int main() {
    setup_board(START_POS);
    print_board();
    return 0;
}
