#ifndef RNG_H_INCLUDE
#define RNG_H_INCLUDE

#include <stdint.h>

uint64_t rng_magic();
void initialise_hash_keys();
uint64_t hash_board();

#endif