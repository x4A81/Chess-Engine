#include "../include/board.h"
#include <stdio.h>

void print_board() {
    char *piece_chars = "pnbrqkPNBRQK";
    for (int r = 7; r >= 0; r--) {
        printf("+---+---+---+---+---+---+---+---+\n");
        for (int f = 0; f < 8; f++) {
            char piece = ' ';
            for (int bb = p; bb <= K; bb++) {
                if (GET_BIT(board.bitboards[bb], 8*r+f)) {
                    piece = piece_chars[bb];
                    break;
                }
            }

            printf("| %c ", piece);
        }

        printf("| %d\n", r+1);
    }

    // Print board info.
    printf("+---+---+---+---+---+---+---+---+\n  a   b   c   d   e   f   g   h\n");
    printf("Side To move: %s\n", (board.side == white) ? "white" : "black");
    printf("Castling: %d\n", board.castling);
    printf("Enpassant square: %s\n", (board.enpassant == -1) ? "-" : (char[]){'a' + (board.enpassant % 8), '1' + (board.enpassant / 8)});
    printf("Halfmove Clock: %d\n", board.halfmove);
    printf("Full moves: %d\n", board.fullmove);
    printf("Key: %d\n", board.hash_key);
}

void print_bitboard(uint64_t bb) {
    printf("\nBitboard val: %lu \n", bb);
    for (int r = 7; r >= 0; r--) {
      printf(" %d |", r+1);
      for (int f = 0; f < 8; f++) {
        int sq = 8*r+f;
        printf(" %d ", GET_BIT(bb, sq) ? 1 : 0);
      }
  
      printf("\n");
    }
  
    printf("     a  b  c  d  e  f  g  h\n");
}

void load_fen(const char *fen) {
    int char_to_piece[] = {
        ['p'] = 0, ['n'] = 1, ['b'] = 2, ['r'] = 3, ['q'] = 4, ['k'] = 5,
        ['P'] = 6, ['N'] = 7, ['B'] = 8, ['R'] = 9, ['Q'] = 10, ['K'] = 11};
    RESET_BOARD();

    for (int r = 7; r >= 0; r--) {
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
    int f = fen[0] - 'a', r = (fen[1] - '0') - 1;
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

  // Set up the white, black, all bitboards.
  board.bitboards[12] = board.bitboards[p] | board.bitboards[n] | board.bitboards[b] | board.bitboards[r] | board.bitboards[q] | board.bitboards[k];
  board.bitboards[13] = board.bitboards[P] | board.bitboards[N] | board.bitboards[B] | board.bitboards[R] | board.bitboards[Q] | board.bitboards[K];
  board.bitboards[14] = board.bitboards[12] | board.bitboards[13];
}