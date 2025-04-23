#include "test.h"
#include "../include/move.h"
#include "../include/board.h"
#include <stdio.h>

// Define perft_nodes
long perft_nodes = 0L;

void perft(int depth) {
    if (depth == 0) {
        perft_nodes++;
        return;
    }

    MOVE_LIST_T moves;
    generate_moves(&moves, 0);

    // The position is stalemate or checkmate.
    if (moves.count == 0){
        return;
    }

    for (int idx = 0; idx < moves.count; idx++) {
        int move = moves.moves[idx];
        SAVE_BOARD();
        make_move(move);
        perft(depth - 1);
        RESTORE_BOARD();
    }
}

void perft_divide(int depth) {
    if (depth == 0) {
        perft_nodes++;
        return;
    }

    MOVE_LIST_T moves;
    generate_moves(&moves, 0);

    // The position is stalemate or checkmate.
    if (moves.count == 0){
        return;
    }

    for (int idx = 0; idx < moves.count; idx++) {
        int move = moves.moves[idx];
        SAVE_BOARD();
        make_move(move);
        long copy_perft_nodes = perft_nodes;
        perft(depth - 1);
        print_move(move);
        printf(": %ld\n", perft_nodes - copy_perft_nodes);
        RESTORE_BOARD();
    }
}

void perft_test(int start_depth, int max_depth, int test_f) {
    for (int depth = start_depth; depth <= max_depth; depth++) {
        perft_nodes = 0L;
        if (test_f)
            perft(depth);
        else
            perft_divide(depth);

        // Print debug info.
        printf("\nDepth %d\nNodes: %ld\n", depth, perft_nodes);
    }
}