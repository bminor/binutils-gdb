#ifndef __ASYNC_NAT_INFERIOR_H__
#define __ASYNC_NAT_INFERIOR_H__

#include "async-nat-sigthread.h"

#include "defs.h"
#include "event-loop.h"
#include "target.h"

struct gdb_inferior_status
{
  int pid;

  int attached_in_ptrace;
  int stopped_in_ptrace;
  int stopped_in_softexc;

  unsigned int suspend_count;

  gdb_signal_thread_status signal_status;
};
typedef struct gdb_inferior_status gdb_inferior_status;

extern gdb_inferior_status *gdb_status;

struct private_thread_info
{
  int app_thread_port;
};

ptid_t gdb_wait (struct gdb_inferior_status *inferior,
		 struct target_waitstatus *status,
		 gdb_client_data client_data);

void gdb_create_inferior (struct gdb_inferior_status *inferior, int pid);

int gdb_process_events (struct gdb_inferior_status *ns,
			struct target_waitstatus *status,
			int timeout, int service_first_event);

ptid_t gdb_process_pending_event (struct gdb_inferior_status *ns,
				  struct target_waitstatus *status,
				  gdb_client_data client_data);

int gdb_post_pending_event (void);

void (*async_client_callback) (enum inferior_event_type event_type,
                                      void *context);

void *async_client_context;

void async_terminal_inferior (void);

void async_terminal_ours (void);

#endif /* __ASYNC_NAT_INFERIOR_H__ */
