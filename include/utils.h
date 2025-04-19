#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE

void graceful_exit(const char* message, int exit_code);
void setup_engine();
void reset_engine();
void setup_board(const char *fen);
int parse_move(char* move_string);

#endif