#include "../include/utils.h"
#include "../include/move.h"
#include "../include/board.h"
#include "../include/rng.h"
#include "../include/transposition.h"
#include <stdio.h>
#include <stdlib.h>

int engine_initialised = 0;

void graceful_exit(const char* message, int exit_code) {
    if (message)
        fprintf(stderr, "%s\n", message);

    // Cleanup.
    cleanup_transposition();

    exit(exit_code);
}

void setup_engine(__ssize_t transposition_size) {
    if (engine_initialised)
        return;
    initialise_sliding_move_tables(0);
    initialise_sliding_move_tables(1);
    initialise_hash_keys();
    initialise_transposition(transposition_size);
    engine_initialised = 1;
}

void reset_engine() {
    RESET_BOARD();
    clear_transposition();
}

void setup_board(const char *fen) {
    if (strcmp(fen, "startpos") == 0)
        fen = START_POS;
    load_fen(fen);
    board.hash_key = hash_board();
}

int parse_move(char* move_string) {
    int from_sq, to_sq, code, rank, file;
    file = move_string[0] - 'a';
    rank = (move_string[1] - '0') - 1;
    from_sq = 8*rank + file;
    file = move_string[2] - 'a';
    rank = (move_string[3] - '0') - 1;
    to_sq = 8*rank + file;
    
    // Determine the move code.
    code = 0;
    if (board.bitboards[14] & (1ULL << to_sq)) {
        code = 4;
    }

    if (move_string[4] == 'n')
        code += 8;
    if (move_string[4] == 'b')
        code += 9;
    if (move_string[4] == 'R')
        code += 10;
    if (move_string[4] == 'Q')
        code += 11;
    
    if ((board.bitboards[p] | board.bitboards[P]) & (1ULL << from_sq)) {
        if (to_sq - from_sq == 16 || to_sq - from_sq == -16)
            code = 1;
        else if (to_sq == board.enpassant)
            code = 5;
    }

    if ((board.bitboards[k] | board.bitboards[K]) & (1ULL << from_sq)) {
        if (to_sq == g1 || to_sq == g8)
            code = 2;
        else if (to_sq == c1 || to_sq == c8)
            code = 3;
    }


    // Return encoded move.
    return from_sq | (to_sq << 6) | (code << 12);
}