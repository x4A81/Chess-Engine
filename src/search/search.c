#include "../../include/search.h"
#include "../../include/move.h"
#include "../../include/board.h"
#include "../../include/eval.h"
#include "../../include/uci.h"
#include "../../include/transposition.h"
#include "../../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

long ply = 0, nodes = 0;
int pv_table[(MAX_PLY*MAX_PLY+MAX_PLY)/2];
int pv_length[MAX_PLY];
int killer_moves[MAX_PLY][2] = { 0 };
int history_moves[2][64][64] = { 0 };
int fall_back_best_move = 0;

uint64_t repetition_his[MAX_PLY];
int repetition_his_length = 0;

// Flags
int flags = 0;
#define STOPPED_SEARCH 0b1

static inline int is_repetition() {
    if (board.halfmove >= 50)
        return 1;

    for (int i = 0; i <= repetition_his_length - 2; i++) {
        if (repetition_his[i] == board.hash_key)
            return 1;
    }

    return 0;
}

void reset_search() {
    ply = 0;
    nodes = 0;
    repetition_his_length = 0;
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(repetition_his, 0, sizeof(repetition_his));
}

void sort_moves(MOVE_LIST_T *move_list, int hash_move, int ply) {
    /*
    1. Hash moves
    2. PV moves
    3. Promotions
    4. MVA/LVA
    5. Killer Moves
    6. History Moves
    */

    // MVA/LVA Table
    int mva_lva_table[5][6] = {
        { 15, 14, 13, 12, 11, 10 },
        { 25, 24, 23, 22, 21, 20 },
        { 35, 34, 33, 32, 31, 30 },
        { 45, 44, 43, 42, 41, 40 },
        { 55, 54, 53, 52, 51, 50 },
    };

    // Score each move.
    int scores[move_list->count];
    for (int i = 0; i < move_list->count; i++) {
        // Hash move.
        if (move_list->moves[i] == hash_move) {
            scores[i] = 100;
        }

        // PV move
        else if (move_list->moves[i] == pv_table[(ply*(2*MAX_PLY+1-ply))/2]) {
            scores[i] = 900;
        }

        // Promotions
        else if (GET_CODE(move_list->moves[i]) >= knight_promo && GET_CODE(move_list->moves[i]) <= queen_promo) {
            scores[i] = 800;
        }

        else if (GET_CODE(move_list->moves[i]) >= knight_promo_capture) {
            // Get the attacker.
            int att;
            for (att = p; att <= K; att++) {
                if (GET_BIT(board.bitboards[att], GET_FROM(move_list->moves[i])))
                    break;
            }
            
            // Get the victim.
            int victim;
            for (victim = p; victim <= K; victim++) {
                if (GET_BIT(board.bitboards[victim], GET_FROM(move_list->moves[i])))
                    break;
            }

            scores[i] = 800 + mva_lva_table[victim][att];
        }

        else if (move_list->moves[i] == killer_moves[ply][0]) {
            scores[i] = 700;
        }

        else if (move_list->moves[i] == killer_moves[ply][1]) {
            scores[i] = 600;
        }

        else
            scores[i] = history_moves[board.side][GET_FROM(move_list->moves[i])][GET_TO(move_list->moves[i])];

    }

    // Sort moves by score.
    for (int i = 0; i < move_list->count - 1; i++) {
        for (int j = i + 1; j < move_list->count; j++) {
            if (scores[i] < scores[j]) {
                // Swap moves.
                int temp = move_list->moves[i];
                move_list->moves[i] = move_list->moves[j];
                move_list->moves[j] = temp;

                // Swap scores.
                int temp_score = scores[i];
                scores[i] = scores[j];
                scores[j] = temp_score;
            }
        }
    }
}

