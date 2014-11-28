/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define MIN(X,Y)  ((X) < (Y) ? (X) : (Y))

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

int L1 = 128;
int W1 = 8;
    char*
sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    printf("using sequential\n");
    const int LDA = nrows;
    int curgen, i, j, i1, j1;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (i = 0; i < nrows; i++)
        {
            for (j = 0; j < ncols; j++)
            {
              for (i1 = i; i < MIN(nrows, i + L1); i++) {
                for (j1 = j; j < MIN(ncols, j + W1); j++) {
                const int inorth = mod (i1-1, nrows);
                const int isouth = mod (i1+1, nrows);
                const int jwest = mod (j1-1, ncols);
                const int jeast = mod (j1+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j1) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i1, jwest) +
                    BOARD (inboard, i1, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j1) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i1, j1) = alivep (neighbor_count, BOARD (inboard, i1, j1));
            }
            }
            }
        }
        SWAP_BOARDS( outboard, inboard );

    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}


