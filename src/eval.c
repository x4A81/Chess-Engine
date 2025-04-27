#include "../include/eval.h"
#include "../include/board.h"
#include "../include/move.h"
#include <stdint.h>
#include <stdio.h>

#define MIDDLE_GAME 0
#define END_GAME 1

int material_value[12] = {-100, -320, -330, -500, -900, -1000, 100, 320, 330, 500, 900, 1000};

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

static inline int evaluate_pawns() {
    int pawn_score = 0;
    uint64_t pawn_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    pawn_score += pop_count(board.bitboards[p]) * material_value[p];
    pawn_score += pop_count(board.bitboards[P]) * material_value[P];
    
    // Piece Square Tables
    pawn_bb = board.bitboards[p];
    while (pawn_bb) {
        int sq = ls1b(pawn_bb);
        POP_BIT(pawn_bb, sq);
        pawn_score -= pawn_psqt[sq];
    }

    // printf("%d\n", pawn_score);
    pawn_bb = board.bitboards[P];
    while (pawn_bb) {
        int sq = ls1b(pawn_bb);
        POP_BIT(pawn_bb, sq);
        pawn_score += pawn_psqt[sq ^ 56]; // Flip square
        // printf("sq: %d, score: %d\n", sq, pawn_psqt[sq ^ 56]);
    }

    return pawn_score;
}

static inline int evaluate_knights() {
    int knight_score = 0;
    uint64_t knight_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    knight_score += pop_count(board.bitboards[n]) * material_value[n];
    knight_score += pop_count(board.bitboards[N]) * material_value[N];
    
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

static inline int evaluate_bishops() {
    int bishop_score = 0;
    uint64_t bishop_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    bishop_score += pop_count(board.bitboards[b]) * material_value[b];
    bishop_score += pop_count(board.bitboards[B]) * material_value[B];
    
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

static inline int evaluate_rooks() {
    int rook_score = 0;
    uint64_t rook_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    rook_score += pop_count(board.bitboards[r]) * material_value[r];
    rook_score += pop_count(board.bitboards[R]) * material_value[R];
    
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

static inline int evaluate_queens() {
    int queen_score = 0;
    uint64_t queen_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    queen_score += pop_count(board.bitboards[q]) * material_value[q];
    queen_score += pop_count(board.bitboards[Q]) * material_value[Q];
    
    // Piece Square Tables
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

static inline int evaluate_kings(int is_endgame) {
    int king_score = 0;
    uint64_t king_bb;
    
    /*
    1. Evaluate Material
    2. Piece Square Tables
    */

    // Material
    king_score += pop_count(board.bitboards[k]) * material_value[k];
    king_score += pop_count(board.bitboards[K]) * material_value[K];
    
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