#include <stdio.h>
#include <pthread.h>

#define NTHREADS 	 4
#define NUM_ELEMS	12

#define ELEMS_PER_THREAD (NUM_ELEMS/NTHREADS)
