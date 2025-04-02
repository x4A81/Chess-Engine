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
                char piece_char = ((r+f)%2 == 0) ? ' ' : '#';
                if (i==1) {
                    for (int bb = p; bb <= K; bb++) {
                        if (GET_BIT(board.bitboards[bb], (8*r+f)))
                            piece_char = piece_chars[bb];
                    }
                }
                
                if ((r+f)%2 == 0) {
                    printf("  %c  ", piece_char);
                } else {
                    if (piece_char == '#')
                        printf("#####");
                    else
                        printf("# %c #", piece_char);
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
    int char_to_piece[] = {
        ['P'] = 0, ['N'] = 1, ['B'] = 2, ['R'] = 3, ['Q'] = 4, ['K'] = 5,
        ['p'] = 6, ['n'] = 7, ['b'] = 8, ['r'] = 9, ['q'] = 10, ['k'] = 11};
    RESET_BOARD();
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int sq = 8*r+f;
            if ((*fen >= 'a' && *fen <= 'z') || ((*fen >= 'A' && *fen <= 'Z'))) {
                int piece = char_to_piece[*fen];
                SET_BIT(board.bitboards[piece], sq);
                *fen++;
              }
        
              if (*fen >= '0' && *fen <= '9') {
                int offset = *fen - '0', piece = -1;
                for (int bb = 0; bb <= 12; bb++) {
                  if (GET_BIT(board.bitboards[bb], sq)) {
                    piece = bb;
                    break;
                  }
                }
        
                if (piece == -1) 
                    f--;
                f += offset;
                *fen++;
              }
        
              if (*fen == '/') 
                *fen++;
        }
    }
}