#ifndef UCI_H_INCLUDE
#define UCI_H_INCLUDE

#include <string.h>

// Parses and acts on UCI commands.
void parse_uci(const char* uci_string);

// Acts on the UCI 'go' command.
void uci_go(const char* go_string);

#endif