int quiescence(int alpha, int beta) {
    int stand_pat = eval();
    int best_val = stand_pat;
    if (stand_pat >= beta)
        return stand_pat;
    if (stand_pat > alpha)
        alpha = stand_pat;

    MOVE_LIST_T move_list;
    generate_moves(&move_list);
    sort_moves(&move_list, 0, ply);
    
    // Checkmate and stalemate detection.
    if (move_list.count == 0) {
        // Position is checkmate or stalemate.

        if (is_check(board.side))
            return -INF; // Checkmate.
        else
            return 0; // Stalemate.
    }

    if (STOP_SEARCH || time_exceeded()) {
        flags |= STOPPED_SEARCH;
        return best_val;
    }

    for (int i = 0; i < move_list.count; i++) {
        int move = move_list.moves[i];
        if (!IS_CAPT(move))
            continue;

        SAVE_BOARD();
        make_move(move);
        ply++;
        nodes++;

        // Run alpha beta search.
        int score = -quiescence(-beta, -alpha);
        
        ply--;
        RESTORE_BOARD();

        // Score was not properly calculated as search stopped.
        if (flags & STOPPED_SEARCH)
            break;

        if (score >= beta)
            return score;
        if (score > best_val)
            best_val = score;
        if (score > alpha )
            alpha = score;
    }

    return best_val;
}

int negamax(int depth, int alpha, int beta) {

    /*
    Search Algorithm enhancements ontop of fail-hard alpha beta:
    1. Quiescence search at horizon.
    2. Repetition Check.
    3. Transpotition table cutoffs.
    4. Null move reductions.
    5. Move ordering.
    6. Fultility Pruning.
    7. Late move reductions.
    8. Principle Variation Search.

    NB. Check extensions at start and when making moves.
    */

    // Quiescence search at horizon.
    if (depth <= 0)
        return quiescence(alpha, beta);

    int score = 0, node_type = (beta-alpha) > 1 ? EXACT : UPPERBOUND;
    int moves_searched = 0;
    int static_eval = eval();
    int draw_score = -eval() / 2;

    // Fall back if pv is empty.
    int best_score = -INF;

    // Set up PV.
    int pv_index = (ply*(2*MAX_PLY+1-ply))/2;
    int next_pv_index = pv_index + MAX_PLY - ply;

    // Generate moves.
    MOVE_LIST_T move_list;
    generate_moves(&move_list);

    // Checkmate and stalemate detection.
    if (move_list.count == 0) {
        // Position is checkmate or stalemate.

        if (is_check(board.side))
            return -INF; // Checkmate.
        else
            return draw_score; // Stalemate.
    }

    if (is_repetition())
        return draw_score;

    // Transposition table cutoffs.
    TRANSPOSITION_T *entry = probe_transposition(board.hash_key, depth);
    if (entry && pv_table[pv_index] != 0) {
        if (entry->type == EXACT)
            return entry->score;
        else if (entry->type == LOWERBOUND && entry->score >= beta)
            return entry->score;
        else if (entry->type == UPPERBOUND && entry->score < alpha)
            return entry->score;
    }

    // If the search was stopped or time ran out.
    if (STOP_SEARCH || time_exceeded()) {
        flags |= STOPPED_SEARCH;
        return alpha;
    }

    // Move ordering.
    sort_moves(&move_list, entry ? entry->hash_move : 0, ply);
    
    // Null move reductions.
    if (!is_check(board.side)) {
        int R = depth > 6 ? 4 : 3;
        SAVE_BOARD();
        make_move(0);
        ply++;
        nodes++;
        score = -negamax(depth-R-1, 0-beta, 1 - beta);
        ply--;
        RESTORE_BOARD();
        if (score >= beta) {
            depth -= 4;
            if (depth <= 0)
                return quiescence(alpha, beta);
        }
    }
    
    // Fultility pruning flags.
    int can_f_prune = 1;
    int fultility_margin = 125 * depth * depth;
    if (depth > 2 || is_check(board.side) || pv_table[pv_index] == 0)
        can_f_prune = 0;

    for (int i = 0; i < move_list.count; i++) {
        int move = move_list.moves[i];
        
        SAVE_BOARD();
        make_move(move);
        
        // Fultility pruning.
        if (can_f_prune) {

            // Search moves that aren't captures or checks.
            if (!IS_CAPT(move) && !is_check(board.side)) {
                if (static_eval + fultility_margin < alpha) {
                    
                    // Prune the move.
                    RESTORE_BOARD();
                    continue;
                }
                
            }
        }
        
        ply++;
        nodes++;
        moves_searched++;
        repetition_his[ply] = board.hash_key;
        repetition_his_length = ply;

        // Run alpha beta search.
        if (i == 0)
            score = -negamax(depth - 1, -beta, -alpha);
        else {

            // Late move reductions. (researches if fails high)
            int reduced = depth;
            if (i >= 3)
                reduced -= 2;

            // Principle Variation Search. (researches if score > alpha)
            score = -negamax(reduced - 1, -(alpha + 1), -alpha);

            // Research position.
            if (score > alpha && beta - alpha > 1)
                score = -negamax(depth - 1, -beta, -alpha);
        }
        
        ply--;
        repetition_his_length = ply;
        RESTORE_BOARD();

        if (score > best_score && ply == 0) {
            best_score = score;
            fall_back_best_move = move;
        }

        // Score was not properly calculated as search stopped.
        if (flags & STOPPED_SEARCH)
            break;

        // Fail hard beta cutoff.
        if (score >= beta) {
            if (!IS_CAPT(move)) {

                // Update killer moves.
                if (killer_moves[ply][1] != 0) {
                    killer_moves[ply][0] = killer_moves[ply][1];
                    killer_moves[ply][1] = move;
                } else 
                    killer_moves[ply][0] = move;
            } else {

                // Update history moves.
                int from = GET_FROM(move);
                int to = GET_TO(move);
                history_moves[board.side][from][to] += depth * depth;
            }

            store_transposition(board.hash_key, depth, beta, LOWERBOUND, move);
            return beta;
        }

        // Pv node.
        if (score > alpha) {
            alpha = score;
            node_type = EXACT;

            // Update PV table.
            pv_table[pv_index] = move;

            if (pv_length[ply + 1] > 0) {
                // Propagate up pv table
                for (int j = 0; j < pv_length[ply+1]; j++)
                    pv_table[pv_index + j + 1] = pv_table[next_pv_index + j];
                pv_length[ply] = pv_length[ply+1] + 1;
            } else
                pv_length[ply] = 1;
        }
    }

    if (moves_searched > 0)
        store_transposition(board.hash_key, depth, alpha, node_type, pv_table[pv_index]);

    return alpha;
}

