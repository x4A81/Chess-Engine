#include "../include/move.h"
#include "../include/board.h"
#include "../include/rng.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#define GET_FROM(move) (move & 0b111111)
#define GET_TO(move) ((move >> 6) & 0b111111)
#define GET_CODE(move) ((move >> 12) & 0b1111)

typedef enum MOVE_TYPES {
    quiet, dbl_pawn, king_castle, queen_castle, capture, ep_capture,
    knight_promo = 8, bishop_promo, rook_promo, queen_promo,
    knight_promo_capture, bishop_promo_capture, rook_promo_capture, queen_promo_capture
} MOVE_TYPES;

uint64_t pawn_move_table[64][2] = { { 0x0, 0x100 }, { 0x0, 0x200 }, { 0x0, 0x400 }, { 0x0, 0x800 }, { 0x0, 0x1000 }, { 0x0, 0x2000 }, { 0x0, 0x4000 }, { 0x0, 0x8000 }, { 0x1, 0x1010000 }, { 0x2, 0x2020000 }, { 0x4, 0x4040000 }, { 0x8, 0x8080000 }, { 0x10, 0x10100000 }, { 0x20, 0x20200000 }, { 0x40, 0x40400000 }, { 0x80, 0x80800000 }, { 0x100, 0x1000000 }, { 0x200, 0x2000000 }, { 0x400, 0x4000000 }, { 0x800, 0x8000000 }, { 0x1000, 0x10000000 }, { 0x2000, 0x20000000 }, { 0x4000, 0x40000000 }, { 0x8000, 0x80000000 }, { 0x10000, 0x100000000 }, { 0x20000, 0x200000000 }, { 0x40000, 0x400000000 }, { 0x80000, 0x800000000 }, { 0x100000, 0x1000000000 }, { 0x200000, 0x2000000000 }, { 0x400000, 0x4000000000 }, { 0x800000, 0x8000000000 }, { 0x1000000, 0x10000000000 }, { 0x2000000, 0x20000000000 }, { 0x4000000, 0x40000000000 }, { 0x8000000, 0x80000000000 }, { 0x10000000, 0x100000000000 }, { 0x20000000, 0x200000000000 }, { 0x40000000, 0x400000000000 }, { 0x80000000, 0x800000000000 }, { 0x100000000, 0x1000000000000 }, { 0x200000000, 0x2000000000000 }, { 0x400000000, 0x4000000000000 }, { 0x800000000, 0x8000000000000 }, { 0x1000000000, 0x10000000000000 }, { 0x2000000000, 0x20000000000000 }, { 0x4000000000, 0x40000000000000 }, { 0x8000000000, 0x80000000000000 }, { 0x10100000000, 0x100000000000000 }, { 0x20200000000, 0x200000000000000 }, { 0x40400000000, 0x400000000000000 }, { 0x80800000000, 0x800000000000000 }, { 0x101000000000, 0x1000000000000000 }, { 0x202000000000, 0x2000000000000000 }, { 0x404000000000, 0x4000000000000000 }, { 0x808000000000, 0x8000000000000000 }, { 0x1000000000000, 0x0 }, { 0x2000000000000, 0x0 }, { 0x4000000000000, 0x0 }, { 0x8000000000000, 0x0 }, { 0x10000000000000, 0x0 }, { 0x20000000000000, 0x0 }, { 0x40000000000000, 0x0 }, { 0x80000000000000, 0x0 } };
uint64_t pawn_attack_table[64][2] = { { 0x0, 0x200 }, { 0x0, 0x500 }, { 0x0, 0xa00 }, { 0x0, 0x1400 }, { 0x0, 0x2800 }, { 0x0, 0x5000 }, { 0x0, 0xa000 }, { 0x0, 0x4000 }, { 0x2, 0x20000 }, { 0x5, 0x50000 }, { 0xa, 0xa0000 }, { 0x14, 0x140000 }, { 0x28, 0x280000 }, { 0x50, 0x500000 }, { 0xa0, 0xa00000 }, { 0x40, 0x400000 }, { 0x200, 0x2000000 }, { 0x500, 0x5000000 }, { 0xa00, 0xa000000 }, { 0x1400, 0x14000000 }, { 0x2800, 0x28000000 }, { 0x5000, 0x50000000 }, { 0xa000, 0xa0000000 }, { 0x4000, 0x40000000 }, { 0x20000, 0x200000000 }, { 0x50000, 0x500000000 }, { 0xa0000, 0xa00000000 }, { 0x140000, 0x1400000000 }, { 0x280000, 0x2800000000 }, { 0x500000, 0x5000000000 }, { 0xa00000, 0xa000000000 }, { 0x400000, 0x4000000000 }, { 0x2000000, 0x20000000000 }, { 0x5000000, 0x50000000000 }, { 0xa000000, 0xa0000000000 }, { 0x14000000, 0x140000000000 }, { 0x28000000, 0x280000000000 }, { 0x50000000, 0x500000000000 }, { 0xa0000000, 0xa00000000000 }, { 0x40000000, 0x400000000000 }, { 0x200000000, 0x2000000000000 }, { 0x500000000, 0x5000000000000 }, { 0xa00000000, 0xa000000000000 }, { 0x1400000000, 0x14000000000000 }, { 0x2800000000, 0x28000000000000 }, { 0x5000000000, 0x50000000000000 }, { 0xa000000000, 0xa0000000000000 }, { 0x4000000000, 0x40000000000000 }, { 0x20000000000, 0x200000000000000 }, { 0x50000000000, 0x500000000000000 }, { 0xa0000000000, 0xa00000000000000 }, { 0x140000000000, 0x1400000000000000 }, { 0x280000000000, 0x2800000000000000 }, { 0x500000000000, 0x5000000000000000 }, { 0xa00000000000, 0xa000000000000000 }, { 0x400000000000, 0x4000000000000000 }, { 0x2000000000000, 0x0 }, { 0x5000000000000, 0x0 }, { 0xa000000000000, 0x0 }, { 0x14000000000000, 0x0 }, { 0x28000000000000, 0x0 }, { 0x50000000000000, 0x0 }, { 0xa0000000000000, 0x0 }, { 0x40000000000000, 0x0 } };
uint64_t knight_move_table[64] = { 0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000, 0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020, 0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040, 0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000, 0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000, 0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000, 0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000 };
uint64_t king_move_table[64] = { 0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0, 0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000, 0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000, 0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000, 0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000, 0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000 };

