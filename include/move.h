#ifndef MOVE_H
#define MOVE_H

typedef struct MOVE_T {
    int from_to;
    // from sq  to sq
    // 0000000 0000000
    int code;
    /*
    code | Promo | capt | special 1 | sp 2 | type
    0    | 0     | 0    | 0         | 0    | quiet
    1    | 0     | 0    | 0         | 1    | dbp pawn
    2    | 0     | 0    | 1         | 0    | king castle
    3    | 0     | 0    | 1         | 1    | q castle
    4    | 0     | 1    | 0         | 0    | capture
    5    | 0     | 1    | 0         | 1    | ep capture
    8    | 1     | 0    | 0         | 0    | knight promo
    9    | 1     | 0    | 0         | 1    | bishop promo
    10   | 1     | 0    | 1         | 0    | rook promo
    11   | 1     | 0    | 1         | 1    | queen promo
    12   | 1     | 1    | 0         | 0    | knight promo capt
    13   | 1     | 1    | 0         | 1    | bishop promo capt
    14   | 1     | 1    | 1         | 0    | rook promo capt
    15   | 1     | 1    | 1         | 1    | queen promo capt
    */
} MOVES_T;

typedef enum MOVE_TYPES {
    quiet, dbp_pawn, king_castle, queen_castle, capture, ep_capture,
    knight_promo = 8, bishop_promo, rook_promo, queen_promo,
    knight_promo_capture, bishop_promo_capture, rook_promo_capture, queen_promo_capture
} MOVE_TYPES;

void generate_moves(int side);
void make_move(MOVES_T move);
void generate_pawn_moves(int side);



#endif