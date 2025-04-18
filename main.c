#include "include/board.h"
#include "include/uci.h"
#include "tests/test.h"
#include "include/utils.h"
#include <stdio.h>

BOARD_T board;

int main() {
    char uci_string[256];
    while (fgets(uci_string, sizeof(uci_string), stdin)) {
        uci_string[strcspn(uci_string, "\n")] = 0;
        parse_uci(uci_string);
    }

    graceful_exit(NULL, 0);
    return 0;
}
