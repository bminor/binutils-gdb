#include <stdio.h>

/* Library code for thread local in lib test.
*/
#include "thr-lib.h"

extern pthread_mutex_t mutex;
extern int get_number();
extern int total;
extern int the_code_saw[NTHREADS][ELEMS_PER_THREAD];

/* The debugger should see this without a declaration.
 *
 * extern int debugger_saw[NTHREADS][ELEMS_PER_THREAD]; 
 */

/* The actual thread locals.
 */
__thread int sum;
__thread int x[ ELEMS_PER_THREAD ]; /* [3] */

void sumup()
{
   int j;

   sum = 0;
   for (j = 0; j < ELEMS_PER_THREAD; j++) {
      sum += x[j];
   }

   if( sum == x[0] )
       /* It won't be "==", but this lets us set a breakpoint
        * and look at the thread-local storage.
        */
       sum++;

   x[0] = x[2];  /* Another no-op for debugger use */
}

void *adder( vid )
   void * vid;
{
   int id;
   int i, j;
   int ret;

   id = (int) vid;
   
   /* printf( "== Thread: Welcome to adder %d\n", id ); */

   for (j = 0; j < ELEMS_PER_THREAD; j++) {
      x[j] = 0;
   }

   for (i = id, j = 0; i < NUM_ELEMS; i += NTHREADS, j++ ) {

   /*   printf( "== Thread: id %d, i %d, j %d\n", id, i, j );
      fflush( stdout ); */
    
      x[j] = get_number(i); /* {0,1,2,3} +0, +4, +8 */

      /* Record for posterity; the debugger will gather
       * the same data here, using "x[j]".
       */
      the_code_saw[ id ][ j ] = x[j];   

   /*   printf( "== Thread %d, sample %d, val %d, i %d\n", id, j, x[j],i );
      fflush( stdout ); */
   }

   sumup();
   /* printf("== Thread: adder %d contributes total %d\n", id, sum); */

   /* protect global data */
   ret = pthread_mutex_lock(&mutex);
   if (ret != 0) {
      printf("== Thread: pthread_mutex_lock() error: %d\n", ret);
      exit(1);
   }

   total += sum;

   ret = pthread_mutex_unlock(&mutex);
   if (ret != 0) {
      printf("== Thread: pthread_mutex_unlock() error: %d\n", ret);
      exit(1);
   }

   if( NTHREADS != 4 || ELEMS_PER_THREAD != 3 || NUM_ELEMS != 12 ) {
      printf( "** ERROR in test code **\n" );
   }
}
