#include "../include/board.h"
#include <stdio.h>

void print_board() {
    char *piece_chars = "pnbrqkPNBRQK";
    printf("--------------------------------------------\n");
    for (int r = 0; r < 8; r++) {
        for (int i = 0; i < 3; i++) {
            if (i==1) printf("%d ", 8-r);
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
    printf("Side To move: %s\n", (board.side == white) ? "white" : "black");
    printf("Castling: %d\n", board.castling);
    printf("Enpassant square: %s\n", " ");
    printf("Halfmove Clock: %d\n", board.halfmove);
    printf("Full moves: %d\n", board.fullmove);
}

void print_bitboard(uint64_t bb) {
    printf("\nBitboard val: %lu \n", bb);
    for (int r = 0; r < 8; r++) {
      printf(" %d |", 8-r);
      for (int f = 0; f < 8; f++) {
        int sq = 8*r+f;
        printf(" %d ", GET_BIT(bb, sq) ? 1 : 0);
      }
  
      printf("\n");
    }
  
    printf("     a  b  c  d  e  f  g  h\n");
}

void setup_board(char *fen) {
    int char_to_piece[] = {
        ['p'] = 0, ['n'] = 1, ['b'] = 2, ['r'] = 3, ['q'] = 4, ['k'] = 5,
        ['P'] = 6, ['N'] = 7, ['B'] = 8, ['R'] = 9, ['Q'] = 10, ['K'] = 11};
    RESET_BOARD();

    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int sq = 8*r+f;
            if ((*fen >= 'a' && *fen <= 'z') || ((*fen >= 'A' && *fen <= 'Z'))) {
                int piece = char_to_piece[*fen];
                SET_BIT(board.bitboards[piece], sq);
                *fen++;
            } else if (*fen >= '1' && *fen <= '8') {
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

    *fen++;
    board.side = (*fen == 'w') ? white : black;
    
    fen += 2;
    while (*fen != ' ') {
        switch (*fen) {
        case 'K':
        board.castling |= wkingside; break;
        case 'Q':
        board.castling |= wqueenside; break;
        case 'k':
        board.castling |= bkingside; break;
        case 'q':
        board.castling |= bqueenside; break;
        default:
        break;
        }

    *fen++;
  }

  *fen++;
  if (*fen != '-') {
    int f = fen[0] - 'a', r = 8 - (fen[1] - '0');
    board.enpassant = 8*r+f;
    *fen++;
  }

  *(fen += 2);
  if (*fen != '\0') {
    board.halfmove = 0;
    while (*fen != ' ') {
      board.halfmove = board.halfmove * 10 + (fen[0] - '0');
      *fen++;
    }

    *fen++;

    board.fullmove = 0;
    while (*fen != ' ' && *fen != '\0') {
      board.fullmove = board.fullmove * 10 + (fen[0] - '0');
      *fen++;
    }
  }
}