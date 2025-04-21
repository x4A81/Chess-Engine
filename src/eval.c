#include "../include/eval.h"
#include "../include/board.h"
#include "../include/move.h"

int material_value[12] = {-100, -320, -330, -500, -900, -1000, 100, 320, 330, 500, 900, 1000};

int eval() {
    int score = 0;
    for (int piece = p; piece <= K; piece++)
        score += pop_count(board.bitboards[piece]) * material_value[piece];

    return (board.side == white) ? score : -score;
}