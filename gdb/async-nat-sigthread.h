#ifndef __ASYNC_NAT_SIGTHREAD_H__
#define __ASYNC_NAT_SIGTHREAD_H__

#include "defs.h"
//#include "macosx-nat-mutils.h"
//#include "macosx-nat-threads.h"

#include <pthread.h>

typedef void *(*pthread_fn_t) (void *arg);

void gdb_pthread_kill (pthread_t pthread);
pthread_t gdb_pthread_fork (pthread_fn_t function, void *arg);

#define gdb_thread_fork gdb_pthread_fork
#define gdb_thread_kill gdb_pthread_kill

typedef pthread_t gdb_thread_t;
typedef pthread_fn_t gdb_thread_fn_t;

#include <sys/wait.h>

typedef int WAITSTATUS;

struct gdb_signal_thread_message
{
  int pid;
  WAITSTATUS status;
};

struct gdb_signal_thread_status
{
  gdb_thread_t signal_thread;

  int transmit_fd;
  int receive_fd;

  int inferior_pid;
};

typedef struct gdb_signal_thread_message gdb_signal_thread_message;
typedef struct gdb_signal_thread_status gdb_signal_thread_status;

void gdb_signal_thread_debug_status (FILE *f, WAITSTATUS status);

void gdb_signal_thread_init (gdb_signal_thread_status *s);

void gdb_signal_thread_create (gdb_signal_thread_status *s, int pid);
void gdb_signal_thread_destroy (gdb_signal_thread_status *s);

extern gdb_signal_thread_status *gdb_signal_status;

#endif /* __ASYNC_NAT_SIGTHREAD_H__ */
