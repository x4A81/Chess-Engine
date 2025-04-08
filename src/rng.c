#include "../include/rng.h"

uint32_t seed = 912345432;

uint32_t rng32() {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

uint64_t rng64() {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(rng32()) & 0xffff;
    u2 = (uint64_t)(rng32()) & 0xffff;
    u3 = (uint64_t)(rng32()) & 0xffff;
    u4 = (uint64_t)(rng32()) & 0xffff;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t rng_magic() {
    return rng64() & rng64() & rng64() & rng64();
}