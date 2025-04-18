#include "../include/uci.h"
#include "../include/utils.h"
#include <stdio.h>

void parse_uci(const char* uci_string) {
    if (strcmp(uci_string, "uci") == 0) {
        printf("id name CHESS_ENGINE\n");
        printf("id author x4A81\n");
        printf("uciok\n");
    }

    else if (strcmp(uci_string, "quit") == 0) {
        graceful_exit("Received quit command...", 0);
    }

    else if (strcmp(uci_string, "isready") == 0) {
        setup_engine();
        printf("readyok\n");
    }

    else if (strcmp(uci_string, "ucinewgame") == 0) {
        // Clear things like transposition tables, killer move tables, board history, etc.
        reset_engine();
    }

    else if (strncmp(uci_string, "position", 8) == 0) {
        int offset = 9;
        char *extra_moves = NULL;
        // input the fen string
        if (strncmp(uci_string+offset, "startpos", 8) == 0) {
            setup_board("startpos");
            offset = 18;
            extra_moves = uci_string+offset;
        }

        else if (strncmp(uci_string+offset, "fen", 3) == 0) {
            offset = 13;
            setup_board(uci_string+offset);
            extra_moves = strstr(uci_string+offset, "moves");
        }

        // make any additional moves
        // if (strncmp(uci_string+offset, "moves", 5) == 0) {
        //     offset += 6;
        //     printf("%s\n", uci_string+offset);
        // }
        printf("%s\n", extra_moves);
    }
    
    else
        printf("Unknown command: %s\n", uci_string);
}