/* BeginSourceFile quicksort.c

  This file is take from the DDE test system.  It spawns six
  threads to do a sort of an array of random numbers.

  The locations marked "quick N" are used in the test "quicksort.exp".

  The locations marked "att N" are used in the test "attach.exp".

  To compile:

      cc -Ae +DA1.0 -g -o quicksort -lpthread quicksort.c

  To run:
  
     quicksort     --normal run
     quicksort 1   --waits before starting to allow attach
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define SORTSET 100000

/* Uncomment to turn on debugging output */
/* #define QUICK_DEBUG  */

/* Uncomment to turn on wait on each thread create */
/* #define THREAD_WAIT */

/* Fewer than SORT_DIRECT items are sorted with an insertion sort. */
#define SORT_DIRECT 20

/* Work at this depth or less generates a separate work item. */
#define DEFER_DEPTH 6

/* Workpile controller */
typedef void (*work_proc_t)(void *);

typedef struct workpile_struct {
	pthread_mutex_t lock;	       /* mutex for this structure */
	pthread_cond_t work_wait;      /* workers waiting for work */
	pthread_cond_t finish_wait;    /* to wait for workers to finish */
	int max_pile;		       /* length of workpile array */
	work_proc_t worker_proc;       /* work procedure */
	int n_working;		       /* number of workers working */
	int n_waiting;		       /* number of workers waiting for work */
	int n_pile;		       /* number of pointers in the workpile */
	int inp;		       /* FIFO input pointer */
	int outp;		       /* FIFO output pointer */
	void *pile[1];		       /* array of pointers - the workpile */
} *workpile_t;

typedef struct {
	float *data;		   /* Array to sort */
	int n;			   /* Number of elements in the array */
	int depth;		   /* Depth of recursion */
	workpile_t wp;		   /* Workpile to use */
} quick_sort_args;

/* True if waiting for attach.
 */
int wait_here = FALSE;

static workpile_t quick_sort_workpile = NULL;

void *worker(void * wptr);

/* Allocates and initializes a workpile that holds max_pile entries.
 * worker_proc is called to process each work item on the queue.
 */
workpile_t
work_init(int max_pile, work_proc_t worker_proc, int n_threads)
{
	int err;
	pthread_t t;
	workpile_t wp = (workpile_t)
		malloc(sizeof (struct workpile_struct) +
			(max_pile * sizeof (void *)));

	if (wp != NULL) {
		pthread_mutex_init(&wp->lock, NULL);
		pthread_cond_init(&wp->work_wait, NULL);
		pthread_cond_init(&wp->finish_wait, NULL);
		wp->max_pile = max_pile;
		wp->worker_proc = worker_proc;
		wp->n_working = wp->n_waiting = wp->n_pile = 0;
		wp->inp = wp->outp = 0;
		while (n_threads--) {
			err = pthread_create(&t, NULL,
			      worker, (void *)&wp);
#ifdef QUICK_DEBUG
                        printf( "== Quicksort: created new thread\n" );
#ifdef THREAD_WAIT
                        if( n_threads > 0 ) {
                            int i;
                        printf( "== Quicksort: waiting on user input of an integer\n" );
                            scanf( "%d", &i );
                            printf( "== Quicksort: continuing with quicksort\n" );
                        }
#endif
#endif

			assert(err == 0);    /* quick 1 */
		}
                /* All the threads have now been created.
                 */
                assert( n_threads == -1 );         /* att 1 */
                if( wait_here ) {
#ifdef QUICK_DEBUG
                    printf( "== Quicksort: waiting for attach\n" );
#endif
                    sleep( 25 );
                }
                wait_here = 99;              /* att 2, otherwise useless */
	}
	return (wp);                         /* quick 2 */
}

/*
 * Worker thread routine. Continuously looks for work, calls the
 * worker_proc associated with the workpile to do work.
 */
void *
worker(void * wptr)
{
        workpile_t wp;
	void *ptr;

        wp = * (workpile_t *) wptr;

	pthread_mutex_lock(&wp->lock);
	wp->n_working++;
	for (;;) {
		while (wp->n_pile == 0) {								/* wait for new work */
			if (--wp->n_working == 0)
				pthread_cond_signal(&wp->finish_wait);
			wp->n_waiting++;
			pthread_cond_wait(&wp->work_wait, &wp->lock);
			wp->n_waiting--;       /* quick 3 */
			wp->n_working++;
		}
		wp->n_pile--;
		ptr = wp->pile[wp->outp];
		wp->outp = (wp->outp + 1) % wp->max_pile;
		pthread_mutex_unlock(&wp->lock);
		/* Call application worker routine. */
		(*wp->worker_proc)(ptr);
		pthread_mutex_lock(&wp->lock);  /* quick 4 */
	}
	/* NOTREACHED */
}

