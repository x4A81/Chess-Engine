#include "../include/eval.h"
#include "../include/board.h"
#include "../include/move.h"
#include <stdint.h>

#define MIDDLE_GAME 0
#define END_GAME 1

// Pawn Eval helpers
static inline uint64_t north_fill(uint64_t gen) {
    gen |= (gen << 8);
    gen |= (gen << 16);
    gen |= (gen << 32);
    return gen;
}

static inline uint64_t south_fill(uint64_t gen) {
    gen |= (gen >> 8);
    gen |= (gen >> 16);
    gen |= (gen >> 32);
    return gen;
}

static inline uint64_t file_fill(uint64_t gen) { return north_fill(gen) | south_fill(gen); }

static inline uint64_t wfront_span(uint64_t wpawns) { return north_fill(wpawns) << 8; }
static inline uint64_t brear_span(uint64_t bpawns) { return north_fill(bpawns) << 8; }
static inline uint64_t bfront_span(uint64_t bpawns) { return south_fill(bpawns) >> 8; }
static inline uint64_t wrear_span(uint64_t wpawns) { return south_fill(wpawns) >> 8; }

int evaluate_pawns() {
    int pawn_score = 0;
    uint64_t pawn_bb;

    int pawn_psqt[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
       50, 50, 50, 50, 50, 50, 50, 50,
       10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    /*
    1. Evaluate Material
    2. Piece Square Tables
    3. Doubled & Tripled Pawns
    */

    // Material
    int material = 100;
    pawn_score -= pop_count(board.bitboards[p]) * material;
    pawn_score += pop_count(board.bitboards[P]) * material;
    
    // Piece Square Tables
    pawn_bb = board.bitboards[p];
    while (pawn_bb) {
        int sq = ls1b(pawn_bb);
        POP_BIT(pawn_bb, sq);
        pawn_score -= pawn_psqt[sq];
    }

    pawn_bb = board.bitboards[P];
    while (pawn_bb) {
        int sq = ls1b(pawn_bb);
        POP_BIT(pawn_bb, sq);
        pawn_score += pawn_psqt[sq ^ 56]; // Flip square
    }

    // Double & Triple pawns
    int doubled_penalty = -80, tripled_penalty = -100;

    // Doubled
    uint64_t wpawns_infront_behind = board.bitboards[P] & wrear_span(board.bitboards[P]);
    uint64_t bpawns_infront_behind = board.bitboards[p] & brear_span(board.bitboards[p]);
    int num_doubled = pop_count(wpawns_infront_behind) / 2;
    pawn_score += doubled_penalty;

    num_doubled = pop_count(bpawns_infront_behind) / 2;
    pawn_score -= doubled_penalty;

    // Triples
    wpawns_infront_behind &= board.bitboards[P] & wfront_span(board.bitboards[P]);
    bpawns_infront_behind &= board.bitboards[p] & bfront_span(board.bitboards[p]);
    uint64_t file_with_triple = file_fill(wpawns_infront_behind);

    int num_triples = pop_count(file_with_triple) / 3;
    pawn_score += tripled_penalty;

    file_with_triple = file_fill(bpawns_infront_behind);
    num_triples = pop_count(file_with_triple) / 3;
    pawn_score -= tripled_penalty;

    return pawn_score;
}

int evaluate_knights() {
    int knight_score = 0;
    uint64_t knight_bb;

    int knight_psqt[64] = {
        -50,-20,-30,-30,-30,-30,-20,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  7, 13, 10, 10, 13,  7,-30,
        -30,  2, 12, 17, 17, 12,  2,-30,
        -30,  0, 12, 17, 17, 12,  0,-30,
        -30,  7, 13, 10, 10, 13,  7,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-20,-30,-30,-30,-30,-20,-50,
    };
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    int material = 320;
    knight_score -= pop_count(board.bitboards[n]) * material;
    knight_score += pop_count(board.bitboards[N]) * material;
    
    // Piece Square Tables
    knight_bb = board.bitboards[n];
    while (knight_bb) {
        int sq = ls1b(knight_bb);
        POP_BIT(knight_bb, sq);
        knight_score -= knight_psqt[sq];
    }

    knight_bb = board.bitboards[N];
    while (knight_bb) {
        int sq = ls1b(knight_bb);
        POP_BIT(knight_bb, sq);
        knight_score += knight_psqt[sq ^ 56]; // Flip square
    }

    return knight_score;
}

int evaluate_bishops() {
    int bishop_score = 0;
    uint64_t bishop_bb;

    int bishop_psqt[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 12, 10, 10, 12,  0,-10,
        -10, 10,  7, 12, 12,  7, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    int material = 330;
    bishop_score -= pop_count(board.bitboards[b]) * material;
    bishop_score += pop_count(board.bitboards[B]) * material;
    
    // Piece Square Tables
    bishop_bb = board.bitboards[b];
    while (bishop_bb) {
        int sq = ls1b(bishop_bb);
        POP_BIT(bishop_bb, sq);
        bishop_score -= bishop_psqt[sq];
    }

    bishop_bb = board.bitboards[B];
    while (bishop_bb) {
        int sq = ls1b(bishop_bb);
        POP_BIT(bishop_bb, sq);
        bishop_score += bishop_psqt[sq ^ 56]; // Flip square
    }

    return bishop_score;
}

int evaluate_rooks() {
    int rook_score = 0;
    uint64_t rook_bb;
    
    int rook_psqt[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    int material = 500;
    rook_score -= pop_count(board.bitboards[r]) * material;
    rook_score += pop_count(board.bitboards[R]) * material;
    
    // Piece Square Tables
    rook_bb = board.bitboards[r];
    while (rook_bb) {
        int sq = ls1b(rook_bb);
        POP_BIT(rook_bb, sq);
        rook_score -= rook_psqt[sq];
    }

    rook_bb = board.bitboards[R];
    while (rook_bb) {
        int sq = ls1b(rook_bb);
        POP_BIT(rook_bb, sq);
        rook_score += rook_psqt[sq ^ 56]; // Flip square for white
    }

    return rook_score;
}

int evaluate_queens() {
    int queen_score = 0;
    uint64_t queen_bb;
    
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */
   
    // Material
    int material = 900;
    queen_score -= pop_count(board.bitboards[q]) * material;
    queen_score += pop_count(board.bitboards[Q]) * material;
   
    // Piece Square Tables
    int queen_psqt[64] = {
       -20,-10,-10, -5, -5,-10,-10,-20,
       -10,  0,  0,  0,  0,  0,  0,-10,
       -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
         0,  0,  5,  5,  5,  5,  0, -5,
       -10,  5,  5,  5,  5,  5,  0,-10,
       -10,  0,  5,  0,  0,  0,  0,-10,
       -20,-10,-10, -5, -5,-10,-10,-20
    };

    queen_bb = board.bitboards[q];
    while (queen_bb) {
        int sq = ls1b(queen_bb);
        POP_BIT(queen_bb, sq);
        queen_score -= queen_psqt[sq];
    }

    queen_bb = board.bitboards[Q];
    while (queen_bb) {
        int sq = ls1b(queen_bb);
        POP_BIT(queen_bb, sq);
        queen_score += queen_psqt[sq ^ 56]; // Flip square for white
    }

    return queen_score;
}

int evaluate_kings(int is_endgame) {
    int king_score = 0;
    uint64_t king_bb;

    int king_psqt[2][64] = {
        {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20, -5, -5, -5, -5, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
        },
        {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
        }
    };
    
    /*
    1. Piece Square Tables
    */
    
    // Piece Square Tables
    king_bb = board.bitboards[k];
    while (king_bb) {
        int sq = ls1b(king_bb);
        POP_BIT(king_bb, sq);
        king_score -= king_psqt[is_endgame][sq];
    }

    king_bb = board.bitboards[K];
    while (king_bb) {
        int sq = ls1b(king_bb);
        POP_BIT(king_bb, sq);
        king_score += king_psqt[is_endgame][sq ^ 56]; // Flip square for white
    }

    return king_score;
}


int eval() {

    // + values are good for white, - good for black

    // Defining endgames
    int is_endgame = 0;
    if (pop_count(board.bitboards[Q]) == 0 && pop_count(board.bitboards[q]) == 0 
        && pop_count(board.bitboards[N] | board.bitboards[B] | board.bitboards[n] | board.bitboards[b]) < 2)
        is_endgame = 1;
    else {
        if (pop_count(board.bitboards[Q]) == 1 && pop_count(board.bitboards[N] | board.bitboards[B] | board.bitboards[R]) <= 1)
            is_endgame = 1;
        if (pop_count(board.bitboards[q]) == 1 && pop_count(board.bitboards[n] | board.bitboards[b] | board.bitboards[r]) <= 1)
            is_endgame = 1;
    }

    int score = 0;
    score += evaluate_pawns();
    score += evaluate_knights();
    score += evaluate_bishops();
    score += evaluate_rooks();
    score += evaluate_queens();
    score += evaluate_kings(is_endgame);
    
    return (board.side == white) ? score : -score;
}