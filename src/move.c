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

uint64_t pawn_move_table[64][2] = { { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x3, 0x1030000 }, { 0x7, 0x2070000 }, { 0xe, 0x40e0000 }, { 0x1c, 0x81c0000 }, { 0x38, 0x10380000 }, { 0x70, 0x20700000 }, { 0xe0, 0x40e00000 }, { 0xc0, 0x80c00000 }, { 0x300, 0x3000000 }, { 0x700, 0x7000000 }, { 0xe00, 0xe000000 }, { 0x1c00, 0x1c000000 }, { 0x3800, 0x38000000 }, { 0x7000, 0x70000000 }, { 0xe000, 0xe0000000 }, { 0xc000, 0xc0000000 }, { 0x30000, 0x300000000 }, { 0x70000, 0x700000000 }, { 0xe0000, 0xe00000000 }, { 0x1c0000, 0x1c00000000 }, { 0x380000, 0x3800000000 }, { 0x700000, 0x7000000000 }, { 0xe00000, 0xe000000000 }, { 0xc00000, 0xc000000000 }, { 0x3000000, 0x30000000000 }, { 0x7000000, 0x70000000000 }, { 0xe000000, 0xe0000000000 }, { 0x1c000000, 0x1c0000000000 }, { 0x38000000, 0x380000000000 }, { 0x70000000, 0x700000000000 }, { 0xe0000000, 0xe00000000000 }, { 0xc0000000, 0xc00000000000 }, { 0x300000000, 0x3000000000000 }, { 0x700000000, 0x7000000000000 }, { 0xe00000000, 0xe000000000000 }, { 0x1c00000000, 0x1c000000000000 }, { 0x3800000000, 0x38000000000000 }, { 0x7000000000, 0x70000000000000 }, { 0xe000000000, 0xe0000000000000 }, { 0xc000000000, 0xc0000000000000 }, { 0x30100000000, 0x300000000000000 }, { 0x70200000000, 0x700000000000000 }, { 0xe0400000000, 0xe00000000000000 }, { 0x1c0800000000, 0x1c00000000000000 }, { 0x381000000000, 0x3800000000000000 }, { 0x702000000000, 0x7000000000000000 }, { 0xe04000000000, 0xe000000000000000 }, { 0xc08000000000, 0xc000000000000000 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 }, { 0x0, 0x0 } };
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
        pawn_move_table[sq][0] |= ((pawn_move_table[sq][0] << 1) & 0xfefefefefefefefe) |
                                 ((pawn_move_table[sq][0] >> 1) & 0x7f7f7f7f7f7f7f7f);

        // Dbl
        if (sq <= h7 && sq >= a7)
            pawn_move_table[sq][0] |= bb >> 16;

        // White Pawns

        // Single Push
        pawn_move_table[sq][1] = bb << 8;

        // Captures
        pawn_move_table[sq][1] |= ((pawn_move_table[sq][1] << 1) & 0xfefefefefefefefe) |
                                  ((pawn_move_table[sq][1] >> 1) & 0x7f7f7f7f7f7f7f7f);

        // Dbl
        if (sq >= a2 && sq <= h2)
            pawn_move_table[sq][1] |= bb << 16;

        printf("{ 0x%" PRIx64 ", 0x%" PRIx64 " }, ", pawn_move_table[sq][0], pawn_move_table[sq][1]);
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

uint64_t rook_magics[64] = { 0x9008298799940100, 0x612636892472837120, 0x4516948388151296, 0x360499355603435528, 0x316663644291072, 0x1152947927246717954, 0x297241423967748624, 0x36134970760110336, 0x1441160747719098626, 0x1224979721419260032, 0x36099718207570048, 0x18155711523717122, 0x291105367261440, 0x618476404736, 0x4611756558971068432, 0x35202155939360, 0x36592090603864068, 0x72341303219724292, 0x1153484458050191624, 0x9802084591152923648, 0x297378313163505664, 0x143074219008048, 0x288235945415674368, 0x2450802630881051136, 0x9516158797782974912, 0x22518582336815616, 0x144154770503369728, 0x1130297957613569, 0x567417289838592, 0x6755674856095777, 0x145204254539793, 0x1236952685056, 0x1153000806883008768, 0x105656195485696, 0x299491583372304392, 0x5764677960766455840, 0x2251885713035520, 0x18579549902151688, 0x4612319354841743872, 0x142936545296768, 0x8800657031552, 0x21994544457728, 0x108174369184288768, 0x43016782337, 0x860086092800, 0x18023220374405136, 0x28657096589376, 0x2261698710863880, 0x4582798841151488, 0x2305845243675148289, 0x71539160532992, 0x36028797087252484, 0x137577381920, 0x138043006980, 0x109985557127168, 0x5356889505300480, 0x35210812989952, 0x9224533129724693120, 0x9007199321883904, 0x4611686087416351360, 0x137447408192, 0x144115738100531232, 0x2454497084393398408, 0x9224075793016193032 };
uint_fast64_t bishop_magics[64] = { 0x2449975798602436608, 0x144115190223339520, 0x4683743612465315840, 0x2341871806232662016, 0x1297036709862572040, 0x586030918691454977, 0x288239172244733952, 0x144115188075855872, 0x4644337115725824, 0x564051612532737, 0x1688849860280448, 0x2314990946229420064, 0x4573968372076560, 0x2287018813947904, 0x5629568262078464, 0x1301682129310056448, 0x2308385088703823872, 0x162553998073659392, 0x9042935530127364, 0x1130366672830464, 0x54984171323392, 0x327084918322757632, 0x144159168542015744, 0x720602328926781956, 0x27110799939993600, 0x290768633104171008, 0x36171742120525824, 0x146367539793887232, 0x6052908302424113152, 0x1153484531869745152, 0x9290890435657728, 0x1297071945774268418, 0x36600612053336192, 0x4508582057869314, 0x9289516112150528, 0x36312471288414208, 0x141012501004288, 0x1126037681373184, 0x283676214559760, 0x283745001144320, 0x36039929848922368, 0x1234013803944347648, 0x11259553153286144, 0x148619407319629824, 0x2256198936297984, 0x140876002099201, 0x72578506948624, 0x2852687222800385, 0x148691905763869185, 0x2537948269518853, 0x202666518851485698, 0x9293278474665984, 0x1226105582701838336, 0x36174005639708680, 0x144185698654913024, 0x36104663591846400, 0x2639128090659274752, 0x289514640633757696, 0x289358509979484416, 0x2252916548173824, 0x589409166622720, 0x1127008616615940, 0x141906289901568, 0x1297056622413089024 };

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
        rook_magics[sq] = find_magics(sq, rook_shifts[sq], 1);
        bishop_magics[sq] = find_magics(sq, bishop_shifts[sq], 0);
        printf("0x%" PRIu64 ", ", rook_magics[sq]);
    }

    printf("};\n\n{ ");
    for (sq = 0; sq < 64; sq++) {
        printf("0x%" PRIu64 ", ", bishop_magics[sq]);
    }

    printf("};");
}