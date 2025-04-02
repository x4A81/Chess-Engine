#include "include/board_utils.h"
#include "include/defs.h"
#include <stdio.h>
#include <string.h>

void print_board() {
    char *piece_chars = "pnbrqkPNBRQK";
    printf("--------------------------------------------\n");
    for (int r = 7; r >= 0; r--) {
        for (int i = 0; i < 3; i++) {
            if (i==1) printf("%d ", r+1);
            else printf("| ");
            for (int f = 0; f < 8; f++) {
                char piece_char = ((r+f)%2 == 0) ? ' ' : '+';
                if (i==1) {
                    for (int bb = p; bb <= K; bb++) {
                        if (GET_BIT(board.bitboards[bb], (8*r+f)))
                            piece_char = piece_chars[bb];
                    }
                }
                
                if ((r+f)%2 == 0) {
                    printf("  %c  ", piece_char);
                } else {
                    printf("++%c++", piece_char);
                }
            }
        
            printf(" |");
            printf("\n");
        }
    }
    printf("----a----b----c----d----e----f----g----h----\n");
}

void print_bitboard(uint64_t bb) {
    int r, f, sq;
    printf("\nBitboard val: %lu \n", bb);
    for (r = 7; r >= 0; r--) {
      printf(" %d |", r + 1);
      for (f = 0; f < 8; f++) {
        sq = 8*r+f;
        printf(" %d ", GET_BIT(bb, sq) ? 1 : 0);
      }
  
      printf("\n");
    }
  
    printf("     a  b  c  d  e  f  g  h\n");
}

void setup_board(char *fen) {
    for (int bb = p; bb <= K; bb++)
        memset(1ULL, board.bitboards[bb], 8);
}