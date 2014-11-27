/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include "pthread.h"

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

void *thread(void **args);

char* parellel_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{

    pthread_t thrd[4];
    int index[4];

    void *args [6] = {0, outboard, inboard, nrows, ncols, gens_max};
    for (int i = 0; i < num_threads; i++) {
      // index[i] = i;
      args[0] = (void*) i;
      pthread_create(&thrd[i], NULL, &thread, args);
    }

    for(int i = 0; i < num_threads; i++) {
      pthread_join(thrd[i], NULL);
    }

    // /* HINT: in the parallel decomposition, LDA may not be equal to
    //    nrows! */
    // const int LDA = nrows;
    // int curgen, i, j;

    // for (curgen = 0; curgen < gens_max; curgen++)
    // {
    //     /* HINT: you'll be parallelizing these loop(s) by doing a
    //        geometric decomposition of the output */
    //     for (i = 0; i < nrows; i++)
    //     {
    //         for (j = 0; j < ncols; j++)
    //         {
    //             const int inorth = mod (i-1, nrows);
    //             const int isouth = mod (i+1, nrows);
    //             const int jwest = mod (j-1, ncols);
    //             const int jeast = mod (j+1, ncols);

    //             const char neighbor_count = 
    //                 BOARD (inboard, inorth, jwest) + 
    //                 BOARD (inboard, inorth, j) + 
    //                 BOARD (inboard, inorth, jeast) + 
    //                 BOARD (inboard, i, jwest) +
    //                 BOARD (inboard, i, jeast) + 
    //                 BOARD (inboard, isouth, jwest) +
    //                 BOARD (inboard, isouth, j) + 
    //                 BOARD (inboard, isouth, jeast);

    //             BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

    //         }
    //     }
    //     SWAP_BOARDS( outboard, inboard );

    // }

    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}

void *thread (void ** args) {
  int slice = *((int *)args[0]);
  char *outboard = (char *) args[1];
  char *inboard = (char *) args[2];
  const int nrows = *((int *)args[3]); 
  const int ncols = *((int *)args[4]);
  const int gens_max = *((int *)args[5]);
  const int LDA = nrows/4;
  int curgen, i, j;
  int from = (slice*nrows)/num_threads;
  int to = ((slice+1)*nrows)/num_threads;

  for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (i = from; i < to; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

            }
        }
        SWAP_BOARDS( outboard, inboard );

    }  
}


