/* OBSOLETE #include <stdio.h> */
/* OBSOLETE  */
/* OBSOLETE /* #include "config.h" */ */
/* OBSOLETE #define HAVE_PTHREAD_H */
/* OBSOLETE #define __hpux__ */
/* OBSOLETE #define __osf__ */
/* OBSOLETE  */
/* OBSOLETE #ifndef HAVE_PTHREAD_H */
/* OBSOLETE  */
/* OBSOLETE /* Don't even try to compile.  In fact, cause a syntax error that we can */
/* OBSOLETE    look for as a compiler error message and know that we have no pthread */
/* OBSOLETE    support.  In that case we can just suppress the test completely. */ */
/* OBSOLETE  */
/* OBSOLETE #error "no posix threads support" */
/* OBSOLETE  */
/* OBSOLETE #else */
/* OBSOLETE  */
/* OBSOLETE /* OK.  We have the right header.  If we try to compile this and fail, then */
/* OBSOLETE    there is something wrong and the user should know about it so the testsuite */
/* OBSOLETE    should issue an ERROR result.. */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef __linux__ */
/* OBSOLETE #define  _MIT_POSIX_THREADS 1	/* Linux (or at least RedHat 4.0) needs this */ */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #include <pthread.h> */
/* OBSOLETE  */
/* OBSOLETE /* Under OSF 2.0 & 3.0 and HPUX 10, the second arg of pthread_create */
/* OBSOLETE    is prototyped to be just a "pthread_attr_t", while under Solaris it */
/* OBSOLETE    is a "pthread_attr_t *".  Arg! */ */
/* OBSOLETE  */
/* OBSOLETE #if defined (__osf__) || defined (__hpux__) */
/* OBSOLETE #define PTHREAD_CREATE_ARG2(arg) arg */
/* OBSOLETE #define PTHREAD_CREATE_NULL_ARG2 null_attr */
/* OBSOLETE static pthread_attr_t null_attr; */
/* OBSOLETE #else */
/* OBSOLETE #define PTHREAD_CREATE_ARG2(arg) &arg */
/* OBSOLETE #define PTHREAD_CREATE_NULL_ARG2 NULL */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE static int verbose = 0; */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE common_routine (arg) */
/* OBSOLETE      int arg; */
/* OBSOLETE { */
/* OBSOLETE   static int from_thread1; */
/* OBSOLETE   static int from_thread2; */
/* OBSOLETE   static int from_main; */
/* OBSOLETE   static int hits; */
/* OBSOLETE   static int full_coverage; */
/* OBSOLETE  */
/* OBSOLETE   if (verbose) printf("common_routine (%d)\n", arg); */
/* OBSOLETE   hits++; */
/* OBSOLETE   switch (arg) */
/* OBSOLETE     { */
/* OBSOLETE     case 0: */
/* OBSOLETE       from_main++; */
/* OBSOLETE       break; */
/* OBSOLETE     case 1: */
/* OBSOLETE       from_thread1++; */
/* OBSOLETE       break; */
/* OBSOLETE     case 2: */
/* OBSOLETE       from_thread2++; */
/* OBSOLETE       break; */
/* OBSOLETE     } */
/* OBSOLETE   if (from_main && from_thread1 && from_thread2) */
/* OBSOLETE     full_coverage = 1; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static void * */
/* OBSOLETE thread1 (void *arg) */
/* OBSOLETE { */
/* OBSOLETE   int i; */
/* OBSOLETE   int z = 0; */
/* OBSOLETE  */
/* OBSOLETE   if (verbose) printf ("thread1 (%0x) ; pid = %d\n", arg, getpid ()); */
/* OBSOLETE   for (i=1; i <= 10000000; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if (verbose) printf("thread1 %d\n", pthread_self ()); */
/* OBSOLETE       z += i; */
/* OBSOLETE       common_routine (1); */
/* OBSOLETE       sleep(1); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static void * */
/* OBSOLETE thread2 (void * arg) */
/* OBSOLETE { */
/* OBSOLETE   int i; */
/* OBSOLETE   int k = 0; */
/* OBSOLETE  */
/* OBSOLETE   if (verbose) printf ("thread2 (%0x) ; pid = %d\n", arg, getpid ()); */
/* OBSOLETE   for (i=1; i <= 10000000; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if (verbose) printf("thread2 %d\n", pthread_self ()); */
/* OBSOLETE       k += i; */
/* OBSOLETE       common_routine (2); */
/* OBSOLETE       sleep(1); */
/* OBSOLETE     } */
/* OBSOLETE   sleep(100); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE foo (a, b, c) */
/* OBSOLETE      int a, b, c; */
/* OBSOLETE { */
/* OBSOLETE   int d, e, f; */
/* OBSOLETE  */
/* OBSOLETE   if (verbose) printf("a=%d\n", a); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE main(argc, argv) */
/* OBSOLETE      int argc; */
/* OBSOLETE      char **argv; */
/* OBSOLETE { */
/* OBSOLETE   pthread_t tid1, tid2; */
/* OBSOLETE   int j; */
/* OBSOLETE   int t = 0; */
/* OBSOLETE   void (*xxx) (); */
/* OBSOLETE   pthread_attr_t attr; */
/* OBSOLETE  */
/* OBSOLETE   if (verbose) printf ("pid = %d\n", getpid()); */
/* OBSOLETE  */
/* OBSOLETE   foo (1, 2, 3); */
/* OBSOLETE  */
/* OBSOLETE #ifndef __osf__ */
/* OBSOLETE   if (pthread_attr_init (&attr)) */
/* OBSOLETE     { */
/* OBSOLETE       perror ("pthread_attr_init 1"); */
/* OBSOLETE       exit (1); */
/* OBSOLETE     } */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #ifdef PTHREAD_SCOPE_SYSTEM */
/* OBSOLETE   if (pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM)) */
/* OBSOLETE     { */
/* OBSOLETE       perror ("pthread_attr_setscope 1"); */
/* OBSOLETE       exit (1); */
/* OBSOLETE     } */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE   if (pthread_create (&tid1, pthread_attr_default, thread1, (void *) 0xfeedface)) */
/* OBSOLETE     { */
/* OBSOLETE       perror ("pthread_create 1"); */
/* OBSOLETE       exit (1); */
/* OBSOLETE     } */
/* OBSOLETE   if (verbose) printf ("Made thread %d\n", tid1); */
/* OBSOLETE   sleep (1); */
/* OBSOLETE  */
/* OBSOLETE   if (pthread_create (&tid2, null_attr, thread2, (void *) 0xdeadbeef)) */
/* OBSOLETE     { */
/* OBSOLETE       perror ("pthread_create 2"); */
/* OBSOLETE       exit (1); */
/* OBSOLETE     } */
/* OBSOLETE   if (verbose) printf("Made thread %d\n", tid2); */
/* OBSOLETE  */
/* OBSOLETE   sleep (1); */
/* OBSOLETE  */
/* OBSOLETE   for (j = 1; j <= 10000000; j++) */
/* OBSOLETE     { */
/* OBSOLETE       if (verbose) printf("top %d\n", pthread_self ()); */
/* OBSOLETE       common_routine (0); */
/* OBSOLETE       sleep(1); */
/* OBSOLETE       t += j; */
/* OBSOLETE       if (j > 3)  */
/* OBSOLETE       { */
/* OBSOLETE 	 int*	int_p; */
/* OBSOLETE 	 int_p = 0; */
/* OBSOLETE 	 *int_p = 1; */
/* OBSOLETE       } */
/* OBSOLETE     } */
/* OBSOLETE    */
/* OBSOLETE   exit(0); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #endif	/* ifndef HAVE_PTHREAD_H */ */
