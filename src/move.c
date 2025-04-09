#include "../include/move.h"
#include "../include/board.h"
#include "../include/rng.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define GET_FROM(move) ((move) & 0b111111)
#define GET_TO(move) (((move) >> 6) & 0b111111)

typedef enum MOVE_TYPES {
    quiet, dbp_pawn, king_castle, queen_castle, capture, ep_capture,
    knight_promo = 8, bishop_promo, rook_promo, queen_promo,
    knight_promo_capture, bishop_promo_capture, rook_promo_capture, queen_promo_capture
} MOVE_TYPES;

uint64_t pawn_move_table[64][2] = { { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x1, 0x1010000 }, { 0x2, 0x2020000 }, { 0x4, 0x4040000 }, { 0x8, 0x8080000 }, { 0x10, 0x10100000 }, { 0x20, 0x20200000 }, { 0x40, 0x40400000 }, { 0x80, 0x80800000 }, { 0x100, 0x1000000 }, { 0x200, 0x2000000 }, { 0x400, 0x4000000 }, { 0x800, 0x8000000 }, { 0x1000, 0x10000000 }, { 0x2000, 0x20000000 }, { 0x4000, 0x40000000 }, { 0x8000, 0x80000000 }, { 0x10000, 0x100000000 }, { 0x20000, 0x200000000 }, { 0x40000, 0x400000000 }, { 0x80000, 0x800000000 }, { 0x100000, 0x1000000000 }, { 0x200000, 0x2000000000 }, { 0x400000, 0x4000000000 }, { 0x800000, 0x8000000000 }, { 0x1000000, 0x10000000000 }, { 0x2000000, 0x20000000000 }, { 0x4000000, 0x40000000000 }, { 0x8000000, 0x80000000000 }, { 0x10000000, 0x100000000000 }, { 0x20000000, 0x200000000000 }, { 0x40000000, 0x400000000000 }, { 0x80000000, 0x800000000000 }, { 0x100000000, 0x1000000000000 }, { 0x200000000, 0x2000000000000 }, { 0x400000000, 0x4000000000000 }, { 0x800000000, 0x8000000000000 }, { 0x1000000000, 0x10000000000000 }, { 0x2000000000, 0x20000000000000 }, { 0x4000000000, 0x40000000000000 }, { 0x8000000000, 0x80000000000000 }, { 0x10100000000, 0x100000000000000 }, { 0x20200000000, 0x200000000000000 }, { 0x40400000000, 0x400000000000000 }, { 0x80800000000, 0x800000000000000 }, { 0x101000000000, 0x1000000000000000 }, { 0x202000000000, 0x2000000000000000 }, { 0x404000000000, 0x4000000000000000 }, { 0x808000000000, 0x8000000000000000 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, };
uint64_t pawn_attack_table[64][2] = { { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x2, 0x20000 }, { 0x5, 0x50000 }, { 0xa, 0xa0000 }, { 0x14, 0x140000 }, { 0x28, 0x280000 }, { 0x50, 0x500000 }, { 0xa0, 0xa00000 }, { 0x40, 0x400000 }, { 0x200, 0x2000000 }, { 0x500, 0x5000000 }, { 0xa00, 0xa000000 }, { 0x1400, 0x14000000 }, { 0x2800, 0x28000000 }, { 0x5000, 0x50000000 }, { 0xa000, 0xa0000000 }, { 0x4000, 0x40000000 }, { 0x20000, 0x200000000 }, { 0x50000, 0x500000000 }, { 0xa0000, 0xa00000000 }, { 0x140000, 0x1400000000 }, { 0x280000, 0x2800000000 }, { 0x500000, 0x5000000000 }, { 0xa00000, 0xa000000000 }, { 0x400000, 0x4000000000 }, { 0x2000000, 0x20000000000 }, { 0x5000000, 0x50000000000 }, { 0xa000000, 0xa0000000000 }, { 0x14000000, 0x140000000000 }, { 0x28000000, 0x280000000000 }, { 0x50000000, 0x500000000000 }, { 0xa0000000, 0xa00000000000 }, { 0x40000000, 0x400000000000 }, { 0x200000000, 0x2000000000000 }, { 0x500000000, 0x5000000000000 }, { 0xa00000000, 0xa000000000000 }, { 0x1400000000, 0x14000000000000 }, { 0x2800000000, 0x28000000000000 }, { 0x5000000000, 0x50000000000000 }, { 0xa000000000, 0xa0000000000000 }, { 0x4000000000, 0x40000000000000 }, { 0x20000000000, 0x200000000000000 }, { 0x50000000000, 0x500000000000000 }, { 0xa0000000000, 0xa00000000000000 }, { 0x140000000000, 0x1400000000000000 }, { 0x280000000000, 0x2800000000000000 }, { 0x500000000000, 0x5000000000000000 }, { 0xa00000000000, 0xa000000000000000 }, { 0x400000000000, 0x4000000000000000 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, };
uint64_t knight_move_table[64] = { 0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000, 0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020, 0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040, 0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000, 0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000, 0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000, 0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000 };
uint64_t king_move_table[64] = { 0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0, 0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000, 0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000, 0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000, 0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000, 0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000 };

void generate_pawn_move_table() {
    uint64_t bb;
    printf("{ ");
    for (int sq = 0; sq < 64; sq++) {
        if (sq < a2 || sq >= a8) {
            printf("{ 0x0, 0x0 }, ");
            continue;
        }

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
        if (sq < a2 || sq >= a8) {
            printf("{ 0x0, 0x0 }, ");
            continue;
        }
        printf("{ 0x%" PRIx64 ", 0x%" PRIx64 " }, ", pawn_attack_table[sq][0], pawn_attack_table[sq][1]);
    }

    printf("};");
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

int is_square_attacked(int sq, int side) {
    uint64_t bb, attack_bb;
    int is_attacked = 0;

    // Attacks by pawns
    bb = (side == white) ? pawn_attack_table[sq][black] : pawn_attack_table[sq][white];
    attack_bb = bb & ((side == white) ? board.bitboards[P] : board.bitboards[p]);

    // Attacks by knights
    bb = knight_move_table[sq];
    attack_bb |= bb & ((side == white) ? board.bitboards[N] : board.bitboards[n]);

    // Attacks by bishops & diagonal attacks by queen
    bb = bishop_moves(sq, board.bitboards[14]);
    print_bitboard(bb);
    attack_bb |= bb & ((side == white) ? board.bitboards[B] | board.bitboards[Q] : board.bitboards[n] | board.bitboards[q]);
    
    // Attacks by rook & straight attacks by queen
    bb = rook_moves(sq, board.bitboards[14]);
    attack_bb |= bb & ((side == white) ? board.bitboards[R] | board.bitboards[Q] : board.bitboards[r] | board.bitboards[q]);
    
    
    print_bitboard(attack_bb);
    return 0;
}

void generate_moves(MOVE_LIST_T *move_list) {
    /*
    1. Detect legal king moves.
    Evasion from sliding pieces (x-ray attacks through king)
    
    2. Check evasions and double check.
    Enpassant check evasion

    3. Pinned pieces

    4. Other moves 
    Enpassant discovered check
    */

    int from_sq;
    uint64_t attack_bb;
    

}

void make_move(uint16_t move) {
    
}