void generate_pawn_move_table() {
    uint64_t bb;
    printf("{ ");
    for (int sq = 0; sq < 64; sq++) {

        bb = (1ULL << sq);

        // Black Pawns

        // Single Push
        pawn_move_table[sq][0] = bb >> 8;

        // Captures
        pawn_attack_table[sq][0] = ((pawn_move_table[sq][0] << 1) & 0xfefefefefefefefe) |
                                 ((pawn_move_table[sq][0] >> 1) & 0x7f7f7f7f7f7f7f7f);

        // Dbl
        if (sq <= h7 && sq >= a7)
            pawn_move_table[sq][0] |= bb >> 16;

        // White Pawns

        // Single Push
        pawn_move_table[sq][1] = bb << 8;

        // Captures
        pawn_attack_table[sq][1] |= ((pawn_move_table[sq][1] << 1) & 0xfefefefefefefefe) |
                                  ((pawn_move_table[sq][1] >> 1) & 0x7f7f7f7f7f7f7f7f);

        // Dbl
        if (sq >= a2 && sq <= h2)
            pawn_move_table[sq][1] |= bb << 16;

        printf("{ 0x%" PRIx64 ", 0x%" PRIx64 " }, ", pawn_move_table[sq][0], pawn_move_table[sq][1]);
    }
    
    printf("};\n{ ");
    
    for (int sq = 0; sq < 64; sq++) {
        printf("{ 0x%" PRIx64 ", 0x%" PRIx64 " }, ", pawn_attack_table[sq][0], pawn_attack_table[sq][1]);
    }

    printf("};\n");
}

void generate_knight_move_table() {
    uint64_t l1, l2, r1, r2, h1, h2, bb;
    printf("{ ");
    for (int sq = 0; sq < 64; sq++) {
        bb = (1ULL << sq);
        l1 = (bb >> 1) & 0x7f7f7f7f7f7f7f7f;
        l2 = (bb >> 2) & 0x3f3f3f3f3f3f3f3f;
        r1 = (bb << 1) & 0xfefefefefefefefe;
        r2 = (bb << 2) & 0xfcfcfcfcfcfcfcfc;
        h1 = l1 | r1;
        h2 = l2 | r2;
        knight_move_table[sq] = (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
        printf("0x%" PRIx64 ", ", knight_move_table[sq]);
    }

    printf("};");
}

void generate_king_move_table() {
    uint64_t bb;
    printf("{ ");
    for (int sq = 0; sq < 64; sq++) {
        bb = (1ULL << sq);
        king_move_table[sq] = ((bb >> 1) & 0x7f7f7f7f7f7f7f7f) | ((bb << 1) & 0xfefefefefefefefe);
        bb |= king_move_table[sq];
        king_move_table[sq] |= (bb << 8) | (bb >> 8);
        printf("0x%" PRIx64 ", ", king_move_table[sq]);
    }

    printf("};");
}

uint64_t rook_movement_masks[64];
uint64_t bishop_movement_masks[64];
uint64_t rook_move_table[64][4096];
uint64_t bishop_move_table[64][512];

const int rook_shifts[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

const int bishop_shifts[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

uint64_t rook_magics[64] = { 0x6080104000208000, 0x240400010002000, 0x8080200010008008, 0x4080100080080004, 0x2080080004008003, 0x880040080020001, 0x280020000800100, 0x100005021000082, 0x2000802080004000, 0x80200080400c, 0x801000200080, 0x1002008100100, 0x41801800040080, 0x1000400020900, 0x3000200010004, 0x1000080420100, 0x208000804008, 0x1010040002080, 0x10120040208200, 0x808010000800, 0x4000808004000800, 0x101010004000802, 0x4080808002000100, 0x100020000840041, 0x800802080004008, 0x2000500040002000, 0x1004100200010, 0x10008080100800, 0x1004008080080004, 0x20080800400, 0x100a100040200, 0x8200010044, 0x40800101002040, 0x10006000c00040, 0x100080802004, 0x400900089002100, 0x2000510005000800, 0x800400800200, 0x1800200800100, 0x11c0082000041, 0x4180002000404000, 0x800420081020024, 0x200010008080, 0x2000100100210008, 0x60040008008080, 0x4000201004040, 0x4080201040010, 0x4100820004, 0x10800040002080, 0x40002000804080, 0x8000200080100080, 0x8200810010100, 0x8000080080040080, 0x4008004020080, 0x1000010810028400, 0x140100488200, 0x40144100288001, 0x8000820021004012, 0x4100400c200011, 0x11000420081001, 0x802002144100882, 0x1000204000801, 0x8450002000400c1, 0x10004100802402 };
uint64_t bishop_magics[64] = { 0x2088080100440100, 0x8124802410180, 0x4042082000000, 0x29040100c01800, 0x8004042000000004, 0x2080208200010, 0x100a820088000, 0x1008090011080, 0x200042004841480, 0x8020100101041080, 0x11060206120280, 0x10842402800400, 0x11045040000000, 0x2020008250400000, 0x1100004104104000, 0x2104100400, 0x440010408082126, 0x5081001081100, 0x10000800401822, 0x8000800802004010, 0x45000090400000, 0x4085202020200, 0x450202100409, 0x400022080400, 0x4200010021008, 0x2080010300080, 0x10900018004010, 0x8004040080410200, 0x9010000104000, 0x100410022010100, 0x2104004000880400, 0x908009042080, 0x10442010110298, 0x1280298881040, 0x140200040800, 0x400808108200, 0x8040024010030100, 0x8500020010083, 0x84040090005800, 0x404010040002402, 0x2025182010000420, 0x4014014950204800, 0x941088001000, 0x8000a02011002800, 0x1000080100400400, 0x210200a5008200, 0x10014104010100, 0x1014200802a00, 0x1080804840800, 0x220802080200, 0x10020201440800, 0x814084040400, 0x1202020004, 0x800088208020400, 0x2008101002005002, 0x4280200620000, 0x2010402510400, 0x8004202012000, 0x400000028841001, 0x200008000840428, 0x1001040050100, 0x504080201, 0x1020045110110109, 0x1020208102102040, };

uint64_t mask_rook_movement(int sq) {
    uint64_t mask = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 6; r++) { mask |= (1ULL << (fl + r * 8)); }
    for (r = rk - 1; r >= 1; r--) { mask |= (1ULL << (fl + r * 8)); }
    for (f = fl + 1; f <= 6; f++) { mask |= (1ULL << (f + rk * 8)); }
    for (f = fl - 1; f >= 1; f--) { mask |= (1ULL << (f + rk * 8)); }
    return mask;
}

uint64_t mask_bishop_movement(int sq) {
    uint64_t mask = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) { mask |= (1ULL << (f + r * 8)); }
    for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) { mask |= (1ULL << (f + r * 8)); }
    for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) { mask |= (1ULL << (f + r * 8)); }
    for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) { mask |= (1ULL << (f + r * 8)); }
    return mask;
}

