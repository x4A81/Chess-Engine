#include "../include/uci.h"
#include "../include/utils.h"
#include "../include/move.h"
#include "../include/board.h"
#include "../include/search.h"
#include "../tests/test.h"
#include "../include/eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

volatile int DEBUG = 0;
volatile int STOP_SEARCH = 0;
volatile int INFINITE = 0;
int MOVESTOGO = 0;
int MOVETIME = 0; // what the gui specifies.
int TIME_FOR_MOVE = 0; // What the engine determines.
ssize_t transposition_size = 32LL * 1024 * 1024;

int W_TIME = 0, B_TIME = 0, W_INC = 0, B_INC = 0;
clock_t start_time;

int depth_search = 0;

void* start_search_thread(void* arg) {
    int depth = *((int*)arg);
    free(arg);
    if (depth_search == 0) {
        if (MOVETIME == 0) {
            int timer = (board.side == white) ? W_TIME : B_TIME;
            timer /= (MOVESTOGO == 0) ? 40 : MOVESTOGO;
            timer -= 50;
            TIME_FOR_MOVE = timer;
        } else 
            TIME_FOR_MOVE = MOVETIME;
    }

    start_time = clock();
    search(depth);

    if (board.side == white)
        W_TIME += W_INC - TIME_FOR_MOVE;
    else
        B_TIME += B_INC - TIME_FOR_MOVE;

    return NULL;
}

int time_exceeded() {
    if (TIME_FOR_MOVE == 0) return 0;
    clock_t now = clock();
    return ((now - start_time) * 1000 / CLOCKS_PER_SEC) >= TIME_FOR_MOVE;
}

void uci_go(const char* go_string) {
    int depth = 18;
    MOVETIME = 0;
    INFINITE = 0;
    STOP_SEARCH = 0;
    depth_search = 0;

    const char* tok = strtok((char*)go_string, " ");
    while (tok != NULL) {
        if (strcmp(tok, "depth") == 0) {
            tok = strtok(NULL, " ");
            if (tok) depth = atoi(tok);
            depth_search = 1;

        } else if (strcmp(tok, "movetime") == 0) {
            tok = strtok(NULL, " ");
            if (tok) MOVETIME = atoi(tok);
        
        } else if (strcmp(tok, "infinite") == 0) {
            INFINITE = 1;
            depth_search = 1;

        } else if (strcmp(tok, "movestogo") == 0) {
            tok = strtok(NULL, " ");
            if (tok) MOVESTOGO = atoi(tok);
        
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
        } else if (strcmp(tok, "eval") == 0) {
            printf("cp: %d\n", eval());
            return;
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
        printf("option name Hash type spin default 32 min 16 max 64\n");
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
        if (strncmp(uci_string + 10, "name Hash value", 15) == 0) {
            long long size = atoll(uci_string + 26);
            if (size < 16 || size > 64) size = 32;
            transposition_size = size * 1024 * 1024;
        }
    }
    
    else
        printf("Unknown command: %s\n", uci_string);
}