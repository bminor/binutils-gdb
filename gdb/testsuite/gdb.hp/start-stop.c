/* BeginSourceFile start_stop.c

  This file creates and deletes threads, so that wdb
  can be tested on thread delete.

  To compile:

      cc -Ae +DA1.0 -g -o start_stop -lpthread start_stop.c

  To run:
  
     start_stop     --normal run
     start_stop 1   --waits in each thread to keep it alive.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define OUTER_LOOP_COUNT 3
#define N_THREADS 3
#define MAX_LOCAL_VAL 40

/* Uncomment to turn on debugging output */
/* #define START_DEBUG  */

/* True if waiting for attach.
 */
int wait_here;

/* Thing to check for debugging purposes.
*/
int a_global = 0;

/* Thread-local storage.
 */
__thread int a_thread_local;

/* Check the results of thread-local storage.
 */
int thread_local_val[ N_THREADS ];
int val_debugger_saw[ N_THREADS ];

/* Routine for each thread to run, does nothing.
 */
void *spin( vp )
    void * vp;
{
    int me = (int) vp;
    int i;
    
#ifdef START_DEBUG
    printf( "== In thread %d\n", me );
#endif

    a_global++;

    a_thread_local = 0;
    for( i = 0; i < a_global; i++ ) {
        a_thread_local += i;
    }

    thread_local_val[ me ] = a_thread_local; /* Line 67 */

    printf( "== Thread %d, a_thread_local is %d\n",
            (int) vp, a_thread_local );

   if( wait_here ) {
       /* Extend life of thread to extend life of thread-local var.
        * This makes life easier for human debugging (though you'd
        * probably want to make the delay longer).
        */
       sleep( 5 );
   }
}

void
do_pass( pass )
    int pass;
{
    int i;
    pthread_t t[ N_THREADS ];
    int err;

    for( i = 0; i < N_THREADS; i++) {
        thread_local_val[i] = 0;
        val_debugger_saw[i] = 0;
    }
   
    /* Start N_THREADS threads, then join them so
     * that they are terminated.
     */
    for( i = 0; i < N_THREADS; i++ ) {
        err = pthread_create( &t[i], NULL, spin, (void *)i );
        if( err != 0 ) {
            printf( "== Start/stop, error in thread %d create\n", i );
        }
    }

    for( i = 0; i < N_THREADS; i++ ) {
        err = pthread_join(t[i], NULL );    /* Line 105 */
        if( err != 0 ) {                    /* Line 106 */
            printf( "== Start/stop, error in thread %d join\n", i );
        }
    }

    i = 10;  /* Line 109.  Null line for setting bpts on. */

/*#ifdef START_DEBUG*/
    for( i = 0; i < N_THREADS; i++) {
        printf( "   Local in thread %d was %d, debugger saw %d\n",
                 i, thread_local_val[i], val_debugger_saw[i] );
    }
    printf( "== Pass %d done\n", pass );
/*#endif*/
   
}

void
do_it()
{
    /* We want to start some threads and then
     * end them, and then do it again and again
     */
    int i;
    int dummy;
    
    for( i = 0; i < OUTER_LOOP_COUNT; i++ ) {
        do_pass( i );
        dummy = i;      /* Line 134, null line for setting bps on */
    }
}

main( argc, argv )
int    argc;
char **argv;
{
   wait_here = FALSE;
   if((argc > 1) && (0 != argv )) {
       if( 1 == atoi( argv[1] ) )
           wait_here = TRUE;
    }

#ifdef START_DEBUG 
    printf( "== Test starting\n" );
#endif

    do_it();
    
#ifdef START_DEBUG
    printf( "== Test done\n" );
#endif

    return(0);
}

/* EndSourceFile */