uint64_t mask_blocked_rook_movement(int sq, uint64_t blockers) {
    uint64_t mask = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 7; r++) {
        mask |= (1ULL << (fl + r * 8));
        if (blockers & (1ULL << (fl + r * 8)))
            break;
    }

    for (r = rk - 1; r >= 0; r--) {
        mask |= (1ULL << (fl + r * 8));
        if (blockers & (1ULL << (fl + r * 8)))
            break;
    }

    for (f = fl + 1; f <= 7; f++) {
        mask |= (1ULL << (f + rk * 8));
        if (blockers & (1ULL << (f + rk * 8)))
            break;
    }

    for (f = fl - 1; f >= 0; f--) {
        mask |= (1ULL << (f + rk * 8));
        if (blockers & (1ULL << (f + rk * 8)))
            break;
    }

    return mask;
}

uint64_t mask_blocked_bishop_movement(int sq, uint64_t blockers) {
    uint64_t mask = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
        mask |= (1ULL << (f + r * 8));
        if (blockers & (1ULL << (f + r * 8)))
            break;
    }

    for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
        mask |= (1ULL << (f + r*8));
        if (blockers & (1ULL << (f + r * 8)))
            break;
    }

    for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
        mask |= (1ULL << (f + r*8));
        if (blockers & (1ULL << (f + r * 8)))
            break;
    }

    for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
        mask |= (1ULL << (f + r*8));
        if (blockers & (1ULL << (f + r * 8)))
            break;
    }

    return mask;
}

uint64_t generate_variation_mask(int idx, int pop_c, uint64_t mask) {
    uint64_t bb = 0ULL;
    for (int variation = 0; variation < pop_c; variation++) {
        int sq = ls1b(mask);
        POP_BIT(mask, sq);
        if (idx & (1 << variation)) 
            bb |= (1ULL << sq);
    }

    return bb;
}

uint64_t find_magics(int sq, int bits, int bishop_f) {
    uint64_t variations[4096], attacks[4096], used_attacks[4096];
    uint64_t attack_mask = bishop_f ? mask_bishop_movement(sq) : mask_rook_movement(sq);
    int variations_count = 1 << bits;
    for (int idx = 0; idx < variations_count; idx++) {
        variations[idx] = generate_variation_mask(idx, bits, attack_mask);
        attacks[idx] = bishop_f ? mask_blocked_bishop_movement(sq, variations[idx]) :
        mask_blocked_rook_movement(sq, variations[idx]);
    }

    for (int k = 0; k < 100000000; k++) {
        uint64_t magic = rng_magic();
        if (pop_count((attack_mask * magic) & 0xFF00000000000000ULL) < 6)
            continue;
        memset(used_attacks, 0ULL, sizeof(used_attacks));
        int idx, fail;
        for (idx = 0, fail = 0; !fail && idx < variations_count; idx++) {
            int magic_idx = (int)((variations[idx] * magic) >> (64 - bits));
            if (used_attacks[magic_idx] == 0ULL) { used_attacks[magic_idx] = attacks[idx]; }
            else if (used_attacks[magic_idx] != attacks[idx]) { fail = 1; }
        }

        if (!fail)
            return magic;
    }

    printf("\n Magic Number Failed! \n");
    return 0ULL;
}