void search(int depth) {

    /*
    Search algorithm enhancements:
    1. Iterative deepening.
    2. Aspiration windows.
    */

    int alpha = -INF;
    int beta = INF;
    int widening = 75;
    int aspiration_fails_alpha = 0;
    int aspiration_fails_beta = 0;
    flags = 0;
    fall_back_best_move = 0;
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    // Reset killer moves.
    memset(killer_moves, 0, sizeof(killer_moves));

    // Decay history heuristic.
    for (int side = 0; side < 2; side++) {
        for (int from = 0; from < 64; from++) {
            for (int to = 0; to < 64; to++) {
                history_moves[side][from][to] /= 2;
            }
        }
    }

    long long elasped = get_time_ms();
   
    // Step 1. Iterative deepening.
    for (int d = 1; d <= depth || INFINITE;) {
        nodes = 0;
        
        int score = negamax(d, alpha, beta);
        
        if (STOP_SEARCH || time_exceeded())
            break;

        // Aspiration windows.
        if (score <= alpha) {
            aspiration_fails_alpha++;
            alpha -= widening * aspiration_fails_alpha;
            continue;
        }

        if (score >= beta) {
            aspiration_fails_beta++;
            beta += widening * aspiration_fails_beta;
            continue;
        }

        aspiration_fails_alpha = 0;
        aspiration_fails_beta = 0;

        alpha = score - 25;
        beta = score + 25;
        
        printf("info depth %d nodes %d time %lld score cp %d pv ", d, nodes, get_time_ms() - elasped, score);
        for (int i = 0; i < pv_length[0]; i++) {
            print_move(pv_table[i]);
            printf(" ");
        }
        
        printf("\n");
        fflush(stdout);
        
        d++;
        elasped = get_time_ms();
    }

    printf("bestmove ");
    if (pv_table[0] == 0)
        print_move(fall_back_best_move);
    else
        print_move(pv_table[0]);
    printf("\n");
    fflush(stdout);
}