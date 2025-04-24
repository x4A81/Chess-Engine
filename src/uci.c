#include "../include/uci.h"
#include "../include/utils.h"
#include "../include/move.h"
#include "../include/board.h"
#include "../include/search.h"
#include "../tests/test.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

volatile int DEBUG = 0;
volatile int STOP_SEARCH = 0;
volatile int INFINITE = 0;
int MOVETIME = 0;
__ssize_t transposition_size = 32LL * 1024 * 1024 * 1024;

float W_TIME = 0, B_TIME = 0, W_INC = 0, B_INC = 0;
clock_t start_time;

void* start_search_thread(void* arg) {
    int depth = *((int*)arg);
    free(arg);
    start_time = clock();
    search(depth);
    return NULL;
}

int time_exceeded() {
    if (MOVETIME == 0) return 0;
    clock_t now = clock();
    return ((now - start_time) * 1000 / CLOCKS_PER_SEC) >= MOVETIME;
}

void uci_go(const char* go_string) {
    int depth = 18;
    MOVETIME = 0;
    INFINITE = 0;
    STOP_SEARCH = 0;

    const char* tok = strtok((char*)go_string, " ");
    while (tok != NULL) {
        if (strcmp(tok, "depth") == 0) {
            tok = strtok(NULL, " ");
            if (tok) depth = atoi(tok);

        } else if (strcmp(tok, "movetime") == 0) {
            tok = strtok(NULL, " ");
            if (tok) MOVETIME = atoi(tok);
        
        } else if (strcmp(tok, "infinite") == 0) {
            INFINITE = 1;
        
        } else if (strcmp(tok, "wtime") == 0) {
            tok = strtok(NULL, " ");
            if (tok) W_TIME = atof(tok);
        
        } else if (strcmp(tok, "btime") == 0) {
            tok = strtok(NULL, " ");
            if (tok) B_TIME = atof(tok);
        
        } else if (strcmp(tok, "winc") == 0) {
            tok = strtok(NULL, " ");
            if (tok) W_INC = atof(tok);
        
        } else if (strcmp(tok, "binc") == 0) {
            tok = strtok(NULL, " ");
            if (tok) B_INC = atof(tok);
        
        } else if (strcmp(tok, "perft") == 0) {
            tok = strtok(NULL, " ");
            if (tok) {
                perft_test(atoi(tok), atoi(tok), 0);
                return;
            }
        } 

        tok = strtok(NULL, " ");
    }

    // Create a thread for the search algorithm.
    pthread_t search_thread;
    int* pdepth = malloc(sizeof(int));
    *pdepth = depth;
    pthread_create(&search_thread, NULL, start_search_thread, pdepth);
    pthread_detach(search_thread);
}

void parse_uci(const char* uci_string) {

    // Handles uci commands.

    if (strcmp(uci_string, "uci") == 0) {
        printf("id name CHESS_ENGINE\n");
        printf("id author x4A81\n");
        printf("option name Hash type spin default 32 min 64 max 16\n");
        printf("uciok\n");
    }

    else if (strcmp(uci_string, "quit") == 0) {
        graceful_exit(NULL, 0);
    }

    else if (strcmp(uci_string, "isready") == 0) {
        setup_engine(transposition_size);
        printf("readyok\n");
    }

    else if (strcmp(uci_string, "ucinewgame") == 0) {
        
        // Clear things like transposition tables, killer move tables, board history, etc.
        reset_engine();
        setup_engine(transposition_size);
    }

    else if (strncmp(uci_string, "position", 8) == 0) {
        int offset = 9;
        char *extra_moves = NULL;
        
        // Input the fen string.
        if (strncmp(uci_string+offset, "startpos", 8) == 0) {
            setup_board("startpos");
            offset = 18;
            extra_moves = (char*)uci_string+offset;
        }

        else if (strncmp(uci_string+offset, "fen", 3) == 0) {
            offset = 13;
            setup_board(uci_string+offset);
            extra_moves = strstr(uci_string+offset, "moves");
        }

        // Make any additional moves.
        if (strncmp(uci_string+offset, "moves", 5) == 0) {
            extra_moves += strlen("moves ");
            // Parse the moves.
            char *move_string = strtok(extra_moves, " ");
            while (move_string) {
                make_move(parse_move(move_string));
                move_string = strtok(NULL, " ");
            }
        }
    }

    else if (strncmp(uci_string, "debug", 5) == 0) {
        if (strncmp(uci_string+6, "on", 2) == 0)
            DEBUG = 1;
        else
            DEBUG = 0;
    }

    else if (strcmp(uci_string, "stop") == 0) {
        STOP_SEARCH = 1;
    }

    else if (strncmp(uci_string, "go", 2) == 0)
        uci_go(uci_string+3);

    else if (strncmp(uci_string, "d", 1) == 0)
        print_board();

    else if (strncmp(uci_string, "setoption", 9) == 0) {
        if (strncmp(uci_string + 10, "name Hash value", 15) == 0)
            transposition_size = atoll(uci_string + 26) * 1024 * 1024 * 1024;
    }
    
    else
        printf("Unknown command: %s\n", uci_string);
}