/* Puts ptr in workpile. Called at the outset, or within a worker. */
void
work_put(workpile_t wp, void *ptr)
{
	pthread_mutex_lock(&wp->lock);
	if (wp->n_waiting) {
		/* idle workers to be awakened */
		pthread_cond_signal(&wp->work_wait);
	}
	assert(wp->n_pile != wp->max_pile);									/* check for room */
	wp->n_pile++;
	wp->pile[wp->inp] = ptr;
	wp->inp = (wp->inp + 1) % wp->max_pile;
	pthread_mutex_unlock(&wp->lock);
}


/* Wait until all work is done and workers quiesce. */
void
work_wait(workpile_t wp)
{
	pthread_mutex_lock(&wp->lock);
	while(wp->n_pile !=0 || wp->n_working != 0)
		pthread_cond_wait(&wp->finish_wait, &wp->lock);
	pthread_mutex_unlock(&wp->lock);
}

void
quick_sort_aux(float *data, int n, int depth, workpile_t wp, int deferrable)
{
	int i,j;

	/* If array small, use insertion sort */
	if (n <= SORT_DIRECT) {
		for (j = 1; j < n; j++) {
			/* data[0..j-1] in sort; find a spot for data[j] */
			float key = data[j];
			for (i = j - 1; i >= 0 && key < data[i]; i--)
				data[i+1] = data[i];
			data[i+1] = key;
		}
		return;
	}
	/* Defer this work to work queue if policy says so */
	if (deferrable && depth <= DEFER_DEPTH) {
		quick_sort_args *q = (quick_sort_args *)
								malloc(sizeof (quick_sort_args));
		assert(q != NULL);
		q->data = data; q->n = n; q->depth = depth; q->wp = wp;
		work_put(wp, (void *)q);
		return;
	}
	/* Otherwise, partition data based on a median estimate */
#define swap(i,j) {float t = data[i]; data[i] = data[j]; data[j] = t;}
	i = 0;
	j = n - 1;
	for (;;) {
		while (data[i] < data[j]) j--;
		if (i >= j) break;
		swap(i, j); i++;
		while (data[i] < data[j]) i++;
		if (i >= j) { i = j; break; }
		swap(i, j); j--;
	}
	/* Median value is now at data[i] */
	/* Partitioned so that data[0..i-1] <= median <= data[i+1..n-1] */
	quick_sort_aux(data,       i,     depth+1, wp, TRUE);
	quick_sort_aux(&data[i+1], n-i-1, depth+1, wp, TRUE);
}
/* Called from workpile controller with argument pointing to work. */
void
quick_sort_worker(void *a)
{
	quick_sort_args *q = (quick_sort_args *)a;
	quick_sort_aux(q->data, q->n, q->depth, q->wp, FALSE);
	free(q);
}
/* Main routine, called by client to do a sort. */
void
quick_sort(float *data, int n)
{
	if (quick_sort_workpile == NULL) {
		int n_threads = 6;
		quick_sort_workpile = work_init(2 << DEFER_DEPTH,
			quick_sort_worker, n_threads);
		assert(quick_sort_workpile != NULL);
	}
        
	quick_sort_aux(data, n, 0, quick_sort_workpile, FALSE);
        
	/* Wait for all work to finish */
	work_wait(quick_sort_workpile);
        
#ifdef QUICK_DEBUG
        printf( "== Quicksort: done sorting\n" );
#endif
}


main( argc, argv )
int    argc;
char **argv;
{
	float data[SORTSET];
	int i; int debugging = 0;

        if((argc > 1) && (0 != argv )) {
            if( 1 == atoi( argv[1] ) )
               wait_here = TRUE;
        }

	for(i = 0; i < SORTSET; i++)
		data[SORTSET -1 -i] = drand48();
                
	for(i = 0; i < SORTSET; i++)
		if (debugging)
                   printf("data[%d] = %f\n", i, data[i]);

	quick_sort(data, SORTSET);
	for(i = 0; i < SORTSET; i++)
		if (debugging)
		printf("data[%d] = %f\n", i, data[i]);

	return(0);
}
/* EndSourceFile */
