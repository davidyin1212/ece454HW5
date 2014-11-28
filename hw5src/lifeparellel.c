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

typedef struct args
{
  int slice;
  char* outboard;
  char* inboard;
  int nrows;
  int ncols;
  int gens_max;
} Args;

void *thread(void *args);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

char* parellel_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{

    pthread_t thrd[4];
    int index[4];
    // int i;

    Args *args [4];

    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    const int LDA = nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
      for (i = 0; i < 4; i++) {
        args[i] = malloc (sizeof(int) * 4 + sizeof(char *) * 2);
        args[i]->slice = i;
        args[i]->outboard = outboard;
        args[i]->inboard = inboard;
        args[i]->nrows = nrows;
        args[i]->ncols = ncols;
        args[i]->gens_max = gens_max;
        pthread_create(&thrd[i], NULL, &thread, args[i]);
      }

      for(i = 0; i < 4; i++) {
        pthread_join(thrd[i], NULL);
      }
      // for (i = 0; i < nrows; i++)
      // {
      //     for (j = 0; j < ncols; j++)
      //     {
      //         const int inorth = mod (i-1, nrows);
      //         const int isouth = mod (i+1, nrows);
      //         const int jwest = mod (j-1, ncols);
      //         const int jeast = mod (j+1, ncols);

      //         const char neighbor_count = 
      //             BOARD (inboard, inorth, jwest) + 
      //             BOARD (inboard, inorth, j) + 
      //             BOARD (inboard, inorth, jeast) + 
      //             BOARD (inboard, i, jwest) +
      //             BOARD (inboard, i, jeast) + 
      //             BOARD (inboard, isouth, jwest) +
      //             BOARD (inboard, isouth, j) + 
      //             BOARD (inboard, isouth, jeast);

      //         BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

      //     }
      // }
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

void *thread (void * args) {
  int slice = ((Args *)args)->slice;
  printf("slice: %d\n", slice);
  // printf("arg 1: %p\n", (char *) args[1]);
  char *outboard = ((Args *)args)->outboard;
  char *inboard = ((Args *)args)->inboard;
  const int nrows = ((Args *)args)->nrows; 
  const int ncols = ((Args *)args)->ncols;
  printf("nrows: %d\n", nrows);
  printf("ncols: %d\n", ncols);
  // printf("gens_max: %d\n", gens_max);
  const int LDA = nrows;
  int i, j;
  int from = (slice*ncols)/4;
  int to = ((slice+1)*ncols)/4;

  for (i = 0; i < nrows; i++)
  {
    for (j = from; j < to; j++)
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
}


