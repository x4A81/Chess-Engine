#include "../../include/search.h"
#include "../../include/move.h"
#include "../../include/board.h"
#include "../../include/eval.h"
#include "../../include/uci.h"
#include <stdio.h>

long ply = 0, nodes = 0;
int pv_table[(MAX_PLY*MAX_PLY+MAX_PLY)/2];
int pv_length[MAX_PLY];

int quiescence(int alpha, int beta) {
    return eval();
}

int negamax(int depth, int alpha, int beta) {

    // If the search was stopped or time ran out.
    if (STOP_SEARCH || time_exceeded())
        return alpha; // Return the best score so far.

    if (depth == 0)
        return quiescence(alpha, beta);

    MOVE_LIST_T move_list;
    generate_moves(&move_list);

    // Checkmate and stalemate detection.
    if (move_list.count == 0) {
        // Position is checkmate or stalemate.

        if (is_check(board.side))
            return -INF; // Checkmate.
        else
            return 0; // Stalemate.
    }

    // Set up PV.
    int pv_index = (ply*(2*MAX_PLY+1-ply))/2;
    int next_pv_index = pv_index + MAX_PLY - ply;
    pv_length[ply] = 0;
    pv_table[pv_index] = 0;

    for (int i = 0; i < move_list.count; i++) {
        int move = move_list.moves[i];

        SAVE_BOARD();
        make_move(move);
        ply++;
        nodes++;

        // Run alpha beta search.
        int score = -negamax(depth - 1, -beta, -alpha);
        
        ply--;
        RESTORE_BOARD();

        // Fail hard beta cutoff.
        if (score >= beta) {
            return beta;
        }

        // Pv node.
        if (score > alpha) {
            alpha = score;

            // Update PV table.
            pv_table[pv_index] = move;

            // Propagate up pv table
            for (int j = 0; j < pv_length[ply+1]; j++)
                pv_table[pv_index + j + 1] = pv_table[next_pv_index + j];

            // Update pv length
            pv_length[ply] = pv_length[ply+1] + 1;

        }
    }

    return alpha;
}

void search(int depth) {

    /*
    1. Use iterative deepening.
    2. Call negamax search.
    */

    int alpha = -INF;
    int beta = INF;
    ply = 0;
   
    // Step 1. Iterative deepening.
    for (int d = 1; d <= depth || INFINITE; d++) {
        nodes = 0;

        int score = negamax(d, alpha, beta);

        // Print debug info.
        if (DEBUG) {
            printf("info depth %d nodes %d score cp %d pv ", d, nodes, score);
            for (int i = 0; i < pv_length[0]; i++) {
                print_move(pv_table[i]);
                printf(" ");
            }

            printf("\n");
            fflush(stdout);
        }

        if (STOP_SEARCH || time_exceeded())
            break;
        
    }

    printf("bestmove ");
    print_move(pv_table[0]);
    printf("\n");
    fflush(stdout);
}