#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE

#include <stdlib.h>


// Cleanup the program and exit the program gracefully.
void graceful_exit(const char* message, int exit_code);

// Sets up the engine and initialises things such as transposition tables.
void setup_engine();

// Resets engine variables such as transposition tables.
void reset_engine();

// Sets up the board from a UCI position command.
void setup_board(const char *fen);

// Parses a move string and return the encoded move.
int parse_move(char* move_string);

void build_string(char *buf, size_t buf_size, size_t *len, const char *fmt, ...);

#endif