void generate_magics() {
    int sq;
    printf("{ ");
    for (sq = 0; sq < 64; sq++) {
        rook_magics[sq] = find_magics(sq, rook_shifts[sq], 0);
        bishop_magics[sq] = find_magics(sq, bishop_shifts[sq], 1);
        printf("0x%" PRIx64 ", ", rook_magics[sq]);
    }

    printf("};\n\n{ ");
    for (sq = 0; sq < 64; sq++) {
        printf("0x%" PRIx64 ", ", bishop_magics[sq]);
    }

    printf("};");
}

void initialise_sliding_move_tables(int bishop_f) {
    for (int sq = 0; sq < 64; sq++) {
        bishop_movement_masks[sq] = mask_bishop_movement(sq);
        rook_movement_masks[sq] = mask_rook_movement(sq);
        uint64_t attack_mask = bishop_f ? bishop_movement_masks[sq] : rook_movement_masks[sq];
        int bits = pop_count(attack_mask);
        int variations_count = 1 << bits;
        for (int idx = 0; idx < variations_count; idx++) {
            if (bishop_f) {
                uint64_t variation = generate_variation_mask(idx, bits, attack_mask);
                int magic_idx = (variation * bishop_magics[sq]) >> (64 - bishop_shifts[sq]);
                bishop_move_table[sq][magic_idx] = mask_blocked_bishop_movement(sq, variation);
            } else {
                uint64_t variation = generate_variation_mask(idx, bits, attack_mask);
                int magic_idx = (variation * rook_magics[sq]) >> (64 - rook_shifts[sq]);
                rook_move_table[sq][magic_idx] = mask_blocked_rook_movement(sq, variation);
            }
        }
    }
}

uint64_t rook_moves(int sq, uint64_t bb) {
    bb &= rook_movement_masks[sq];
    bb *= rook_magics[sq];
    bb >>= 64 - rook_shifts[sq];
    return rook_move_table[sq][bb];
}

uint64_t bishop_moves(int sq, uint64_t bb) {
    bb &= bishop_movement_masks[sq];
    bb *= bishop_magics[sq];
    bb >>= 64 - bishop_shifts[sq];
    return bishop_move_table[sq][bb];
}

uint64_t queen_moves(int sq, uint64_t bb) {
    return rook_moves(sq, bb) | bishop_moves(sq, bb);
}

uint64_t get_psuedo_attackers(int sq, int side) {
    uint64_t bb, attack_bb;

    // Attacks by pawns
    bb = (side == white) ? pawn_attack_table[sq][black] : pawn_attack_table[sq][white];
    attack_bb = bb & ((side == white) ? board.bitboards[P] : board.bitboards[p]);

    // Attacks by knights
    bb = knight_move_table[sq];
    attack_bb |= bb & ((side == white) ? board.bitboards[N] : board.bitboards[n]);

    // Attacks by bishops & diagonal attacks by queen
    bb = bishop_moves(sq, board.bitboards[14]);
    attack_bb |= bb & ((side == white) ? board.bitboards[B] | board.bitboards[Q] : board.bitboards[b] | board.bitboards[q]);
    
    // Attacks by rook & straight attacks by queen
    bb = rook_moves(sq, board.bitboards[14]);
    attack_bb |= bb & ((side == white) ? board.bitboards[R] | board.bitboards[Q] : board.bitboards[r] | board.bitboards[q]);
    
    // Attacks by opposing king
    bb = (side == white) ? king_move_table[sq] : king_move_table[sq];
    attack_bb |= bb & ((side == white) ? board.bitboards[K] : board.bitboards[k]);

    return attack_bb; // If the attack board is empty then the square is not attacked
}

