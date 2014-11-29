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

#define MIN(X,Y)  ((X) < (Y) ? (X) : (Y))

#define MOD(x,m) ((x < 0) ? ((x % m) + m) : (x % m))

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
  // printf("slice: %d\n", slice);
  // printf("arg 1: %p\n", (char *) args[1]);
  char *outboard = ((Args *)args)->outboard;
  char *inboard = ((Args *)args)->inboard;
  const int nrows = ((Args *)args)->nrows; 
  const int ncols = ((Args *)args)->ncols;
  // printf("nrows: %d\n", nrows);
  // printf("ncols: %d\n", ncols);
  // printf("gens_max: %d\n", gens_max);
  const int LDA = nrows;
  int i, j;
  int from = (slice*nrows)/4;
  int to = ((slice+1)*nrows)/4;
  char block[8];
  for (i = from; i < to; i++)
  {
    const int inorth = (i == 0) ? (nrows-1) : (i-1);
    const int isouth = (i == nrows-1) ? (0) : (i+1);
    const int start_of_inorth = inorth * ncols;
    const int start_of_isouth = isouth * ncols;
    const int start_of_i = i * ncols;

    int prevpair = inboard[start_of_inorth + (ncols-1)] + inboard[start_of_isouth + (ncols-1)] - '0';
    int prevnode = inboard[start_of_i + (ncols-1)] - '0';
    int curpair = inboard[start_of_inorth] + inboard[start_of_isouth] - '0';

    for (j = 0; j < ncols; j++)
    {
      const int jwest = (j == 0) ? (ncols-1) : (j-1);
      const int jeast = (j == ncols-1) ? (0) : (j+1);
      // printf("i: %d\n", i);
      // printf("j: %d\n", j);
      int nextpair = inboard[start_of_inorth + jeast] + inboard[start_of_isouth + jeast] - '0';
      int nextnode = inboard[start_of_i + jeast] - '0';
      int neighbor_count = prevpair + prevnode + curpair + nextpair + nextnode; 
      char count = neighbor_count;
      outboard[start_of_i + j] = (!inboard[start_of_i + j] && (count == 3)) || (inboard[start_of_i + j] && (count >= 2) && (count <= 3));
      prevpair = curpair;
      curpair = nextpair;
      prevnode = nextnode;
    }
  }
  pthread_exit(NULL);
}


