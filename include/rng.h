#ifndef RNG_H_INCLUDE
#define RNG_H_INCLUDE

#include <stdint.h>

// Generates a magic number.
uint64_t rng_magic();

// Initalises the Zobrist hash keys.
void initialise_hash_keys();

// Returns a hash of the board.
uint64_t hash_board();

#endif