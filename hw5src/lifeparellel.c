/*****************************************************************************
 * lifeparellel.c
 * What we do here is first we parallelize by spliting on nrows by 4 threads we
 * then do some dp because we know that in each iteration atleast 5 of the previous
 * values are used again from the previous iteration so we store them as local
 * variables so that we don't have to read them again.
 * We then use function inline to increase the further
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

//list of arguments that will be pased into the thread this is sotred as a struct to make it easier to pass in
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

//We create a 4 new threads that does the geometric decompesition of nrows so it runs what the seq code use to
//but on a quater of the rows we then join it back and do the swap
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
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {

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

//Take in the arguments form the args struct then splits on the nrows into 4 threads
void *thread (void * args) {
  int slice = ((Args *)args)->slice;
  char *outboard = ((Args *)args)->outboard;
  char *inboard = ((Args *)args)->inboard;
  const int nrows = ((Args *)args)->nrows; 
  const int ncols = ((Args *)args)->ncols;
  int i, j;
  int from = (slice*nrows)/4;
  int to = ((slice+1)*nrows)/4;
  char block[8];
  for (i = from; i < to; i++)
  {
    //stores the values at the begining of i so it could be a wrap around value or the first value in the grid
    //stores also the value above and below the current position
    const int inorth = (i == 0) ? (nrows-1) : (i-1);
    const int isouth = (i == nrows-1) ? (0) : (i+1);
    const int start_of_inorth = inorth * ncols;
    const int start_of_isouth = isouth * ncols;
    const int start_of_i = i * ncols;

    //stores the previous value that are above and below the previous iteration
    int prevpair = inboard[start_of_inorth + (ncols-1)] + inboard[start_of_isouth + (ncols-1)];
    //stores the node that you were previously at
    int prevnode = inboard[start_of_i + (ncols-1)];
    //stores the current values that are above below the current node
    int curpair = inboard[start_of_inorth] + inboard[start_of_isouth];

    for (j = 0; j < ncols; j++)
    {
      //calculates teh rest of the pairs which are the ones infront of the current as well as the next node in the order of j
      const int jwest = (j == 0) ? (ncols-1) : (j-1);
      const int jeast = (j == ncols-1) ? (0) : (j+1);
      int nextpair = inboard[start_of_inorth + jeast] + inboard[start_of_isouth + jeast];
      int nextnode = inboard[start_of_i + jeast];
      //sums them all the get the neighbor count
      int neighbor_count = prevpair + prevnode + curpair + nextpair + nextnode; 
      char count = neighbor_count;
      outboard[start_of_i + j] = (!inboard[start_of_i + j] && (count == 3)) || (inboard[start_of_i + j] && (count >= 2) && (count <= 3));
      //updates the values so that the values are correct for the next iteration
      prevpair = curpair;
      curpair = nextpair;
      prevnode = inboard[start_of_i + j];
    }
  }
  pthread_exit(NULL);
}


