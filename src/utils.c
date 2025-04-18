#include "../include/utils.h"
#include "../include/move.h"
#include "../include/board.h"
#include <stdio.h>
#include <stdlib.h>

int engine_initialised = 0;

void graceful_exit(const char* message, int exit_code) {
    if (message)
        fprintf(stderr, "%s\n", message);

    // Cleanup


    exit(exit_code);
}

void setup_engine() {
    if (engine_initialised)
        return;
    initialise_sliding_move_tables(0);
    initialise_sliding_move_tables(1);
    engine_initialised = 1;
}

void reset_engine() {
    RESET_BOARD();
}

void setup_board(const char *fen) {
    if (fen == "startpos")
        fen = START_POS;
    parse_fen(fen);
    print_board();
}