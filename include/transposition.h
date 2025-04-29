#ifndef TRANSPOSITION_H_INCLUDE
#define TRANSPOSITION_H_INCLUDE

#include <stdint.h>

#define EXACT 0
#define LOWERBOUND 1
#define UPPERBOUND 2

typedef struct TRANSPOSITION_T {
    uint64_t key;
    uint16_t hash_move;
    int score;
    int depth;
    int type;
} TRANSPOSITION_T;

void initialise_transposition(ssize_t size);
void cleanup_transposition();
void clear_transposition();
TRANSPOSITION_T *probe_transposition(uint64_t key, int depth);
void store_transposition(uint64_t key, int depth, int score, int type, int hash_move);

#endif
