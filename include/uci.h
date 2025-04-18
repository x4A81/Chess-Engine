#ifndef UCI_H_INCLUDE
#define UCI_H_INCLUDE

#include <string.h>

void parse_uci(const char* uci_string);
void uci_go(const char* go_string);
int parse_move(const char* move_string);

#endif