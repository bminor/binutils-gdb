/* BeginSourceFile more_steps.c

  This file creates a lot of threads which then execute
  in parallel, so that wdb can be tested on handling
  simultaneous thread events.

  To compile:

      cc -Ae +DA1.0 -g -o more_steps -lpthread more_steps.c

  To run:
  
     more_threads
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define N_THREADS 3
#define PHASES 3

typedef enum {
  ZERO,
  ONE,
  TWO,
  THREE
} phase_t;

/* Uncomment to turn on debugging output */
/* #define DEBUG  */

/* Locks.
 */
int lock_one; /* Main W, others R */
int lock_two; /* ditto */
int lock_end[ N_THREADS ]; /* Main R, others R[i] */
int phase[ N_THREADS ];

/* Routine for each thread to run.
 */
void *spin( vp )
    void * vp;
{
    int me = (int) vp;
    int i;

    lock_end[ me ] = TRUE;

    phase[ me ] = ONE;

    while( lock_one );

    phase[ me ] = TWO;

    while( lock_two );

    phase[ me ] = THREE;

    lock_end[ me ] = FALSE;
}

void
do_pass()
{
    int i;
    pthread_t t[ N_THREADS ];
    int err;
    int done;

    /* Start N_THREADS threads, then join them so
     * that they are terminated.
     */
    for( i = 0; i < N_THREADS; i++ ) {
        err = pthread_create( &t[i], NULL, spin, (void *)i );
        if( err != 0 ) {
            printf( "== Start/stop, error in thread %d create\n", i );
        }
    }

    /* Do phase 1.
     */
    lock_one = FALSE;

    /* Do phase 2.
     */
    lock_two = FALSE;
    
    /* Be done.
     */
    done = 0;
    while( !done ) {

       /* Be optimistic.
        */
       done = 1;
       for( i = 0; i < N_THREADS; i++ ) {
           if( lock_end[i] ) {
               /* Thread "i" is not ready yet.
                */
               done = 0;
               break;
           }
       }
    }
    
    /* Finish up
     */
    for( i = 0; i < N_THREADS; i++ ) {
        err = pthread_join(t[i], NULL );    /* Line 105 */
        if( err != 0 ) {                    /* Line 106 */
            printf( "== Start/stop, error in thread %d join\n", i );
        }
    }

    i = 10;  /* Line 109.  Null line for setting bpts on. */
}

main( argc, argv )
int    argc;
char **argv;
{
    int i;
    
    /* Init
     */
    lock_one = TRUE;
    lock_two = TRUE;
    for( i = 0; i < N_THREADS; i++ ) {
        lock_end[i] = TRUE;
        phase[i]    = ZERO;
    }
    
    do_pass();
    return(0);
}