void generate_moves(MOVE_LIST_T *move_list) {
    move_list->count = 0;

    /*
    1. Detect legal king moves.
    2. Check evasions and double check.
    3. Pinned pieces
    4. Other moves 
    NB. Enpassant discovered check
    */

    int king_sq = -1, from_sq = -1, to_sq = -1, i = 0;
    uint64_t attack_bb = UINT64_C(0), piece_bb = UINT64_C(0),
    capture_mask = UINT64_C(0xFFFFFFFFFFFFFFFF),
    push_mask = UINT64_C(0xFFFFFFFFFFFFFFFF);
    
    // Step 1. Detect legal king moves.

    if (board.side == white)
        king_sq = ls1b(board.bitboards[K]);
    else 
        king_sq = ls1b(board.bitboards[k]);
    
    // Set the attackbb to the king moves that don't capture friendly pieces.
    attack_bb = king_move_table[king_sq] & ~board.bitboards[12+board.side];

    // Pop bit from all bitboard, deals with x-ray attacks.
    POP_BIT(board.bitboards[14], king_sq); 
    while (attack_bb) {
        to_sq = ls1b(attack_bb);
        POP_BIT(attack_bb, to_sq);
        if (get_psuedo_attackers(to_sq, !board.side))
            continue;
        
        // add king moves
        int code = quiet;

        // Move is a capture
        if ((1ULL << to_sq) & board.bitboards[13-board.side])
            code = capture;

        add_move(move_list, king_sq, to_sq, code);
    }

    SET_BIT(board.bitboards[14], king_sq);

    // Step 2. Check evasions and double check.

    uint64_t checkers = get_psuedo_attackers(king_sq, !board.side);

    // If in double check then only king moves are legal
    if (pop_count(checkers) > 1) return;

    /*
    If single check:
    1. Move king (already done)
    2. Capture the attacking pieces
    3. Block (if checker is sliding piece)
    */

    int slider;
    uint64_t attacking_bishops = 0ULL, attacking_rooks = 0ULL, attacking_queens = 0ULL;
    uint64_t king_rook_rays = rook_moves(king_sq, board.bitboards[14]),
        king_bishop_rays = bishop_moves(king_sq, board.bitboards[14]);

    if (board.side == white) {
        attacking_rooks = board.bitboards[r];
        attacking_bishops = board.bitboards[b];
        attacking_queens = board.bitboards[q];
    } else {
        attacking_rooks = board.bitboards[R];
        attacking_bishops = board.bitboards[B];
        attacking_queens = board.bitboards[Q];
    }

    if (pop_count(checkers) == 1) { 
        // 2. Capture checkers
        capture_mask = checkers;

        // 3. Block ray attacks

        push_mask = 0ULL;

        // Calculate bishop, rook and queen rays that attack the king. (push mask)
        attack_bb = attacking_bishops & checkers;
        while (attack_bb) {
            slider = ls1b(attack_bb);
            push_mask |= bishop_moves(slider, board.bitboards[14]) & king_bishop_rays;
            POP_BIT(attack_bb, slider);
        }

        attack_bb = attacking_rooks & checkers;
        while (attack_bb) {
            slider = ls1b(attack_bb);
            push_mask |= rook_moves(slider, board.bitboards[14]) & king_rook_rays;
            POP_BIT(attack_bb, slider);
        }

        attack_bb = attacking_queens & checkers;
        while (attack_bb) {
            slider = ls1b(attack_bb);
            if (rook_moves(slider, board.bitboards[14]) & (1ULL << king_sq))
                push_mask |= rook_moves(slider, board.bitboards[14]) & king_rook_rays;
            else
                push_mask |= bishop_moves(slider, board.bitboards[14]) & king_bishop_rays;
            POP_BIT(attack_bb, slider);
        }
    }

    // Step 3. Pinned Pieces.

    /*
    1. Calculate moves from opponent sliding pieces.
    2. Sliding piece moves from king union.
    3. Union of 1. and 2..
    
    If 3. union friendly pieces then it is absolutely pinned.
    */

    uint64_t pinned_pieces = UINT64_C(0);
    uint64_t pinned_movement[8] = {UINT64_C(0)};
    int rays_to_king = 0;

    // Calculate rays from the king
    king_rook_rays = rook_moves(king_sq, board.bitboards[13 - board.side]);
    king_bishop_rays = bishop_moves(king_sq, board.bitboards[13 - board.side]);

    attack_bb = attacking_bishops & king_bishop_rays;
    while (attack_bb) {
        int slider = ls1b(attack_bb);
        uint64_t attacking_ray = bishop_moves(slider, (1ULL << king_sq)) & king_bishop_rays;
        if (pop_count(board.bitboards[13 - board.side] & attacking_ray) == 0) {
            if (pop_count(board.bitboards[12 + board.side] & attacking_ray) == 1) {
                pinned_pieces |= board.bitboards[12 + board.side] & attacking_ray;
                pinned_movement[rays_to_king++] = attacking_ray | (1ULL << slider);
            }
        }

        POP_BIT(attack_bb, slider);
    }

    attack_bb = attacking_rooks & king_rook_rays;
    while (attack_bb) {
        int slider = ls1b(attack_bb);
        uint64_t attacking_ray = rook_moves(slider, (1ULL << king_sq)) & king_rook_rays;
        if (pop_count(board.bitboards[13 - board.side] & attacking_ray) == 0) {
            if (pop_count(board.bitboards[12 + board.side] & attacking_ray) == 1) {
                pinned_pieces |= board.bitboards[12 + board.side] & attacking_ray;
                pinned_movement[rays_to_king++] = attacking_ray | (1ULL << slider);
            }
        }

        POP_BIT(attack_bb, slider);
    }

    attack_bb = attacking_queens & (king_bishop_rays | king_rook_rays);
    while (attack_bb) {
        int slider = ls1b(attack_bb);
        if (rook_moves(slider, (board.bitboards[13 - board.side] | (1ULL << king_sq))) & (1ULL << king_sq)) {
            uint64_t attacking_ray = rook_moves(slider, (1ULL << king_sq)) & king_rook_rays;
            if (pop_count(board.bitboards[13 - board.side] & attacking_ray) == 0) {
                if (pop_count(board.bitboards[12 + board.side] & attacking_ray) == 1) {
                    pinned_pieces |= board.bitboards[12 + board.side] & attacking_ray;
                    pinned_movement[rays_to_king++] = attacking_ray | (1ULL << slider);
                }
            }
        } else {
            uint64_t attacking_ray = bishop_moves(slider, (1ULL << king_sq)) & king_bishop_rays;
        if (pop_count(board.bitboards[13 - board.side] & attacking_ray) == 0) {
            if (pop_count(board.bitboards[12 + board.side] & attacking_ray) == 1) {
                pinned_pieces |= board.bitboards[12 + board.side] & attacking_ray;
                pinned_movement[rays_to_king++] = attacking_ray | (1ULL << slider);
            }
        }
        }

        POP_BIT(attack_bb, slider);
    }

    // Step 4. Other Moves
    /*
    Things to consider:
    1. Check if can castle:
        - If in check you can't castle
        - If the squares that involve castling aren't free or are attacked
        - If the rook has been captured (handled in the make_move function)
    2. Enpassant discovered check
    */

    // Castling

    if (pop_count(checkers) == 0) {
        if (board.side == white) {
            if (board.castling & wkingside) {
                if (!(board.bitboards[14] & ((1ULL << f1) | (1ULL << g1)))) {
                    if (!(get_psuedo_attackers(f1, !board.side) || get_psuedo_attackers(g1, !board.side)))
                        add_move(move_list, e1, g1, king_castle);
                }
            }
            
            if (board.castling & wqueenside) {
                if (!(board.bitboards[14] & ((1ULL << d1) | (1ULL << c1) | (1ULL << b1)))) {
                    if (!(get_psuedo_attackers(d1, !board.side) || get_psuedo_attackers(c1, !board.side)))
                        add_move(move_list, e1, c1, queen_castle);
                }
            }
        } else {
            if (board.castling & bkingside) {
                if (!(board.bitboards[14] & ((1ULL << f8) | (1ULL << g8)))) {
                    if (!(get_psuedo_attackers(f8, !board.side) || get_psuedo_attackers(g8, !board.side)))
                        add_move(move_list, e8, g8, king_castle);
                }
            }
            
            if (board.castling & bqueenside) {
                if (!(board.bitboards[14] & ((1ULL << d8) | (1ULL << c8) | (1ULL << b8)))) {
                    if (!(get_psuedo_attackers(d8, !board.side) || get_psuedo_attackers(c8, !board.side)))
                        add_move(move_list, e8, c8, queen_castle);
                }
            }
        }
    }
    
    // Pawn moves
    if (board.side == white)
        piece_bb = board.bitboards[P];
    else
        piece_bb = board.bitboards[p];

    while (piece_bb) {
        from_sq = ls1b(piece_bb);
        POP_BIT(piece_bb, from_sq);

        // Deal with pawn (dbl) pushes and promotions
        attack_bb = pawn_move_table[from_sq][board.side] & push_mask & ~board.bitboards[14];
        // Pinned piece movement
        if ((1ULL << from_sq) & pinned_pieces) {
            for (i = 0; i < rays_to_king; i++) {
                if ((1ULL << from_sq) & pinned_movement[i]) {
                    attack_bb &= pinned_movement[i];
                    break;
                }
            }
        }

        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if (to_sq >= a8 || to_sq <= h1) {
                add_move(move_list, from_sq, to_sq, knight_promo);
                add_move(move_list, from_sq, to_sq, bishop_promo);
                add_move(move_list, from_sq, to_sq, queen_promo);
                add_move(move_list, from_sq, to_sq, rook_promo);
            } else if (to_sq-from_sq == 16 || to_sq-from_sq == -16) {
                if (board.side == white) {
                    if (!((1ULL << from_sq << 8) & board.bitboards[14]))
                        add_move(move_list, from_sq, to_sq, dbl_pawn);
                } else {
                    if (!((1ULL << from_sq >> 8) & board.bitboards[14]))
                        add_move(move_list, from_sq, to_sq, dbl_pawn);
                }
            } else {
                add_move(move_list, from_sq, to_sq, quiet);
            }
        }

        // Deal with pawn attacks and promotions
        
        // Enpassant and enpassant discovered check
        if (board.enpassant != -1 && (pawn_attack_table[from_sq][board.side] & (1ULL << board.enpassant))) {
            // Enpassant discovered check
            POP_BIT(board.bitboards[14], from_sq);

            attack_bb = pawn_attack_table[from_sq][board.side] & (board.bitboards[13-board.side] | (1ULL << board.enpassant));
            
            if (board.side == white) {
                // If in check, then discovered check doesn't matter
                if (pop_count(checkers) == 0) {
                    // Enpassant discovered check
                    POP_BIT(board.bitboards[14], board.enpassant - 8);
                    SET_BIT(board.bitboards[14], board.enpassant);
                    if (get_psuedo_attackers(king_sq, !board.side))
                        attack_bb = pawn_attack_table[from_sq][board.side] & capture_mask & board.bitboards[13-board.side];
                    SET_BIT(board.bitboards[14], board.enpassant - 8);
                    POP_BIT(board.bitboards[14], board.enpassant);
                } else {
                    // If enpassant captures or blocks a checker
                    if ((1ULL << (board.enpassant - 8)) & capture_mask || (1ULL << board.enpassant) & push_mask)
                        attack_bb = (1ULL << board.enpassant);
                    else
                        attack_bb = pawn_attack_table[from_sq][board.side] & capture_mask & board.bitboards[13-board.side];
                }
            } else {
                if (pop_count(checkers) == 0) {
                    // Enpassant discovered check
                    POP_BIT(board.bitboards[14], board.enpassant + 8);
                    SET_BIT(board.bitboards[14], board.enpassant);
                    if (get_psuedo_attackers(king_sq, !board.side))
                        attack_bb = pawn_attack_table[from_sq][board.side] & capture_mask & board.bitboards[13-board.side];
                    SET_BIT(board.bitboards[14], board.enpassant + 8);
                    POP_BIT(board.bitboards[14], board.enpassant);
                } else {
                    // If enpassant captures or blocks a checker
                    if ((1ULL << (board.enpassant + 8)) & capture_mask || (1ULL << board.enpassant) & push_mask)
                        attack_bb = (1ULL << board.enpassant);
                    else
                        attack_bb = pawn_attack_table[from_sq][board.side] & capture_mask & board.bitboards[13-board.side];
                }
            }

            SET_BIT(board.bitboards[14], from_sq);
        } else
            attack_bb = pawn_attack_table[from_sq][board.side] & capture_mask & board.bitboards[13-board.side];
        
        // Pinned piece movement
        if ((1ULL << from_sq) & pinned_pieces)
            attack_bb &= pinned_movement[i];
        
        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if (to_sq >= a8 || to_sq <= h1) {
                add_move(move_list, from_sq, to_sq, knight_promo_capture);
                add_move(move_list, from_sq, to_sq, bishop_promo_capture);
                add_move(move_list, from_sq, to_sq, rook_promo_capture);
                add_move(move_list, from_sq, to_sq, queen_promo_capture);
            } else if (to_sq == board.enpassant) {
                if ((from_sq >= a5 && from_sq <= h5) || (from_sq >= a4 && from_sq <= h4))
                    add_move(move_list, from_sq, to_sq, ep_capture);
            } else
                add_move(move_list, from_sq, to_sq, capture);
        }
    }

    // Knight moves

    if (board.side == white)
        piece_bb = board.bitboards[N];
    else 
        piece_bb = board.bitboards[n];

    while (piece_bb) {
        from_sq = ls1b(piece_bb);
        POP_BIT(piece_bb, from_sq);
        attack_bb = knight_move_table[from_sq] & (push_mask | capture_mask) & ~board.bitboards[12+board.side];
        if ((1ULL << from_sq) & pinned_pieces)
            continue;
        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if ((1ULL << to_sq) & board.bitboards[13-board.side])
                add_move(move_list, from_sq, to_sq, capture);
            else
                add_move(move_list, from_sq, to_sq, quiet);
        }
    }

    // Bishop moves

    if (board.side == white)
        piece_bb = board.bitboards[B];
    else 
        piece_bb = board.bitboards[b];

    while (piece_bb) {
        from_sq = ls1b(piece_bb);
        POP_BIT(piece_bb, from_sq);
        attack_bb = bishop_moves(from_sq, board.bitboards[14]) & (push_mask | capture_mask) & ~board.bitboards[12+board.side];
        if ((1ULL << from_sq) & pinned_pieces) {
            for (i = 0; i < rays_to_king; i++) {
                if ((1ULL << from_sq) & pinned_movement[i]) {
                    attack_bb &= pinned_movement[i];
                    break;
                }
            }
        }

        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if ((1ULL << to_sq) & board.bitboards[13-board.side])
                add_move(move_list, from_sq, to_sq, capture);
            else
                add_move(move_list, from_sq, to_sq, quiet);
        }
    }

    // Rook moves

    if (board.side == white)
        piece_bb = board.bitboards[R];
    else 
        piece_bb = board.bitboards[r];

    while (piece_bb) {
        from_sq = ls1b(piece_bb);
        POP_BIT(piece_bb, from_sq);
        attack_bb = rook_moves(from_sq, board.bitboards[14]) & (push_mask | capture_mask) & ~board.bitboards[12+board.side];
        if ((1ULL << from_sq) & pinned_pieces) {
            for (i = 0; i < rays_to_king; i++) {
                if ((1ULL << from_sq) & pinned_movement[i]) {
                    attack_bb &= pinned_movement[i];
                    break;
                }
            }
        }
        
        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if ((1ULL << to_sq) & board.bitboards[13-board.side])
                add_move(move_list, from_sq, to_sq, capture);
            else
                add_move(move_list, from_sq, to_sq, quiet);
        }
    }

    // Queen moves

    if (board.side == white)
        piece_bb = board.bitboards[Q];
    else 
        piece_bb = board.bitboards[q];

    while (piece_bb) {
        from_sq = ls1b(piece_bb);
        POP_BIT(piece_bb, from_sq);
        attack_bb = queen_moves(from_sq, board.bitboards[14]) & (push_mask | capture_mask) & ~board.bitboards[12+board.side];
        if ((1ULL << from_sq) & pinned_pieces) {
            for (i = 0; i < rays_to_king; i++) {
                if ((1ULL << from_sq) & pinned_movement[i]) {
                    attack_bb &= pinned_movement[i];
                    break;
                }
            }
        }
        
        while (attack_bb) {
            to_sq = ls1b(attack_bb);
            POP_BIT(attack_bb, to_sq);
            if ((1ULL << to_sq) & board.bitboards[13-board.side])
                add_move(move_list, from_sq, to_sq, capture);
            else
                add_move(move_list, from_sq, to_sq, quiet);
        }
    }
}

