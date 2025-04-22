#include "../../include/transposition.h"
#include "../../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TRANSPOSITION_T* transposition_tt = NULL;
__ssize_t table_size = 0;

void initialise_transposition(__ssize_t size) {

    if (transposition_tt)
        return;

    // Ensure that size is a power of 2.
    __ssize_t power = 1;
    while (power << 1 <= size) power <<= 1;
    size = power;

    do {
        transposition_tt = (TRANSPOSITION_T*)malloc(size * sizeof(TRANSPOSITION_T));
        if (transposition_tt == NULL) size >>= 1; // Halve and try again.
    } while (transposition_tt == NULL && size > 0);

    if (transposition_tt) {
        table_size = size;
        memset(transposition_tt, 0, size * sizeof(TRANSPOSITION_T));
    } else {
        fprintf(stderr, "Failed to allocate transposition table.\n");
        graceful_exit("Failed to allocate transposition table.", ERR_TRANSPOSITION_INITIALISE);
    }
}

void cleanup_transposition() {
    if (transposition_tt) {
        free(transposition_tt);
        transposition_tt = NULL;
        table_size = 0;
    }
}

void clear_transposition() {
    if (transposition_tt) {
        memset(transposition_tt, 0, table_size * sizeof(TRANSPOSITION_T));
        table_size = 0;
    }
}

TRANSPOSITION_T *probe_transposition(uint64_t key, int depth) {
    int index = key & (table_size - 1);
    TRANSPOSITION_T *entry = &transposition_tt[index];

    if (entry->key == key && entry->depth >= depth)
            return entry;
    return NULL;
}

void store_transposition(uint64_t key, int depth, int score, int type, int hash_move) {
    int index = key & (table_size - 1);
    TRANSPOSITION_T *entry = &transposition_tt[index];

    entry->key = key;
    entry->depth = depth;
    entry->score = score;
    entry->type = type;
    entry->hash_move = hash_move;
}
