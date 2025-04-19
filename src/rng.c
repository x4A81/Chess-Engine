#include "../include/rng.h"
#include "../include/board.h"
#include <string.h>

uint32_t seed = 912345432;

uint32_t rng32() {

    // Returns a 32 bit psuedo random number.
    
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

uint64_t rng64() {

    // Returns a 64 bit psuedo random number.

    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(rng32()) & 0xffff;
    u2 = (uint64_t)(rng32()) & 0xffff;
    u3 = (uint64_t)(rng32()) & 0xffff;
    u4 = (uint64_t)(rng32()) & 0xffff;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t rng_magic() {

    // Returns a 64 bit magic number.

    return rng64() & rng64() & rng64() & rng64();
}

uint64_t zobrist_keys[781] = {0UL};

void initialise_hash_keys() {

    // Clears then initialises zobrist keys.

    memset(zobrist_keys, 0ULL, sizeof(zobrist_keys));
    for (int key = 0; key < 781; key++)
        zobrist_keys[key] = rng64();
}

uint64_t hash_board() {

    // hashes the board.

    // For each piece on each sq.
    uint64_t hash = 0ULL;
    for (int sq = 0; sq < 64; sq++) {
        for (int bb = p; bb <= K; bb++) {
            if (GET_BIT(board.bitboards[bb], sq))
                hash ^= zobrist_keys[bb * 64 + sq];
        }
    }


    // Indicate side to move is black.
    if (board.side == black)
        hash ^= zobrist_keys[769];
    

    // Castling rights.
    if (board.castling & wkingside)
        hash ^= zobrist_keys[770];
    if (board.castling & wqueenside)
        hash ^= zobrist_keys[771];
    if (board.castling & bkingside)
        hash ^= zobrist_keys[772];
    if (board.castling & bqueenside)
        hash ^= zobrist_keys[773];


    // Available enpassant square if any.
    if (board.enpassant != -1) {
        int file = board.enpassant % 8;
        hash ^= zobrist_keys[774+file];
    }

    return hash;
}