void make_move(uint16_t move) {
    /*
    1. Move the piece to the new square.
    2. Handle capt
    3. Handle promo
    4. Handle castle
    5. Update castling rights
    6. Update enpassant
    7. Update everything else
    */

    // Step 1.

    // Find piece bitboard
    int piece = -1;
    for (int bb = p; bb <= K; bb++) {
        if ((1ULL << GET_FROM(move)) & board.bitboards[bb]) {
            piece = bb;
            break;
        }
    }

    // Move the piece
    POP_BIT(board.bitboards[piece], GET_FROM(move));
    SET_BIT(board.bitboards[piece], GET_TO(move));

    // Step 2.
    
    if (GET_CODE(move) == capture || GET_CODE(move) >= knight_promo_capture) {
        for (int bb = p; bb <= K; bb++) {
            if (bb == piece) continue;
            if ((1ULL << GET_TO(move)) & board.bitboards[bb]) {
                POP_BIT(board.bitboards[bb], GET_TO(move));
                break;
            }
        }
    }


    if (GET_CODE(move) == ep_capture) {
        if (board.side == white)
            POP_BIT(board.bitboards[p], board.enpassant - 8);
        else
            POP_BIT(board.bitboards[P], board.enpassant + 8);
    }

    // Step 3.

    if (GET_CODE(move) == knight_promo || GET_CODE(move) == knight_promo_capture) {
        if (board.side == white) {
            POP_BIT(board.bitboards[P], GET_TO(move));
            SET_BIT(board.bitboards[N], GET_TO(move));
        } else {
            POP_BIT(board.bitboards[p], GET_TO(move));
            SET_BIT(board.bitboards[n], GET_TO(move));
        }
    }

    if (GET_CODE(move) == bishop_promo || GET_CODE(move) == bishop_promo_capture) {
        if (board.side == white) {
            POP_BIT(board.bitboards[P], GET_TO(move));
            SET_BIT(board.bitboards[B], GET_TO(move));
        } else {
            POP_BIT(board.bitboards[p], GET_TO(move));
            SET_BIT(board.bitboards[b], GET_TO(move));
        }
    }

    if (GET_CODE(move) == rook_promo || GET_CODE(move) == rook_promo_capture) {
        if (board.side == white) {
            POP_BIT(board.bitboards[P], GET_TO(move));
            SET_BIT(board.bitboards[R], GET_TO(move));
        } else {
            POP_BIT(board.bitboards[p], GET_TO(move));
            SET_BIT(board.bitboards[r], GET_TO(move));
        }
    }

    if (GET_CODE(move) == queen_promo || GET_CODE(move) == queen_promo_capture) {
        if (board.side == white) {
            POP_BIT(board.bitboards[P], GET_TO(move));
            SET_BIT(board.bitboards[Q], GET_TO(move));
        } else {
            POP_BIT(board.bitboards[p], GET_TO(move));
            SET_BIT(board.bitboards[q], GET_TO(move));
        }
    }

    // Step 4.

    if (GET_CODE(move) == king_castle) {
        if (board.side == white) {
            POP_BIT(board.bitboards[R], h1);
            SET_BIT(board.bitboards[R], f1);
        } else {
            POP_BIT(board.bitboards[r], h8);
            SET_BIT(board.bitboards[r], f8);
        }
    }

    if (GET_CODE(move) == queen_castle) {
        if (board.side == white) {
            POP_BIT(board.bitboards[R], a1);
            SET_BIT(board.bitboards[R], d1);
        } else {
            POP_BIT(board.bitboards[r], a8);
            SET_BIT(board.bitboards[r], d8);
        }
    }

    // Step 5.

    int castling_encoder[64] = {
        13, 15, 15, 15, 12, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
         7, 15, 15, 15,  3, 15, 15, 11
    };

    board.castling &= castling_encoder[GET_FROM(move)];
    board.castling &= castling_encoder[GET_TO(move)];

    // Step 6.
    if (GET_CODE(move) == dbl_pawn) {
        if (board.side == white)
            board.enpassant = GET_TO(move) - 8;
        else
            board.enpassant = GET_TO(move) + 8;
    } else {
        board.enpassant = -1;
    }


    // Step 7.
    board.side ^= 1;
    board.fullmove++;

    if (GET_CODE(move) == capture || piece == p || piece == P)
        board.halfmove = 0;
    else
        board.halfmove++;

    board.bitboards[12] = board.bitboards[p] | board.bitboards[n] | board.bitboards[b] | board.bitboards[r] | board.bitboards[q] | board.bitboards[k];
    board.bitboards[13] = board.bitboards[P] | board.bitboards[N] | board.bitboards[B] | board.bitboards[R] | board.bitboards[Q] | board.bitboards[K];
    board.bitboards[14] = board.bitboards[12] | board.bitboards[13];
}

void print_move(uint16_t move) {
    printf("%s", (char[]){'a' + (GET_FROM(move) % 8), '1' + (GET_FROM(move) / 8),
         'a' + (GET_TO(move) % 8), '1' + (GET_TO(move) / 8)});
    if (GET_CODE(move) >= knight_promo) {
        switch (GET_CODE(move)) {
            case knight_promo_capture:
            case knight_promo: printf("%s", "n"); break;
            case bishop_promo_capture:
            case bishop_promo: printf("%s", "b"); break;
            case rook_promo_capture:
            case rook_promo: printf("%s", "r"); break;
            case queen_promo_capture:
            case queen_promo: printf("%s", "q"); break;
        }
    }
}

void add_move(MOVE_LIST_T *move_list, int from, int to, int code) {
    uint16_t move = (from | (to << 6) | (code << 12));
    move_list->moves[move_list->count++] = move;
}