/* Thread local in a library.
*/
#include "thread-local-in-lib.h"
/*
 *     #define NTHREADS    4
 *     #define NUM_ELEMS  12
 */

extern void* adder( void * );

pthread_mutex_t mutex; /* mutex for protecting global data total */

int numbers[NUM_ELEMS] = {5, 4, 3, 2, 1, 6, 7, 8, 9, 10, 12, 11};
int total = 0;

int debugger_saw[NTHREADS][ELEMS_PER_THREAD]; /* [4][3] */
int the_code_saw[NTHREADS][ELEMS_PER_THREAD];

int get_number(i)
int i;
{
   /* sleep to force context switch to another thread in non-MP system 
    * so that TLS symbols are used by multiple threads concurrently
    * in some way. 
    */
   sleep(1); 
   return numbers[i];
}

main()
{
   pthread_t	thread[NTHREADS];
   void 	*status;
   int 		i, j, ret;

   printf("== Thread: Test started\n");

   for( i = 0; i < NTHREADS; i++ ) {
       for( j = 0; j < ELEMS_PER_THREAD; j++ ) {
           debugger_saw[i][j] = 0;
           the_code_saw[i][j] = 0;
       }
   }
   
   ret = pthread_mutex_init(&mutex, NULL);
   if (ret != 0) {
      printf("== Thread: pthread_mutex_init() error: %d\n", ret);
      exit(1);
   }

   for (i=0; i < NTHREADS; i++) {
      ret = pthread_create( &thread[i],
                            NULL,
                            adder,
                            (void *) i);
      if (ret != 0) {
         printf("== Thread: pthread_create() error: %d\n", ret);
         exit(1);
      }
      printf("== Thread: thread %d created\n", i);
   }
   
   for (i=0; i < NTHREADS; i++) {
      pthread_join( thread[i], &status);
   }

   printf("== Thread: total = %d\n", total); /* Expect "78" */

   for( i = 0; i < NTHREADS; i++ ) {
       for( j = 0; j < ELEMS_PER_THREAD; j++ ) {
           printf( "== Thread: the debugger saw %d, the program saw %d\n",
                   debugger_saw[i][j],
                   the_code_saw[i][j] );
       }
   }

   printf("== Thread: Test ended\n");
   exit(0);
}
