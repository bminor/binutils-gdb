// OBSOLETE /* Interface GDB to Mach 3.0 operating systems.
// OBSOLETE    (Most) Mach 3.0 related routines live in this file.
// OBSOLETE 
// OBSOLETE    Copyright 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
// OBSOLETE    2002 Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Author: Jukka Virtanen <jtv@hut.fi>
// OBSOLETE  *         Computing Centre
// OBSOLETE  *         Helsinki University of Technology
// OBSOLETE  *         Finland
// OBSOLETE  *
// OBSOLETE  * Thanks to my friends who helped with ideas and testing:
// OBSOLETE  *
// OBSOLETE  *      Johannes Helander, Antti Louko, Tero Mononen,
// OBSOLETE  *      jvh@cs.hut.fi      alo@hut.fi   tmo@cs.hut.fi
// OBSOLETE  *
// OBSOLETE  *      Tero Kivinen       and          Eamonn McManus
// OBSOLETE  *      kivinen@cs.hut.fi               emcmanus@gr.osf.org
// OBSOLETE  *      
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #include <stdio.h>
// OBSOLETE 
// OBSOLETE #include <mach.h>
// OBSOLETE #include <servers/netname.h>
// OBSOLETE #include <servers/machid.h>
// OBSOLETE #include <mach/message.h>
// OBSOLETE #include <mach/notify.h>
// OBSOLETE #include <mach_error.h>
// OBSOLETE #include <mach/exception.h>
// OBSOLETE #include <mach/vm_attributes.h>
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "inferior.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE #include "value.h"
// OBSOLETE #include "language.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "gdb_wait.h"
// OBSOLETE #include "gdbcmd.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE #include <servers/machid_lib.h>
// OBSOLETE #else
// OBSOLETE #define	MACH_TYPE_TASK			1
// OBSOLETE #define MACH_TYPE_THREAD		2
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Included only for signal names and NSIG
// OBSOLETE 
// OBSOLETE  * note: There are many problems in signal handling with
// OBSOLETE  *       gdb in Mach 3.0 in general.
// OBSOLETE  */
// OBSOLETE #include <signal.h>
// OBSOLETE #define SIG_UNKNOWN 0		/* Exception that has no matching unix signal */
// OBSOLETE 
// OBSOLETE #include <cthreads.h>
// OBSOLETE 
// OBSOLETE /* This is what a cproc looks like.  This is here partly because
// OBSOLETE    cthread_internals.h is not a header we can just #include, partly with
// OBSOLETE    an eye towards perhaps getting this to work with cross-debugging
// OBSOLETE    someday.  Best solution is if CMU publishes a real interface to this
// OBSOLETE    stuff.  */
// OBSOLETE #define CPROC_NEXT_OFFSET 0
// OBSOLETE #define CPROC_NEXT_SIZE (TARGET_PTR_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_INCARNATION_OFFSET (CPROC_NEXT_OFFSET + CPROC_NEXT_SIZE)
// OBSOLETE #define CPROC_INCARNATION_SIZE (sizeof (cthread_t))
// OBSOLETE #define CPROC_LIST_OFFSET (CPROC_INCARNATION_OFFSET + CPROC_INCARNATION_SIZE)
// OBSOLETE #define CPROC_LIST_SIZE (TARGET_PTR_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_WAIT_OFFSET (CPROC_LIST_OFFSET + CPROC_LIST_SIZE)
// OBSOLETE #define CPROC_WAIT_SIZE (TARGET_PTR_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_REPLY_OFFSET (CPROC_WAIT_OFFSET + CPROC_WAIT_SIZE)
// OBSOLETE #define CPROC_REPLY_SIZE (sizeof (mach_port_t))
// OBSOLETE #define CPROC_CONTEXT_OFFSET (CPROC_REPLY_OFFSET + CPROC_REPLY_SIZE)
// OBSOLETE #define CPROC_CONTEXT_SIZE (TARGET_INT_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_LOCK_OFFSET (CPROC_CONTEXT_OFFSET + CPROC_CONTEXT_SIZE)
// OBSOLETE #define CPROC_LOCK_SIZE (sizeof (spin_lock_t))
// OBSOLETE #define CPROC_STATE_OFFSET (CPROC_LOCK_OFFSET + CPROC_LOCK_SIZE)
// OBSOLETE #define CPROC_STATE_SIZE (TARGET_INT_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_WIRED_OFFSET (CPROC_STATE_OFFSET + CPROC_STATE_SIZE)
// OBSOLETE #define CPROC_WIRED_SIZE (sizeof (mach_port_t))
// OBSOLETE #define CPROC_BUSY_OFFSET (CPROC_WIRED_OFFSET + CPROC_WIRED_SIZE)
// OBSOLETE #define CPROC_BUSY_SIZE (TARGET_INT_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_MSG_OFFSET (CPROC_BUSY_OFFSET + CPROC_BUSY_SIZE)
// OBSOLETE #define CPROC_MSG_SIZE (sizeof (mach_msg_header_t))
// OBSOLETE #define CPROC_BASE_OFFSET (CPROC_MSG_OFFSET + CPROC_MSG_SIZE)
// OBSOLETE #define CPROC_BASE_SIZE (TARGET_INT_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_SIZE_OFFSET (CPROC_BASE_OFFSET + CPROC_BASE_SIZE)
// OBSOLETE #define CPROC_SIZE_SIZE (TARGET_INT_BIT / HOST_CHAR_BIT)
// OBSOLETE #define CPROC_SIZE (CPROC_SIZE_OFFSET + CPROC_SIZE_SIZE)
// OBSOLETE 
// OBSOLETE /* Values for the state field in the cproc.  */
// OBSOLETE #define CPROC_RUNNING	0
// OBSOLETE #define CPROC_SWITCHING 1
// OBSOLETE #define CPROC_BLOCKED	2
// OBSOLETE #define CPROC_CONDWAIT	4
// OBSOLETE 
// OBSOLETE /* For cproc and kernel thread mapping */
// OBSOLETE typedef struct gdb_thread
// OBSOLETE   {
// OBSOLETE     mach_port_t name;
// OBSOLETE     CORE_ADDR sp;
// OBSOLETE     CORE_ADDR pc;
// OBSOLETE     CORE_ADDR fp;
// OBSOLETE     boolean_t in_emulator;
// OBSOLETE     int slotid;
// OBSOLETE 
// OBSOLETE     /* This is for the mthreads list.  It points to the cproc list.
// OBSOLETE        Perhaps the two lists should be merged (or perhaps it was a mistake
// OBSOLETE        to make them both use a struct gdb_thread).  */
// OBSOLETE     struct gdb_thread *cproc;
// OBSOLETE 
// OBSOLETE     /* These are for the cproc list, which is linked through the next field
// OBSOLETE        of the struct gdb_thread.  */
// OBSOLETE     char raw_cproc[CPROC_SIZE];
// OBSOLETE     /* The cthread which is pointed to by the incarnation field from the
// OBSOLETE        cproc.  This points to the copy we've read into GDB.  */
// OBSOLETE     cthread_t cthread;
// OBSOLETE     /* Point back to the mthreads list.  */
// OBSOLETE     int reverse_map;
// OBSOLETE     struct gdb_thread *next;
// OBSOLETE   }
// OBSOLETE  *gdb_thread_t;
// OBSOLETE 
// OBSOLETE /* 
// OBSOLETE  * Actions for Mach exceptions.
// OBSOLETE  *
// OBSOLETE  * sigmap field maps the exception to corresponding Unix signal.
// OBSOLETE  *
// OBSOLETE  * I do not know how to map the exception to unix signal
// OBSOLETE  * if SIG_UNKNOWN is specified.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE struct exception_list
// OBSOLETE   {
// OBSOLETE     char *name;
// OBSOLETE     boolean_t forward;
// OBSOLETE     boolean_t print;
// OBSOLETE     int sigmap;
// OBSOLETE   }
// OBSOLETE exception_map[] =
// OBSOLETE {
// OBSOLETE   {
// OBSOLETE     "not_mach3_exception", FALSE, TRUE, SIG_UNKNOWN
// OBSOLETE   }
// OBSOLETE   ,
// OBSOLETE   {
// OBSOLETE     "EXC_BAD_ACCESS", FALSE, TRUE, SIGSEGV
// OBSOLETE   }
// OBSOLETE   ,
// OBSOLETE   {
// OBSOLETE     "EXC_BAD_INSTRUCTION", FALSE, TRUE, SIGILL
// OBSOLETE   }
// OBSOLETE   ,
// OBSOLETE   {
// OBSOLETE     "EXC_ARITHMETIC", FALSE, TRUE, SIGFPE
// OBSOLETE   }
// OBSOLETE   ,
// OBSOLETE   {
// OBSOLETE     "EXC_EMULATION", FALSE, TRUE, SIGEMT
// OBSOLETE   }
// OBSOLETE   ,				/* ??? */
// OBSOLETE   {
// OBSOLETE     "EXC_SOFTWARE", FALSE, TRUE, SIG_UNKNOWN
// OBSOLETE   }
// OBSOLETE   ,
// OBSOLETE   {
// OBSOLETE     "EXC_BREAKPOINT", FALSE, FALSE, SIGTRAP
// OBSOLETE   }
// OBSOLETE };
// OBSOLETE 
// OBSOLETE /* Mach exception table size */
// OBSOLETE int max_exception = sizeof (exception_map) / sizeof (struct exception_list) - 1;
// OBSOLETE 
// OBSOLETE #define MAX_EXCEPTION max_exception
// OBSOLETE 
// OBSOLETE WAITTYPE wait_status;
// OBSOLETE 
// OBSOLETE /* If you define this, intercepted bsd server calls will be
// OBSOLETE  * dumped while waiting the inferior to EXEC the correct
// OBSOLETE  * program
// OBSOLETE  */
// OBSOLETE /* #define DUMP_SYSCALL         /* debugging interceptor */
// OBSOLETE 
// OBSOLETE /* xx_debug() outputs messages if this is nonzero.
// OBSOLETE  * If > 1, DUMP_SYSCALL will dump message contents.
// OBSOLETE  */
// OBSOLETE int debug_level = 0;
// OBSOLETE 
// OBSOLETE /* "Temporary" debug stuff */
// OBSOLETE void
// OBSOLETE xx_debug (char *fmt, int a, int b, int c)
// OBSOLETE {
// OBSOLETE   if (debug_level)
// OBSOLETE     warning (fmt, a, b, c);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* This is in libmach.a */
// OBSOLETE extern mach_port_t name_server_port;
// OBSOLETE 
// OBSOLETE /* Set in catch_exception_raise */
// OBSOLETE int stop_exception, stop_code, stop_subcode;
// OBSOLETE int stopped_in_exception;
// OBSOLETE 
// OBSOLETE /* Thread that was the active thread when we stopped */
// OBSOLETE thread_t stop_thread = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE char *hostname = "";
// OBSOLETE 
// OBSOLETE /* Set when task is attached or created */
// OBSOLETE boolean_t emulator_present = FALSE;
// OBSOLETE 
// OBSOLETE task_t inferior_task;
// OBSOLETE thread_t current_thread;
// OBSOLETE 
// OBSOLETE /* Exception ports for inferior task */
// OBSOLETE mach_port_t inferior_exception_port = MACH_PORT_NULL;
// OBSOLETE mach_port_t inferior_old_exception_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* task exceptions and notifications */
// OBSOLETE mach_port_t inferior_wait_port_set = MACH_PORT_NULL;
// OBSOLETE mach_port_t our_notify_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* This is "inferior_wait_port_set" when not single stepping, and
// OBSOLETE  *         "singlestepped_thread_port" when we are single stepping.
// OBSOLETE  * 
// OBSOLETE  * This is protected by a cleanup function: discard_single_step()
// OBSOLETE  */
// OBSOLETE mach_port_t currently_waiting_for = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* A port for external messages to gdb.
// OBSOLETE  * External in the meaning that they do not come
// OBSOLETE  * from the inferior_task, but rather from external
// OBSOLETE  * tasks.
// OBSOLETE  *
// OBSOLETE  * As a debugging feature:
// OBSOLETE  * A debugger debugging another debugger can stop the
// OBSOLETE  * inferior debugger by the following command sequence
// OBSOLETE  * (without running external programs)
// OBSOLETE  *
// OBSOLETE  *    (top-gdb) set stop_inferior_gdb ()
// OBSOLETE  *    (top-gdb) continue
// OBSOLETE  */
// OBSOLETE mach_port_t our_message_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* For single stepping */
// OBSOLETE mach_port_t thread_exception_port = MACH_PORT_NULL;
// OBSOLETE mach_port_t thread_saved_exception_port = MACH_PORT_NULL;
// OBSOLETE mach_port_t singlestepped_thread_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* For machid calls */
// OBSOLETE mach_port_t mid_server = MACH_PORT_NULL;
// OBSOLETE mach_port_t mid_auth = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE /* If gdb thinks the inferior task is not suspended, it
// OBSOLETE  * must take suspend/abort the threads when it reads the state.
// OBSOLETE  */
// OBSOLETE int must_suspend_thread = 0;
// OBSOLETE 
// OBSOLETE /* When single stepping, we switch the port that mach_really_wait() listens to.
// OBSOLETE  * This cleanup is a guard to prevent the port set from being left to
// OBSOLETE  * the singlestepped_thread_port when error() is called.
// OBSOLETE  *  This is nonzero only when we are single stepping.
// OBSOLETE  */
// OBSOLETE #define NULL_CLEANUP (struct cleanup *)0
// OBSOLETE struct cleanup *cleanup_step = NULL_CLEANUP;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static struct target_ops m3_ops;
// OBSOLETE 
// OBSOLETE static void m3_kill_inferior ();
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE #define MACH_TYPE_EXCEPTION_PORT	-1
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Chain of ports to remember requested notifications. */
// OBSOLETE 
// OBSOLETE struct port_chain
// OBSOLETE   {
// OBSOLETE     struct port_chain *next;
// OBSOLETE     mach_port_t port;
// OBSOLETE     int type;
// OBSOLETE     int mid;			/* Now only valid with MACH_TYPE_THREAD and */
// OBSOLETE     /*  MACH_TYPE_THREAD */
// OBSOLETE   };
// OBSOLETE typedef struct port_chain *port_chain_t;
// OBSOLETE 
// OBSOLETE /* Room for chain nodes comes from pchain_obstack */
// OBSOLETE struct obstack pchain_obstack;
// OBSOLETE struct obstack *port_chain_obstack = &pchain_obstack;
// OBSOLETE 
// OBSOLETE /* For thread handling */
// OBSOLETE struct obstack Cproc_obstack;
// OBSOLETE struct obstack *cproc_obstack = &Cproc_obstack;
// OBSOLETE 
// OBSOLETE /* the list of notified ports */
// OBSOLETE port_chain_t notify_chain = (port_chain_t) NULL;
// OBSOLETE 
// OBSOLETE port_chain_t
// OBSOLETE port_chain_insert (port_chain_t list, mach_port_t name, int type)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   port_chain_t new;
// OBSOLETE   int mid;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (name))
// OBSOLETE     return list;
// OBSOLETE 
// OBSOLETE   if (type == MACH_TYPE_TASK || type == MACH_TYPE_THREAD)
// OBSOLETE     {
// OBSOLETE       if (!MACH_PORT_VALID (mid_server))
// OBSOLETE 	{
// OBSOLETE 	  warning ("Machid server port invalid, can not map port 0x%x to MID",
// OBSOLETE 		   name);
// OBSOLETE 	  mid = name;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  ret = machid_mach_register (mid_server, mid_auth, name, type, &mid);
// OBSOLETE 
// OBSOLETE 	  if (ret != KERN_SUCCESS)
// OBSOLETE 	    {
// OBSOLETE 	      warning ("Can not map name (0x%x) to MID with machid", name);
// OBSOLETE 	      mid = name;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE 
// OBSOLETE   new = (port_chain_t) obstack_alloc (port_chain_obstack,
// OBSOLETE 				      sizeof (struct port_chain));
// OBSOLETE   new->next = list;
// OBSOLETE   new->port = name;
// OBSOLETE   new->type = type;
// OBSOLETE   new->mid = mid;
// OBSOLETE 
// OBSOLETE   return new;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE port_chain_t
// OBSOLETE port_chain_delete (port_chain_t list, mach_port_t elem)
// OBSOLETE {
// OBSOLETE   if (list)
// OBSOLETE     if (list->port == elem)
// OBSOLETE       list = list->next;
// OBSOLETE     else
// OBSOLETE       while (list->next)
// OBSOLETE 	{
// OBSOLETE 	  if (list->next->port == elem)
// OBSOLETE 	    list->next = list->next->next;	/* GCd with obstack_free() */
// OBSOLETE 	  else
// OBSOLETE 	    list = list->next;
// OBSOLETE 	}
// OBSOLETE   return list;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE port_chain_destroy (struct obstack *ostack)
// OBSOLETE {
// OBSOLETE   obstack_free (ostack, 0);
// OBSOLETE   obstack_init (ostack);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE port_chain_t
// OBSOLETE port_chain_member (port_chain_t list, mach_port_t elem)
// OBSOLETE {
// OBSOLETE   while (list)
// OBSOLETE     {
// OBSOLETE       if (list->port == elem)
// OBSOLETE 	return list;
// OBSOLETE       list = list->next;
// OBSOLETE     }
// OBSOLETE   return (port_chain_t) NULL;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE map_port_name_to_mid (mach_port_t name, int type)
// OBSOLETE {
// OBSOLETE   port_chain_t elem;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (name))
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   elem = port_chain_member (notify_chain, name);
// OBSOLETE 
// OBSOLETE   if (elem && (elem->type == type))
// OBSOLETE     return elem->mid;
// OBSOLETE 
// OBSOLETE   if (elem)
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (mid_server))
// OBSOLETE     {
// OBSOLETE       warning ("Machid server port invalid, can not map port 0x%x to mid",
// OBSOLETE 	       name);
// OBSOLETE       return -1;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       int mid;
// OBSOLETE       kern_return_t ret;
// OBSOLETE 
// OBSOLETE       ret = machid_mach_register (mid_server, mid_auth, name, type, &mid);
// OBSOLETE 
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	{
// OBSOLETE 	  warning ("Can not map name (0x%x) to mid with machid", name);
// OBSOLETE 	  return -1;
// OBSOLETE 	}
// OBSOLETE       return mid;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Guard for currently_waiting_for and singlestepped_thread_port */
// OBSOLETE static void
// OBSOLETE discard_single_step (thread_t thread)
// OBSOLETE {
// OBSOLETE   currently_waiting_for = inferior_wait_port_set;
// OBSOLETE 
// OBSOLETE   cleanup_step = NULL_CLEANUP;
// OBSOLETE   if (MACH_PORT_VALID (thread) && MACH_PORT_VALID (singlestepped_thread_port))
// OBSOLETE     setup_single_step (thread, FALSE);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE setup_single_step (thread_t thread, boolean_t start_step)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (thread))
// OBSOLETE     error ("Invalid thread supplied to setup_single_step");
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       mach_port_t teport;
// OBSOLETE 
// OBSOLETE       /* Get the current thread exception port */
// OBSOLETE       ret = thread_get_exception_port (thread, &teport);
// OBSOLETE       CHK ("Getting thread's exception port", ret);
// OBSOLETE 
// OBSOLETE       if (start_step)
// OBSOLETE 	{
// OBSOLETE 	  if (MACH_PORT_VALID (singlestepped_thread_port))
// OBSOLETE 	    {
// OBSOLETE 	      warning ("Singlestepped_thread_port (0x%x) is still valid?",
// OBSOLETE 		       singlestepped_thread_port);
// OBSOLETE 	      singlestepped_thread_port = MACH_PORT_NULL;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  /* If we are already stepping this thread */
// OBSOLETE 	  if (MACH_PORT_VALID (teport) && teport == thread_exception_port)
// OBSOLETE 	    {
// OBSOLETE 	      ret = mach_port_deallocate (mach_task_self (), teport);
// OBSOLETE 	      CHK ("Could not deallocate thread exception port", ret);
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      ret = thread_set_exception_port (thread, thread_exception_port);
// OBSOLETE 	      CHK ("Setting exception port for thread", ret);
// OBSOLETE #if 0
// OBSOLETE 	      /* Insert thread exception port to wait port set */
// OBSOLETE 	      ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 					   thread_exception_port,
// OBSOLETE 					   inferior_wait_port_set);
// OBSOLETE 	      CHK ("Moving thread exception port to inferior_wait_port_set",
// OBSOLETE 		   ret);
// OBSOLETE #endif
// OBSOLETE 	      thread_saved_exception_port = teport;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  thread_trace (thread, TRUE);
// OBSOLETE 
// OBSOLETE 	  singlestepped_thread_port = thread_exception_port;
// OBSOLETE 	  currently_waiting_for = singlestepped_thread_port;
// OBSOLETE 	  cleanup_step = make_cleanup (discard_single_step, thread);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  if (!MACH_PORT_VALID (teport))
// OBSOLETE 	    error ("Single stepped thread had an invalid exception port?");
// OBSOLETE 
// OBSOLETE 	  if (teport != thread_exception_port)
// OBSOLETE 	    error ("Single stepped thread had an unknown exception port?");
// OBSOLETE 
// OBSOLETE 	  ret = mach_port_deallocate (mach_task_self (), teport);
// OBSOLETE 	  CHK ("Couldn't deallocate thread exception port", ret);
// OBSOLETE #if 0
// OBSOLETE 	  /* Remove thread exception port from wait port set */
// OBSOLETE 	  ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 				       thread_exception_port,
// OBSOLETE 				       MACH_PORT_NULL);
// OBSOLETE 	  CHK ("Removing thread exception port from inferior_wait_port_set",
// OBSOLETE 	       ret);
// OBSOLETE #endif
// OBSOLETE 	  /* Restore thread's old exception port */
// OBSOLETE 	  ret = thread_set_exception_port (thread,
// OBSOLETE 					   thread_saved_exception_port);
// OBSOLETE 	  CHK ("Restoring stepped thread's exception port", ret);
// OBSOLETE 
// OBSOLETE 	  if (MACH_PORT_VALID (thread_saved_exception_port))
// OBSOLETE 	    (void) mach_port_deallocate (mach_task_self (),
// OBSOLETE 					 thread_saved_exception_port);
// OBSOLETE 
// OBSOLETE 	  thread_trace (thread, FALSE);
// OBSOLETE 
// OBSOLETE 	  singlestepped_thread_port = MACH_PORT_NULL;
// OBSOLETE 	  currently_waiting_for = inferior_wait_port_set;
// OBSOLETE 	  if (cleanup_step)
// OBSOLETE 	    discard_cleanups (cleanup_step);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static
// OBSOLETE request_notify (mach_port_t name, mach_msg_id_t variant, int type)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   mach_port_t previous_port_dummy = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (name))
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   if (port_chain_member (notify_chain, name))
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   ret = mach_port_request_notification (mach_task_self (),
// OBSOLETE 					name,
// OBSOLETE 					variant,
// OBSOLETE 					1,
// OBSOLETE 					our_notify_port,
// OBSOLETE 					MACH_MSG_TYPE_MAKE_SEND_ONCE,
// OBSOLETE 					&previous_port_dummy);
// OBSOLETE   CHK ("Serious: request_notify failed", ret);
// OBSOLETE 
// OBSOLETE   (void) mach_port_deallocate (mach_task_self (),
// OBSOLETE 			       previous_port_dummy);
// OBSOLETE 
// OBSOLETE   notify_chain = port_chain_insert (notify_chain, name, type);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE reverse_msg_bits (mach_msg_header_t *msgp, int type)
// OBSOLETE {
// OBSOLETE   int rbits, lbits;
// OBSOLETE   rbits = MACH_MSGH_BITS_REMOTE (msgp->msgh_bits);
// OBSOLETE   lbits = type;
// OBSOLETE   msgp->msgh_bits =
// OBSOLETE     (msgp->msgh_bits & ~MACH_MSGH_BITS_PORTS_MASK) |
// OBSOLETE     MACH_MSGH_BITS (lbits, rbits);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* On the third day He said:
// OBSOLETE 
// OBSOLETE    Let this be global
// OBSOLETE    and then it was global.
// OBSOLETE 
// OBSOLETE    When creating the inferior fork, the
// OBSOLETE    child code in inflow.c sets the name of the
// OBSOLETE    bootstrap_port in its address space to this
// OBSOLETE    variable.
// OBSOLETE 
// OBSOLETE    The name is transferred to our address space
// OBSOLETE    with mach3_read_inferior().
// OBSOLETE 
// OBSOLETE    Thou shalt not do this with
// OBSOLETE    task_get_bootstrap_port() in this task, since
// OBSOLETE    the name in the inferior task is different than
// OBSOLETE    the one we get.
// OBSOLETE 
// OBSOLETE    For blessed are the meek, as they shall inherit
// OBSOLETE    the address space.
// OBSOLETE  */
// OBSOLETE mach_port_t original_server_port_name = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Called from inferior after FORK but before EXEC */
// OBSOLETE static void
// OBSOLETE m3_trace_me (void)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   /* Get the NAME of the bootstrap port in this task
// OBSOLETE      so that GDB can read it */
// OBSOLETE   ret = task_get_bootstrap_port (mach_task_self (),
// OBSOLETE 				 &original_server_port_name);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE   ret = mach_port_deallocate (mach_task_self (),
// OBSOLETE 			      original_server_port_name);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE 
// OBSOLETE   /* Suspend this task to let the parent change my ports.
// OBSOLETE      Resumed by the debugger */
// OBSOLETE   ret = task_suspend (mach_task_self ());
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Intercept system calls to Unix server.
// OBSOLETE  * After EXEC_COUNTER calls to exec(), return.
// OBSOLETE  *
// OBSOLETE  * Pre-assertion:  Child is suspended. (Not verified)
// OBSOLETE  * Post-condition: Child is suspended after EXEC_COUNTER exec() calls.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE intercept_exec_calls (int exec_counter)
// OBSOLETE {
// OBSOLETE   int terminal_initted = 0;
// OBSOLETE 
// OBSOLETE   struct syscall_msg_t
// OBSOLETE     {
// OBSOLETE       mach_msg_header_t header;
// OBSOLETE       mach_msg_type_t type;
// OBSOLETE       char room[2000];		/* Enuff space */
// OBSOLETE     };
// OBSOLETE 
// OBSOLETE   struct syscall_msg_t syscall_in, syscall_out;
// OBSOLETE 
// OBSOLETE   mach_port_t fake_server;
// OBSOLETE   mach_port_t original_server_send;
// OBSOLETE   mach_port_t original_exec_reply;
// OBSOLETE   mach_port_t exec_reply;
// OBSOLETE   mach_port_t exec_reply_send;
// OBSOLETE   mach_msg_type_name_t acquired;
// OBSOLETE   mach_port_t emulator_server_port_name;
// OBSOLETE   struct task_basic_info info;
// OBSOLETE   mach_msg_type_number_t info_count;
// OBSOLETE 
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (exec_counter <= 0)
// OBSOLETE     return;			/* We are already set up in the correct program */
// OBSOLETE 
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    &fake_server);
// OBSOLETE   CHK ("create inferior_fake_server port failed", ret);
// OBSOLETE 
// OBSOLETE   /* Wait for inferior_task to suspend itself */
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       info_count = sizeof (info);
// OBSOLETE       ret = task_info (inferior_task,
// OBSOLETE 		       TASK_BASIC_INFO,
// OBSOLETE 		       (task_info_t) & info,
// OBSOLETE 		       &info_count);
// OBSOLETE       CHK ("Task info", ret);
// OBSOLETE 
// OBSOLETE       if (info.suspend_count)
// OBSOLETE 	break;
// OBSOLETE 
// OBSOLETE       /* Note that the definition of the parameter was undefined
// OBSOLETE        * at the time of this writing, so I just use an `ad hoc' value.
// OBSOLETE        */
// OBSOLETE       (void) swtch_pri (42);	/* Universal Priority Value */
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Read the inferior's bootstrap port name */
// OBSOLETE   if (!mach3_read_inferior (&original_server_port_name,
// OBSOLETE 			    &original_server_port_name,
// OBSOLETE 			    sizeof (original_server_port_name)))
// OBSOLETE     error ("Can't read inferior task bootstrap port name");
// OBSOLETE 
// OBSOLETE   /* @@ BUG: If more than 1 send right GDB will FAIL!!! */
// OBSOLETE   /*      Should get refs, and set them back when restoring */
// OBSOLETE   /* Steal the original bsd server send right from inferior */
// OBSOLETE   ret = mach_port_extract_right (inferior_task,
// OBSOLETE 				 original_server_port_name,
// OBSOLETE 				 MACH_MSG_TYPE_MOVE_SEND,
// OBSOLETE 				 &original_server_send,
// OBSOLETE 				 &acquired);
// OBSOLETE   CHK ("mach_port_extract_right (bsd server send)", ret);
// OBSOLETE 
// OBSOLETE   if (acquired != MACH_MSG_TYPE_PORT_SEND)
// OBSOLETE     error ("Incorrect right extracted, send right to bsd server expected");
// OBSOLETE 
// OBSOLETE   ret = mach_port_insert_right (inferior_task,
// OBSOLETE 				original_server_port_name,
// OBSOLETE 				fake_server,
// OBSOLETE 				MACH_MSG_TYPE_MAKE_SEND);
// OBSOLETE   CHK ("mach_port_insert_right (fake server send)", ret);
// OBSOLETE 
// OBSOLETE   xx_debug ("inferior task bsd server ports set up \nfs %x, ospn %x, oss %x\n",
// OBSOLETE 	    fake_server,
// OBSOLETE 	    original_server_port_name, original_server_send);
// OBSOLETE 
// OBSOLETE   /* A receive right to the reply generated by unix server exec() request */
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    &exec_reply);
// OBSOLETE   CHK ("create intercepted_reply_port port failed", ret);
// OBSOLETE 
// OBSOLETE   /* Pass this send right to Unix server so it replies to us after exec() */
// OBSOLETE   ret = mach_port_extract_right (mach_task_self (),
// OBSOLETE 				 exec_reply,
// OBSOLETE 				 MACH_MSG_TYPE_MAKE_SEND_ONCE,
// OBSOLETE 				 &exec_reply_send,
// OBSOLETE 				 &acquired);
// OBSOLETE   CHK ("mach_port_extract_right (exec_reply)", ret);
// OBSOLETE 
// OBSOLETE   if (acquired != MACH_MSG_TYPE_PORT_SEND_ONCE)
// OBSOLETE     error ("Incorrect right extracted, send once expected for exec reply");
// OBSOLETE 
// OBSOLETE   ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 			       fake_server,
// OBSOLETE 			       inferior_wait_port_set);
// OBSOLETE   CHK ("Moving fake syscall port to inferior_wait_port_set", ret);
// OBSOLETE 
// OBSOLETE   xx_debug ("syscall fake server set up, resuming inferior\n");
// OBSOLETE 
// OBSOLETE   ret = task_resume (inferior_task);
// OBSOLETE   CHK ("task_resume (startup)", ret);
// OBSOLETE 
// OBSOLETE   /* Read requests from the inferior.
// OBSOLETE      Pass directly through everything else except exec() calls.
// OBSOLETE    */
// OBSOLETE   while (exec_counter > 0)
// OBSOLETE     {
// OBSOLETE       ret = mach_msg (&syscall_in.header,	/* header */
// OBSOLETE 		      MACH_RCV_MSG,	/* options */
// OBSOLETE 		      0,	/* send size */
// OBSOLETE 		      sizeof (struct syscall_msg_t),	/* receive size */
// OBSOLETE 		      inferior_wait_port_set,	/* receive_name */
// OBSOLETE 		      MACH_MSG_TIMEOUT_NONE,
// OBSOLETE 		      MACH_PORT_NULL);
// OBSOLETE       CHK ("mach_msg (intercepted sycall)", ret);
// OBSOLETE 
// OBSOLETE #ifdef DUMP_SYSCALL
// OBSOLETE       print_msg (&syscall_in.header);
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE       /* ASSERT : msgh_local_port == fake_server */
// OBSOLETE 
// OBSOLETE       if (notify_server (&syscall_in.header, &syscall_out.header))
// OBSOLETE 	error ("received a notify while intercepting syscalls");
// OBSOLETE 
// OBSOLETE       if (syscall_in.header.msgh_id == MIG_EXEC_SYSCALL_ID)
// OBSOLETE 	{
// OBSOLETE 	  xx_debug ("Received EXEC SYSCALL, counter = %d\n", exec_counter);
// OBSOLETE 	  if (exec_counter == 1)
// OBSOLETE 	    {
// OBSOLETE 	      original_exec_reply = syscall_in.header.msgh_remote_port;
// OBSOLETE 	      syscall_in.header.msgh_remote_port = exec_reply_send;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  if (!terminal_initted)
// OBSOLETE 	    {
// OBSOLETE 	      /* Now that the child has exec'd we know it has already set its
// OBSOLETE 	         process group.  On POSIX systems, tcsetpgrp will fail with
// OBSOLETE 	         EPERM if we try it before the child's setpgid.  */
// OBSOLETE 
// OBSOLETE 	      /* Set up the "saved terminal modes" of the inferior
// OBSOLETE 	         based on what modes we are starting it with.  */
// OBSOLETE 	      target_terminal_init ();
// OBSOLETE 
// OBSOLETE 	      /* Install inferior's terminal modes.  */
// OBSOLETE 	      target_terminal_inferior ();
// OBSOLETE 
// OBSOLETE 	      terminal_initted = 1;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  exec_counter--;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       syscall_in.header.msgh_local_port = syscall_in.header.msgh_remote_port;
// OBSOLETE       syscall_in.header.msgh_remote_port = original_server_send;
// OBSOLETE 
// OBSOLETE       reverse_msg_bits (&syscall_in.header, MACH_MSG_TYPE_COPY_SEND);
// OBSOLETE 
// OBSOLETE       ret = mach_msg_send (&syscall_in.header);
// OBSOLETE       CHK ("Forwarded syscall", ret);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 			       fake_server,
// OBSOLETE 			       MACH_PORT_NULL);
// OBSOLETE   CHK ("Moving fake syscall out of inferior_wait_port_set", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 			       exec_reply,
// OBSOLETE 			       inferior_wait_port_set);
// OBSOLETE   CHK ("Moving exec_reply to inferior_wait_port_set", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_msg (&syscall_in.header,	/* header */
// OBSOLETE 		  MACH_RCV_MSG,	/* options */
// OBSOLETE 		  0,		/* send size */
// OBSOLETE 		  sizeof (struct syscall_msg_t),	/* receive size */
// OBSOLETE 		  inferior_wait_port_set,	/* receive_name */
// OBSOLETE 		  MACH_MSG_TIMEOUT_NONE,
// OBSOLETE 		  MACH_PORT_NULL);
// OBSOLETE   CHK ("mach_msg (exec reply)", ret);
// OBSOLETE 
// OBSOLETE   ret = task_suspend (inferior_task);
// OBSOLETE   CHK ("Suspending inferior after last exec", ret);
// OBSOLETE 
// OBSOLETE   must_suspend_thread = 0;
// OBSOLETE 
// OBSOLETE   xx_debug ("Received exec reply from bsd server, suspended inferior task\n");
// OBSOLETE 
// OBSOLETE #ifdef DUMP_SYSCALL
// OBSOLETE   print_msg (&syscall_in.header);
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE   /* Message should appear as if it came from the unix server */
// OBSOLETE   syscall_in.header.msgh_local_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE   /*  and go to the inferior task original reply port */
// OBSOLETE   syscall_in.header.msgh_remote_port = original_exec_reply;
// OBSOLETE 
// OBSOLETE   reverse_msg_bits (&syscall_in.header, MACH_MSG_TYPE_MOVE_SEND_ONCE);
// OBSOLETE 
// OBSOLETE   ret = mach_msg_send (&syscall_in.header);
// OBSOLETE   CHK ("Forwarding exec reply to inferior", ret);
// OBSOLETE 
// OBSOLETE   /* Garbage collect */
// OBSOLETE   ret = mach_port_deallocate (inferior_task,
// OBSOLETE 			      original_server_port_name);
// OBSOLETE   CHK ("deallocating fake server send right", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_insert_right (inferior_task,
// OBSOLETE 				original_server_port_name,
// OBSOLETE 				original_server_send,
// OBSOLETE 				MACH_MSG_TYPE_MOVE_SEND);
// OBSOLETE   CHK ("Restoring the original bsd server send right", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_destroy (mach_task_self (),
// OBSOLETE 			   fake_server);
// OBSOLETE   fake_server = MACH_PORT_DEAD;
// OBSOLETE   CHK ("mach_port_destroy (fake_server)", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_destroy (mach_task_self (),
// OBSOLETE 			   exec_reply);
// OBSOLETE   exec_reply = MACH_PORT_DEAD;
// OBSOLETE   CHK ("mach_port_destroy (exec_reply)", ret);
// OBSOLETE 
// OBSOLETE   xx_debug ("Done with exec call interception\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE consume_send_rights (thread_array_t thread_list, int thread_count)
// OBSOLETE {
// OBSOLETE   int index;
// OBSOLETE 
// OBSOLETE   if (!thread_count)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   for (index = 0; index < thread_count; index++)
// OBSOLETE     {
// OBSOLETE       /* Since thread kill command kills threads, don't check ret */
// OBSOLETE       (void) mach_port_deallocate (mach_task_self (),
// OBSOLETE 				   thread_list[index]);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* suspend/abort/resume a thread. */
// OBSOLETE setup_thread (mach_port_t thread, int what)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (what)
// OBSOLETE     {
// OBSOLETE       ret = thread_suspend (thread);
// OBSOLETE       CHK ("setup_thread thread_suspend", ret);
// OBSOLETE 
// OBSOLETE       ret = thread_abort (thread);
// OBSOLETE       CHK ("setup_thread thread_abort", ret);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       ret = thread_resume (thread);
// OBSOLETE       CHK ("setup_thread thread_resume", ret);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE map_slot_to_mid (int slot, thread_array_t threads, int thread_count)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int deallocate = 0;
// OBSOLETE   int index;
// OBSOLETE   int mid;
// OBSOLETE 
// OBSOLETE   if (!threads)
// OBSOLETE     {
// OBSOLETE       deallocate++;
// OBSOLETE       ret = task_threads (inferior_task, &threads, &thread_count);
// OBSOLETE       CHK ("Can not select a thread from a dead task", ret);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (slot < 0 || slot >= thread_count)
// OBSOLETE     {
// OBSOLETE       if (deallocate)
// OBSOLETE 	{
// OBSOLETE 	  consume_send_rights (threads, thread_count);
// OBSOLETE 	  (void) vm_deallocate (mach_task_self (), (vm_address_t) threads,
// OBSOLETE 				(thread_count * sizeof (mach_port_t)));
// OBSOLETE 	}
// OBSOLETE       if (slot < 0)
// OBSOLETE 	error ("invalid slot number");
// OBSOLETE       else
// OBSOLETE 	return -(slot + 1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   mid = map_port_name_to_mid (threads[slot], MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE   if (deallocate)
// OBSOLETE     {
// OBSOLETE       consume_send_rights (threads, thread_count);
// OBSOLETE       (void) vm_deallocate (mach_task_self (), (vm_address_t) threads,
// OBSOLETE 			    (thread_count * sizeof (mach_port_t)));
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return mid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE parse_thread_id (char *arg, int thread_count, int slots)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int mid;
// OBSOLETE   int slot;
// OBSOLETE   int index;
// OBSOLETE 
// OBSOLETE   if (arg == 0)
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   while (*arg && (*arg == ' ' || *arg == '\t'))
// OBSOLETE     arg++;
// OBSOLETE 
// OBSOLETE   if (!*arg)
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   /* Currently parse MID and @SLOTNUMBER */
// OBSOLETE   if (*arg != '@')
// OBSOLETE     {
// OBSOLETE       mid = atoi (arg);
// OBSOLETE       if (mid <= 0)
// OBSOLETE 	error ("valid thread mid expected");
// OBSOLETE       return mid;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   arg++;
// OBSOLETE   slot = atoi (arg);
// OBSOLETE 
// OBSOLETE   if (slot < 0)
// OBSOLETE     error ("invalid slot number");
// OBSOLETE 
// OBSOLETE   /* If you want slot numbers to remain slot numbers, set slots.
// OBSOLETE 
// OBSOLETE    * Well, since 0 is reserved, return the ordinal number
// OBSOLETE    * of the thread rather than the slot number. Awk, this
// OBSOLETE    * counts as a kludge.
// OBSOLETE    */
// OBSOLETE   if (slots)
// OBSOLETE     return -(slot + 1);
// OBSOLETE 
// OBSOLETE   if (thread_count && slot >= thread_count)
// OBSOLETE     return -(slot + 1);
// OBSOLETE 
// OBSOLETE   mid = map_slot_to_mid (slot);
// OBSOLETE 
// OBSOLETE   return mid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* THREAD_ID 0 is special; it selects the first kernel
// OBSOLETE  * thread from the list (i.e. SLOTNUMBER 0)
// OBSOLETE  * This is used when starting the program with 'run' or when attaching.
// OBSOLETE  *
// OBSOLETE  * If FLAG is 0 the context is not changed, and the registers, frame, etc
// OBSOLETE  * will continue to describe the old thread.
// OBSOLETE  *
// OBSOLETE  * If FLAG is nonzero, really select the thread.
// OBSOLETE  * If FLAG is 2, the THREAD_ID is a slotnumber instead of a mid.
// OBSOLETE  * 
// OBSOLETE  */
// OBSOLETE kern_return_t
// OBSOLETE select_thread (mach_port_t task, int thread_id, int flag)
// OBSOLETE {
// OBSOLETE   thread_array_t thread_list;
// OBSOLETE   int thread_count;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int index;
// OBSOLETE   thread_t new_thread = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE   if (thread_id < 0)
// OBSOLETE     error ("Can't select cprocs without kernel thread");
// OBSOLETE 
// OBSOLETE   ret = task_threads (task, &thread_list, &thread_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("Can not select a thread from a dead task");
// OBSOLETE       m3_kill_inferior ();
// OBSOLETE       return KERN_FAILURE;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (thread_count == 0)
// OBSOLETE     {
// OBSOLETE       /* The task can not do anything anymore, but it still
// OBSOLETE        * exists as a container for memory and ports.
// OBSOLETE        */
// OBSOLETE       registers_changed ();
// OBSOLETE       warning ("Task %d has no threads",
// OBSOLETE 	       map_port_name_to_mid (task, MACH_TYPE_TASK));
// OBSOLETE       current_thread = MACH_PORT_NULL;
// OBSOLETE       (void) vm_deallocate (mach_task_self (),
// OBSOLETE 			    (vm_address_t) thread_list,
// OBSOLETE 			    (thread_count * sizeof (mach_port_t)));
// OBSOLETE       return KERN_FAILURE;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!thread_id || flag == 2)
// OBSOLETE     {
// OBSOLETE       /* First thread or a slotnumber */
// OBSOLETE       if (!thread_id)
// OBSOLETE 	new_thread = thread_list[0];
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  if (thread_id < thread_count)
// OBSOLETE 	    new_thread = thread_list[thread_id];
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      (void) vm_deallocate (mach_task_self (),
// OBSOLETE 				    (vm_address_t) thread_list,
// OBSOLETE 				    (thread_count * sizeof (mach_port_t)));
// OBSOLETE 	      error ("No such thread slot number : %d", thread_id);
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       for (index = 0; index < thread_count; index++)
// OBSOLETE 	if (thread_id == map_port_name_to_mid (thread_list[index],
// OBSOLETE 					       MACH_TYPE_THREAD))
// OBSOLETE 	  {
// OBSOLETE 	    new_thread = thread_list[index];
// OBSOLETE 	    index = -1;
// OBSOLETE 	    break;
// OBSOLETE 	  }
// OBSOLETE 
// OBSOLETE       if (index != -1)
// OBSOLETE 	error ("No thread with mid %d", thread_id);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Notify when the selected thread dies */
// OBSOLETE   request_notify (new_thread, MACH_NOTIFY_DEAD_NAME, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE   ret = vm_deallocate (mach_task_self (),
// OBSOLETE 		       (vm_address_t) thread_list,
// OBSOLETE 		       (thread_count * sizeof (mach_port_t)));
// OBSOLETE   CHK ("vm_deallocate", ret);
// OBSOLETE 
// OBSOLETE   if (!flag)
// OBSOLETE     current_thread = new_thread;
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE #if 0
// OBSOLETE       if (MACH_PORT_VALID (current_thread))
// OBSOLETE 	{
// OBSOLETE 	  /* Store the gdb's view of the thread we are deselecting
// OBSOLETE 
// OBSOLETE 	   * @@ I think gdb updates registers immediately when they are
// OBSOLETE 	   * changed, so don't do this.
// OBSOLETE 	   */
// OBSOLETE 	  ret = thread_abort (current_thread);
// OBSOLETE 	  CHK ("Could not abort system calls when saving state of old thread",
// OBSOLETE 	       ret);
// OBSOLETE 	  target_prepare_to_store ();
// OBSOLETE 	  target_store_registers (-1);
// OBSOLETE 	}
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE       registers_changed ();
// OBSOLETE 
// OBSOLETE       current_thread = new_thread;
// OBSOLETE 
// OBSOLETE       ret = thread_abort (current_thread);
// OBSOLETE       CHK ("Could not abort system calls when selecting a thread", ret);
// OBSOLETE 
// OBSOLETE       stop_pc = read_pc ();
// OBSOLETE       flush_cached_frames ();
// OBSOLETE 
// OBSOLETE       select_frame (get_current_frame ());
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Switch to use thread named NEW_THREAD.
// OBSOLETE  * Return it's MID
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE switch_to_thread (thread_t new_thread)
// OBSOLETE {
// OBSOLETE   thread_t saved_thread = current_thread;
// OBSOLETE   int mid;
// OBSOLETE 
// OBSOLETE   mid = map_port_name_to_mid (new_thread,
// OBSOLETE 			      MACH_TYPE_THREAD);
// OBSOLETE   if (mid == -1)
// OBSOLETE     warning ("Can't map thread name 0x%x to mid", new_thread);
// OBSOLETE   else if (select_thread (inferior_task, mid, 1) != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       if (current_thread)
// OBSOLETE 	current_thread = saved_thread;
// OBSOLETE       error ("Could not select thread %d", mid);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return mid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Do this in gdb after doing FORK but before STARTUP_INFERIOR.
// OBSOLETE  * Note that the registers are not yet valid in the inferior task.
// OBSOLETE  */
// OBSOLETE static int
// OBSOLETE m3_trace_him (int pid)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   push_target (&m3_ops);
// OBSOLETE 
// OBSOLETE   inferior_task = task_by_pid (pid);
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (inferior_task))
// OBSOLETE     error ("Can not map Unix pid %d to Mach task", pid);
// OBSOLETE 
// OBSOLETE   /* Clean up previous notifications and create new ones */
// OBSOLETE   setup_notify_port (1);
// OBSOLETE 
// OBSOLETE   /* When notification appears, the inferior task has died */
// OBSOLETE   request_notify (inferior_task, MACH_NOTIFY_DEAD_NAME, MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   emulator_present = have_emulator_p (inferior_task);
// OBSOLETE 
// OBSOLETE   /* By default, select the first thread,
// OBSOLETE    * If task has no threads, gives a warning
// OBSOLETE    * Does not fetch registers, since they are not yet valid.
// OBSOLETE    */
// OBSOLETE   select_thread (inferior_task, 0, 0);
// OBSOLETE 
// OBSOLETE   inferior_exception_port = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE   setup_exception_port ();
// OBSOLETE 
// OBSOLETE   xx_debug ("Now the debugged task is created\n");
// OBSOLETE 
// OBSOLETE   /* One trap to exec the shell, one to exec the program being debugged.  */
// OBSOLETE   intercept_exec_calls (2);
// OBSOLETE 
// OBSOLETE   return pid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE setup_exception_port (void)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    &inferior_exception_port);
// OBSOLETE   CHK ("mach_port_allocate", ret);
// OBSOLETE 
// OBSOLETE   /* add send right */
// OBSOLETE   ret = mach_port_insert_right (mach_task_self (),
// OBSOLETE 				inferior_exception_port,
// OBSOLETE 				inferior_exception_port,
// OBSOLETE 				MACH_MSG_TYPE_MAKE_SEND);
// OBSOLETE   CHK ("mach_port_insert_right", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 			       inferior_exception_port,
// OBSOLETE 			       inferior_wait_port_set);
// OBSOLETE   CHK ("mach_port_move_member", ret);
// OBSOLETE 
// OBSOLETE   ret = task_get_special_port (inferior_task,
// OBSOLETE 			       TASK_EXCEPTION_PORT,
// OBSOLETE 			       &inferior_old_exception_port);
// OBSOLETE   CHK ("task_get_special_port(old exc)", ret);
// OBSOLETE 
// OBSOLETE   ret = task_set_special_port (inferior_task,
// OBSOLETE 			       TASK_EXCEPTION_PORT,
// OBSOLETE 			       inferior_exception_port);
// OBSOLETE   CHK ("task_set_special_port", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_deallocate (mach_task_self (),
// OBSOLETE 			      inferior_exception_port);
// OBSOLETE   CHK ("mack_port_deallocate", ret);
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE   /* When notify appears, the inferior_task's exception
// OBSOLETE    * port has been destroyed.
// OBSOLETE    *
// OBSOLETE    * Not used, since the dead_name_notification already
// OBSOLETE    * appears when task dies.
// OBSOLETE    *
// OBSOLETE    */
// OBSOLETE   request_notify (inferior_exception_port,
// OBSOLETE 		  MACH_NOTIFY_NO_SENDERS,
// OBSOLETE 		  MACH_TYPE_EXCEPTION_PORT);
// OBSOLETE #endif
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Nonzero if gdb is waiting for a message */
// OBSOLETE int mach_really_waiting;
// OBSOLETE 
// OBSOLETE /* Wait for the inferior to stop for some reason.
// OBSOLETE    - Loop on notifications until inferior_task dies.
// OBSOLETE    - Loop on exceptions until stopped_in_exception comes true.
// OBSOLETE    (e.g. we receive a single step trace trap)
// OBSOLETE    - a message arrives to gdb's message port
// OBSOLETE 
// OBSOLETE    There is no other way to exit this loop.
// OBSOLETE 
// OBSOLETE    Returns the inferior_ptid for rest of gdb.
// OBSOLETE    Side effects: Set *OURSTATUS.  */
// OBSOLETE ptid_t
// OBSOLETE mach_really_wait (ptid_t ptid, struct target_waitstatus *ourstatus)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int w;
// OBSOLETE 
// OBSOLETE   struct msg
// OBSOLETE     {
// OBSOLETE       mach_msg_header_t header;
// OBSOLETE       mach_msg_type_t foo;
// OBSOLETE       int data[8000];
// OBSOLETE     }
// OBSOLETE   in_msg, out_msg;
// OBSOLETE 
// OBSOLETE   /* Either notify (death), exception or message can stop the inferior */
// OBSOLETE   stopped_in_exception = FALSE;
// OBSOLETE 
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       QUIT;
// OBSOLETE 
// OBSOLETE       stop_exception = stop_code = stop_subcode = -1;
// OBSOLETE       stop_thread = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE       mach_really_waiting = 1;
// OBSOLETE       ret = mach_msg (&in_msg.header,	/* header */
// OBSOLETE 		      MACH_RCV_MSG,	/* options */
// OBSOLETE 		      0,	/* send size */
// OBSOLETE 		      sizeof (struct msg),	/* receive size */
// OBSOLETE 		      currently_waiting_for,	/* receive name */
// OBSOLETE 		      MACH_MSG_TIMEOUT_NONE,
// OBSOLETE 		      MACH_PORT_NULL);
// OBSOLETE       mach_really_waiting = 0;
// OBSOLETE       CHK ("mach_msg (receive)", ret);
// OBSOLETE 
// OBSOLETE       /* Check if we received a notify of the childs' death */
// OBSOLETE       if (notify_server (&in_msg.header, &out_msg.header))
// OBSOLETE 	{
// OBSOLETE 	  /* If inferior_task is null then the inferior has
// OBSOLETE 	     gone away and we want to return to command level.
// OBSOLETE 	     Otherwise it was just an informative message and we
// OBSOLETE 	     need to look to see if there are any more. */
// OBSOLETE 	  if (inferior_task != MACH_PORT_NULL)
// OBSOLETE 	    continue;
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      /* Collect Unix exit status for gdb */
// OBSOLETE 
// OBSOLETE 	      wait3 (&w, WNOHANG, 0);
// OBSOLETE 
// OBSOLETE 	      /* This mess is here to check that the rest of
// OBSOLETE 	       * gdb knows that the inferior died. It also
// OBSOLETE 	       * tries to hack around the fact that Mach 3.0 (mk69)
// OBSOLETE 	       * unix server (ux28) does not always know what
// OBSOLETE 	       * has happened to it's children when mach-magic
// OBSOLETE 	       * is applied on them.
// OBSOLETE 	       */
// OBSOLETE 	      if ((!WIFEXITED (w) && WIFSTOPPED (w)) ||
// OBSOLETE 		  (WIFEXITED (w) && WEXITSTATUS (w) > 0377))
// OBSOLETE 		{
// OBSOLETE 		  WSETEXIT (w, 0);
// OBSOLETE 		  warning ("Using exit value 0 for terminated task");
// OBSOLETE 		}
// OBSOLETE 	      else if (!WIFEXITED (w))
// OBSOLETE 		{
// OBSOLETE 		  int sig = WTERMSIG (w);
// OBSOLETE 
// OBSOLETE 		  /* Signals cause problems. Warn the user. */
// OBSOLETE 		  if (sig != SIGKILL)	/* Bad luck if garbage matches this */
// OBSOLETE 		    warning ("The terminating signal stuff may be nonsense");
// OBSOLETE 		  else if (sig > NSIG)
// OBSOLETE 		    {
// OBSOLETE 		      WSETEXIT (w, 0);
// OBSOLETE 		      warning ("Using exit value 0 for terminated task");
// OBSOLETE 		    }
// OBSOLETE 		}
// OBSOLETE 	      store_waitstatus (ourstatus, w);
// OBSOLETE 	      return inferior_ptid;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Hmm. Check for exception, as it was not a notification.
// OBSOLETE          exc_server() does an upcall to catch_exception_raise()
// OBSOLETE          if this rpc is an exception. Further actions are decided
// OBSOLETE          there.
// OBSOLETE        */
// OBSOLETE       if (!exc_server (&in_msg.header, &out_msg.header))
// OBSOLETE 	{
// OBSOLETE 
// OBSOLETE 	  /* Not an exception, check for message.
// OBSOLETE 
// OBSOLETE 	   * Messages don't come from the inferior, or if they
// OBSOLETE 	   * do they better be asynchronous or it will hang.
// OBSOLETE 	   */
// OBSOLETE 	  if (gdb_message_server (&in_msg.header))
// OBSOLETE 	    continue;
// OBSOLETE 
// OBSOLETE 	  error ("Unrecognized message received in mach_really_wait");
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Send the reply of the exception rpc to the suspended task */
// OBSOLETE       ret = mach_msg_send (&out_msg.header);
// OBSOLETE       CHK ("mach_msg_send (exc reply)", ret);
// OBSOLETE 
// OBSOLETE       if (stopped_in_exception)
// OBSOLETE 	{
// OBSOLETE 	  /* Get unix state. May be changed in mach3_exception_actions() */
// OBSOLETE 	  wait3 (&w, WNOHANG, 0);
// OBSOLETE 
// OBSOLETE 	  mach3_exception_actions (&w, FALSE, "Task");
// OBSOLETE 
// OBSOLETE 	  store_waitstatus (ourstatus, w);
// OBSOLETE 	  return inferior_ptid;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Called by macro DO_QUIT() in utils.c(quit).
// OBSOLETE  * This is called just before calling error() to return to command level
// OBSOLETE  */
// OBSOLETE void
// OBSOLETE mach3_quit (void)
// OBSOLETE {
// OBSOLETE   int mid;
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (mach_really_waiting)
// OBSOLETE     {
// OBSOLETE       ret = task_suspend (inferior_task);
// OBSOLETE 
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	{
// OBSOLETE 	  warning ("Could not suspend task for interrupt: %s",
// OBSOLETE 		   mach_error_string (ret));
// OBSOLETE 	  mach_really_waiting = 0;
// OBSOLETE 	  return;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   must_suspend_thread = 0;
// OBSOLETE   mach_really_waiting = 0;
// OBSOLETE 
// OBSOLETE   mid = map_port_name_to_mid (current_thread, MACH_TYPE_THREAD);
// OBSOLETE   if (mid == -1)
// OBSOLETE     {
// OBSOLETE       warning ("Selecting first existing kernel thread");
// OBSOLETE       mid = 0;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   current_thread = MACH_PORT_NULL;	/* Force setup */
// OBSOLETE   select_thread (inferior_task, mid, 1);
// OBSOLETE 
// OBSOLETE   return;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE /* bogus bogus bogus.  It is NOT OK to quit out of target_wait.  */
// OBSOLETE 
// OBSOLETE /* If ^C is typed when we are waiting for a message
// OBSOLETE  * and your Unix server is able to notice that we 
// OBSOLETE  * should quit now.
// OBSOLETE  *
// OBSOLETE  * Called by REQUEST_QUIT() from utils.c(request_quit)
// OBSOLETE  */
// OBSOLETE void
// OBSOLETE mach3_request_quit (void)
// OBSOLETE {
// OBSOLETE   if (mach_really_waiting)
// OBSOLETE     immediate_quit = 1;
// OBSOLETE }
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Gdb message server.
// OBSOLETE  * Currently implemented is the STOP message, that causes
// OBSOLETE  * gdb to return to the command level like ^C had been typed from terminal.
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE gdb_message_server (mach_msg_header_t *InP)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int mid;
// OBSOLETE 
// OBSOLETE   if (InP->msgh_local_port == our_message_port)
// OBSOLETE     {
// OBSOLETE       /* A message coming to our_message_port. Check validity */
// OBSOLETE       switch (InP->msgh_id)
// OBSOLETE 	{
// OBSOLETE 
// OBSOLETE 	case GDB_MESSAGE_ID_STOP:
// OBSOLETE 	  ret = task_suspend (inferior_task);
// OBSOLETE 	  if (ret != KERN_SUCCESS)
// OBSOLETE 	    warning ("Could not suspend task for stop message: %s",
// OBSOLETE 		     mach_error_string (ret));
// OBSOLETE 
// OBSOLETE 	  /* QUIT in mach_really_wait() loop. */
// OBSOLETE 	  request_quit (0);
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	default:
// OBSOLETE 	  warning ("Invalid message id %d received, ignored.",
// OBSOLETE 		   InP->msgh_id);
// OBSOLETE 	  break;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       return 1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Message not handled by this server */
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* NOTE: This is not an RPC call. It is a simpleroutine.
// OBSOLETE 
// OBSOLETE  * This is not called from this gdb code.
// OBSOLETE  *
// OBSOLETE  * It may be called by another debugger to cause this
// OBSOLETE  * debugger to enter command level:
// OBSOLETE  *
// OBSOLETE  *            (gdb) set stop_inferior_gdb ()
// OBSOLETE  *            (gdb) continue
// OBSOLETE  *
// OBSOLETE  * External program "stop-gdb" implements this also.
// OBSOLETE  */
// OBSOLETE void
// OBSOLETE stop_inferior_gdb (void)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   /* Code generated by mig, with minor cleanups :-)
// OBSOLETE 
// OBSOLETE    * simpleroutine stop_inferior_gdb (our_message_port : mach_port_t);
// OBSOLETE    */
// OBSOLETE 
// OBSOLETE   typedef struct
// OBSOLETE     {
// OBSOLETE       mach_msg_header_t Head;
// OBSOLETE     }
// OBSOLETE   Request;
// OBSOLETE 
// OBSOLETE   Request Mess;
// OBSOLETE 
// OBSOLETE   register Request *InP = &Mess;
// OBSOLETE 
// OBSOLETE   InP->Head.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
// OBSOLETE 
// OBSOLETE   /* msgh_size passed as argument */
// OBSOLETE   InP->Head.msgh_remote_port = our_message_port;
// OBSOLETE   InP->Head.msgh_local_port = MACH_PORT_NULL;
// OBSOLETE   InP->Head.msgh_seqno = 0;
// OBSOLETE   InP->Head.msgh_id = GDB_MESSAGE_ID_STOP;
// OBSOLETE 
// OBSOLETE   ret = mach_msg (&InP->Head,
// OBSOLETE 		  MACH_SEND_MSG | MACH_MSG_OPTION_NONE,
// OBSOLETE 		  sizeof (Request),
// OBSOLETE 		  0,
// OBSOLETE 		  MACH_PORT_NULL,
// OBSOLETE 		  MACH_MSG_TIMEOUT_NONE,
// OBSOLETE 		  MACH_PORT_NULL);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef THREAD_ALLOWED_TO_BREAK
// OBSOLETE /*
// OBSOLETE  * Return 1 if the MID specifies the thread that caused the
// OBSOLETE  * last exception.
// OBSOLETE  *  Since catch_exception_raise() selects the thread causing
// OBSOLETE  * the last exception to current_thread, we just check that
// OBSOLETE  * it is selected and the last exception was a breakpoint.
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE mach_thread_for_breakpoint (int mid)
// OBSOLETE {
// OBSOLETE   int cmid = map_port_name_to_mid (current_thread, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE   if (mid < 0)
// OBSOLETE     {
// OBSOLETE       mid = map_slot_to_mid (-(mid + 1), 0, 0);
// OBSOLETE       if (mid < 0)
// OBSOLETE 	return 0;		/* Don't stop, no such slot */
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!mid || cmid == -1)
// OBSOLETE     return 1;			/* stop */
// OBSOLETE 
// OBSOLETE   return cmid == mid && stop_exception == EXC_BREAKPOINT;
// OBSOLETE }
// OBSOLETE #endif /* THREAD_ALLOWED_TO_BREAK */
// OBSOLETE 
// OBSOLETE #ifdef THREAD_PARSE_ID
// OBSOLETE /*
// OBSOLETE  * Map a thread id string (MID or a @SLOTNUMBER)
// OBSOLETE  * to a thread-id.
// OBSOLETE  *
// OBSOLETE  *   0  matches all threads.
// OBSOLETE  *   Otherwise the meaning is defined only in this file.
// OBSOLETE  *   (mach_thread_for_breakpoint uses it)
// OBSOLETE  *
// OBSOLETE  * @@ This allows non-existent MIDs to be specified.
// OBSOLETE  *    It now also allows non-existent slots to be
// OBSOLETE  *    specified. (Slot numbers stored are negative,
// OBSOLETE  *    and the magnitude is one greater than the actual
// OBSOLETE  *    slot index. (Since 0 is reserved))
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE mach_thread_parse_id (char *arg)
// OBSOLETE {
// OBSOLETE   int mid;
// OBSOLETE   if (arg == 0)
// OBSOLETE     error ("thread id expected");
// OBSOLETE   mid = parse_thread_id (arg, 0, 1);
// OBSOLETE 
// OBSOLETE   return mid;
// OBSOLETE }
// OBSOLETE #endif /* THREAD_PARSE_ID */
// OBSOLETE 
// OBSOLETE #ifdef THREAD_OUTPUT_ID
// OBSOLETE char *
// OBSOLETE mach_thread_output_id (int mid)
// OBSOLETE {
// OBSOLETE   static char foobar[20];
// OBSOLETE 
// OBSOLETE   if (mid > 0)
// OBSOLETE     sprintf (foobar, "mid %d", mid);
// OBSOLETE   else if (mid < 0)
// OBSOLETE     sprintf (foobar, "@%d", -(mid + 1));
// OBSOLETE   else
// OBSOLETE     sprintf (foobar, "*any thread*");
// OBSOLETE 
// OBSOLETE   return foobar;
// OBSOLETE }
// OBSOLETE #endif /* THREAD_OUTPUT_ID */
// OBSOLETE 
// OBSOLETE /* Called with hook PREPARE_TO_PROCEED() from infrun.c.
// OBSOLETE 
// OBSOLETE  * If we have switched threads and stopped at breakpoint return 1 otherwise 0.
// OBSOLETE  *
// OBSOLETE  *  if SELECT_IT is nonzero, reselect the thread that was active when
// OBSOLETE  *  we stopped at a breakpoint.
// OBSOLETE  *
// OBSOLETE  * Note that this implementation is potentially redundant now that
// OBSOLETE  * default_prepare_to_proceed() has been added.  
// OBSOLETE  *
// OBSOLETE  * FIXME This may not support switching threads after Ctrl-C
// OBSOLETE  * correctly. The default implementation does support this.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE mach3_prepare_to_proceed (int select_it)
// OBSOLETE {
// OBSOLETE   if (stop_thread &&
// OBSOLETE       stop_thread != current_thread &&
// OBSOLETE       stop_exception == EXC_BREAKPOINT)
// OBSOLETE     {
// OBSOLETE       int mid;
// OBSOLETE 
// OBSOLETE       if (!select_it)
// OBSOLETE 	return 1;
// OBSOLETE 
// OBSOLETE       mid = switch_to_thread (stop_thread);
// OBSOLETE 
// OBSOLETE       return 1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* this stuff here is an upcall via libmach/excServer.c 
// OBSOLETE    and mach_really_wait which does the actual upcall.
// OBSOLETE 
// OBSOLETE    The code will pass the exception to the inferior if:
// OBSOLETE 
// OBSOLETE    - The task that signaled is not the inferior task
// OBSOLETE    (e.g. when debugging another debugger)
// OBSOLETE 
// OBSOLETE    - The user has explicitely requested to pass on the exceptions.
// OBSOLETE    (e.g to the default unix exception handler, which maps
// OBSOLETE    exceptions to signals, or the user has her own exception handler)
// OBSOLETE 
// OBSOLETE    - If the thread that signaled is being single-stepped and it
// OBSOLETE    has set it's own exception port and the exception is not
// OBSOLETE    EXC_BREAKPOINT. (Maybe this is not desirable?)
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE catch_exception_raise (mach_port_t port, thread_t thread, task_t task,
// OBSOLETE 		       int exception, int code, int subcode)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   boolean_t signal_thread;
// OBSOLETE   int mid = map_port_name_to_mid (thread, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (thread))
// OBSOLETE     {
// OBSOLETE       /* If the exception was sent and thread dies before we
// OBSOLETE          receive it, THREAD will be MACH_PORT_DEAD
// OBSOLETE        */
// OBSOLETE 
// OBSOLETE       current_thread = thread = MACH_PORT_NULL;
// OBSOLETE       error ("Received exception from nonexistent thread");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Check if the task died in transit.
// OBSOLETE    * @@ Isn't the thread also invalid in such case?
// OBSOLETE    */
// OBSOLETE   if (!MACH_PORT_VALID (task))
// OBSOLETE     {
// OBSOLETE       current_thread = thread = MACH_PORT_NULL;
// OBSOLETE       error ("Received exception from nonexistent task");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (exception < 0 || exception > MAX_EXCEPTION)
// OBSOLETE     internal_error (__FILE__, __LINE__,
// OBSOLETE 		    "catch_exception_raise: unknown exception code %d thread %d",
// OBSOLETE 		    exception,
// OBSOLETE 		    mid);
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (inferior_task))
// OBSOLETE     error ("got an exception, but inferior_task is null or dead");
// OBSOLETE 
// OBSOLETE   stop_exception = exception;
// OBSOLETE   stop_code = code;
// OBSOLETE   stop_subcode = subcode;
// OBSOLETE   stop_thread = thread;
// OBSOLETE 
// OBSOLETE   signal_thread = exception != EXC_BREAKPOINT &&
// OBSOLETE     port == singlestepped_thread_port &&
// OBSOLETE     MACH_PORT_VALID (thread_saved_exception_port);
// OBSOLETE 
// OBSOLETE   /* If it was not our inferior or if we want to forward
// OBSOLETE    * the exception to the inferior's handler, do it here
// OBSOLETE    *
// OBSOLETE    * Note: If you have forwarded EXC_BREAKPOINT I trust you know why.
// OBSOLETE    */
// OBSOLETE   if (task != inferior_task ||
// OBSOLETE       signal_thread ||
// OBSOLETE       exception_map[exception].forward)
// OBSOLETE     {
// OBSOLETE       mach_port_t eport = inferior_old_exception_port;
// OBSOLETE 
// OBSOLETE       if (signal_thread)
// OBSOLETE 	{
// OBSOLETE 	  /*
// OBSOLETE 	     GDB now forwards the exeption to thread's original handler,
// OBSOLETE 	     since the user propably knows what he is doing.
// OBSOLETE 	     Give a message, though.
// OBSOLETE 	   */
// OBSOLETE 
// OBSOLETE 	  mach3_exception_actions ((WAITTYPE *) NULL, TRUE, "Thread");
// OBSOLETE 	  eport = thread_saved_exception_port;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Send the exception to the original handler */
// OBSOLETE       ret = exception_raise (eport,
// OBSOLETE 			     thread,
// OBSOLETE 			     task,
// OBSOLETE 			     exception,
// OBSOLETE 			     code,
// OBSOLETE 			     subcode);
// OBSOLETE 
// OBSOLETE       (void) mach_port_deallocate (mach_task_self (), task);
// OBSOLETE       (void) mach_port_deallocate (mach_task_self (), thread);
// OBSOLETE 
// OBSOLETE       /* If we come here, we don't want to trace any more, since we
// OBSOLETE        * will never stop for tracing anyway.
// OBSOLETE        */
// OBSOLETE       discard_single_step (thread);
// OBSOLETE 
// OBSOLETE       /* Do not stop the inferior */
// OBSOLETE       return ret;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Now gdb handles the exception */
// OBSOLETE   stopped_in_exception = TRUE;
// OBSOLETE 
// OBSOLETE   ret = task_suspend (task);
// OBSOLETE   CHK ("Error suspending inferior after exception", ret);
// OBSOLETE 
// OBSOLETE   must_suspend_thread = 0;
// OBSOLETE 
// OBSOLETE   if (current_thread != thread)
// OBSOLETE     {
// OBSOLETE       if (MACH_PORT_VALID (singlestepped_thread_port))
// OBSOLETE 	/* Cleanup discards single stepping */
// OBSOLETE 	error ("Exception from thread %d while singlestepping thread %d",
// OBSOLETE 	       mid,
// OBSOLETE 	       map_port_name_to_mid (current_thread, MACH_TYPE_THREAD));
// OBSOLETE 
// OBSOLETE       /* Then select the thread that caused the exception */
// OBSOLETE       if (select_thread (inferior_task, mid, 0) != KERN_SUCCESS)
// OBSOLETE 	error ("Could not select thread %d causing exception", mid);
// OBSOLETE       else
// OBSOLETE 	warning ("Gdb selected thread %d", mid);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* If we receive an exception that is not breakpoint
// OBSOLETE    * exception, we interrupt the single step and return to
// OBSOLETE    * debugger. Trace condition is cleared.
// OBSOLETE    */
// OBSOLETE   if (MACH_PORT_VALID (singlestepped_thread_port))
// OBSOLETE     {
// OBSOLETE       if (stop_exception != EXC_BREAKPOINT)
// OBSOLETE 	warning ("Single step interrupted by exception");
// OBSOLETE       else if (port == singlestepped_thread_port)
// OBSOLETE 	{
// OBSOLETE 	  /* Single step exception occurred, remove trace bit
// OBSOLETE 	   * and return to gdb.
// OBSOLETE 	   */
// OBSOLETE 	  if (!MACH_PORT_VALID (current_thread))
// OBSOLETE 	    error ("Single stepped thread is not valid");
// OBSOLETE 
// OBSOLETE 	  /* Resume threads, but leave the task suspended */
// OBSOLETE 	  resume_all_threads (0);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	warning ("Breakpoint while single stepping?");
// OBSOLETE 
// OBSOLETE       discard_single_step (current_thread);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   (void) mach_port_deallocate (mach_task_self (), task);
// OBSOLETE   (void) mach_port_deallocate (mach_task_self (), thread);
// OBSOLETE 
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE port_valid (mach_port_t port, int mask)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   mach_port_type_t type;
// OBSOLETE 
// OBSOLETE   ret = mach_port_type (mach_task_self (),
// OBSOLETE 			port,
// OBSOLETE 			&type);
// OBSOLETE   if (ret != KERN_SUCCESS || (type & mask) != mask)
// OBSOLETE     return 0;
// OBSOLETE   return 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* @@ No vm read cache implemented yet */
// OBSOLETE boolean_t vm_read_cache_valid = FALSE;
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Read inferior task's LEN bytes from ADDR and copy it to MYADDR
// OBSOLETE  * in gdb's address space.
// OBSOLETE  *
// OBSOLETE  * Return 0 on failure; number of bytes read otherwise.
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE mach3_read_inferior (CORE_ADDR addr, char *myaddr, int length)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   vm_address_t low_address = (vm_address_t) trunc_page (addr);
// OBSOLETE   vm_size_t aligned_length =
// OBSOLETE   (vm_size_t) round_page (addr + length) - low_address;
// OBSOLETE   pointer_t copied_memory;
// OBSOLETE   int copy_count;
// OBSOLETE 
// OBSOLETE   /* Get memory from inferior with page aligned addresses */
// OBSOLETE   ret = vm_read (inferior_task,
// OBSOLETE 		 low_address,
// OBSOLETE 		 aligned_length,
// OBSOLETE 		 &copied_memory,
// OBSOLETE 		 &copy_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       /* the problem is that the inferior might be killed for whatever reason
// OBSOLETE        * before we go to mach_really_wait. This is one place that ought to
// OBSOLETE        * catch many of those errors.
// OBSOLETE        * @@ A better fix would be to make all external events to GDB
// OBSOLETE        * to arrive via a SINGLE port set. (Including user input!)
// OBSOLETE        */
// OBSOLETE 
// OBSOLETE       if (!port_valid (inferior_task, MACH_PORT_TYPE_SEND))
// OBSOLETE 	{
// OBSOLETE 	  m3_kill_inferior ();
// OBSOLETE 	  error ("Inferior killed (task port invalid)");
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE #ifdef OSF
// OBSOLETE 	  extern int errno;
// OBSOLETE 	  /* valprint.c gives nicer format if this does not
// OBSOLETE 	     screw it. Eamonn seems to like this, so I enable
// OBSOLETE 	     it if OSF is defined...
// OBSOLETE 	   */
// OBSOLETE 	  warning ("[read inferior %x failed: %s]",
// OBSOLETE 		   addr, mach_error_string (ret));
// OBSOLETE 	  errno = 0;
// OBSOLETE #endif
// OBSOLETE 	  return 0;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   memcpy (myaddr, (char *) addr - low_address + copied_memory, length);
// OBSOLETE 
// OBSOLETE   ret = vm_deallocate (mach_task_self (),
// OBSOLETE 		       copied_memory,
// OBSOLETE 		       copy_count);
// OBSOLETE   CHK ("mach3_read_inferior vm_deallocate failed", ret);
// OBSOLETE 
// OBSOLETE   return length;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #define CHK_GOTO_OUT(str,ret) \
// OBSOLETE   do if (ret != KERN_SUCCESS) { errstr = #str; goto out; } while(0)
// OBSOLETE 
// OBSOLETE struct vm_region_list
// OBSOLETE {
// OBSOLETE   struct vm_region_list *next;
// OBSOLETE   vm_prot_t protection;
// OBSOLETE   vm_address_t start;
// OBSOLETE   vm_size_t length;
// OBSOLETE };
// OBSOLETE 
// OBSOLETE struct obstack region_obstack;
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Write inferior task's LEN bytes from ADDR and copy it to MYADDR
// OBSOLETE  * in gdb's address space.
// OBSOLETE  */
// OBSOLETE int
// OBSOLETE mach3_write_inferior (CORE_ADDR addr, char *myaddr, int length)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   vm_address_t low_address = (vm_address_t) trunc_page (addr);
// OBSOLETE   vm_size_t aligned_length =
// OBSOLETE   (vm_size_t) round_page (addr + length) - low_address;
// OBSOLETE   pointer_t copied_memory;
// OBSOLETE   int copy_count;
// OBSOLETE   int deallocate = 0;
// OBSOLETE 
// OBSOLETE   char *errstr = "Bug in mach3_write_inferior";
// OBSOLETE 
// OBSOLETE   struct vm_region_list *region_element;
// OBSOLETE   struct vm_region_list *region_head = (struct vm_region_list *) NULL;
// OBSOLETE 
// OBSOLETE   /* Get memory from inferior with page aligned addresses */
// OBSOLETE   ret = vm_read (inferior_task,
// OBSOLETE 		 low_address,
// OBSOLETE 		 aligned_length,
// OBSOLETE 		 &copied_memory,
// OBSOLETE 		 &copy_count);
// OBSOLETE   CHK_GOTO_OUT ("mach3_write_inferior vm_read failed", ret);
// OBSOLETE 
// OBSOLETE   deallocate++;
// OBSOLETE 
// OBSOLETE   memcpy ((char *) addr - low_address + copied_memory, myaddr, length);
// OBSOLETE 
// OBSOLETE   obstack_init (&region_obstack);
// OBSOLETE 
// OBSOLETE   /* Do writes atomically.
// OBSOLETE    * First check for holes and unwritable memory.
// OBSOLETE    */
// OBSOLETE   {
// OBSOLETE     vm_size_t remaining_length = aligned_length;
// OBSOLETE     vm_address_t region_address = low_address;
// OBSOLETE 
// OBSOLETE     struct vm_region_list *scan;
// OBSOLETE 
// OBSOLETE     while (region_address < low_address + aligned_length)
// OBSOLETE       {
// OBSOLETE 	vm_prot_t protection;
// OBSOLETE 	vm_prot_t max_protection;
// OBSOLETE 	vm_inherit_t inheritance;
// OBSOLETE 	boolean_t shared;
// OBSOLETE 	mach_port_t object_name;
// OBSOLETE 	vm_offset_t offset;
// OBSOLETE 	vm_size_t region_length = remaining_length;
// OBSOLETE 	vm_address_t old_address = region_address;
// OBSOLETE 
// OBSOLETE 	ret = vm_region (inferior_task,
// OBSOLETE 			 &region_address,
// OBSOLETE 			 &region_length,
// OBSOLETE 			 &protection,
// OBSOLETE 			 &max_protection,
// OBSOLETE 			 &inheritance,
// OBSOLETE 			 &shared,
// OBSOLETE 			 &object_name,
// OBSOLETE 			 &offset);
// OBSOLETE 	CHK_GOTO_OUT ("vm_region failed", ret);
// OBSOLETE 
// OBSOLETE 	/* Check for holes in memory */
// OBSOLETE 	if (old_address != region_address)
// OBSOLETE 	  {
// OBSOLETE 	    warning ("No memory at 0x%x. Nothing written",
// OBSOLETE 		     old_address);
// OBSOLETE 	    ret = KERN_SUCCESS;
// OBSOLETE 	    length = 0;
// OBSOLETE 	    goto out;
// OBSOLETE 	  }
// OBSOLETE 
// OBSOLETE 	if (!(max_protection & VM_PROT_WRITE))
// OBSOLETE 	  {
// OBSOLETE 	    warning ("Memory at address 0x%x is unwritable. Nothing written",
// OBSOLETE 		     old_address);
// OBSOLETE 	    ret = KERN_SUCCESS;
// OBSOLETE 	    length = 0;
// OBSOLETE 	    goto out;
// OBSOLETE 	  }
// OBSOLETE 
// OBSOLETE 	/* Chain the regions for later use */
// OBSOLETE 	region_element =
// OBSOLETE 	  (struct vm_region_list *)
// OBSOLETE 	  obstack_alloc (&region_obstack, sizeof (struct vm_region_list));
// OBSOLETE 
// OBSOLETE 	region_element->protection = protection;
// OBSOLETE 	region_element->start = region_address;
// OBSOLETE 	region_element->length = region_length;
// OBSOLETE 
// OBSOLETE 	/* Chain the regions along with protections */
// OBSOLETE 	region_element->next = region_head;
// OBSOLETE 	region_head = region_element;
// OBSOLETE 
// OBSOLETE 	region_address += region_length;
// OBSOLETE 	remaining_length = remaining_length - region_length;
// OBSOLETE       }
// OBSOLETE 
// OBSOLETE     /* If things fail after this, we give up.
// OBSOLETE      * Somebody is messing up inferior_task's mappings.
// OBSOLETE      */
// OBSOLETE 
// OBSOLETE     /* Enable writes to the chained vm regions */
// OBSOLETE     for (scan = region_head; scan; scan = scan->next)
// OBSOLETE       {
// OBSOLETE 	boolean_t protection_changed = FALSE;
// OBSOLETE 
// OBSOLETE 	if (!(scan->protection & VM_PROT_WRITE))
// OBSOLETE 	  {
// OBSOLETE 	    ret = vm_protect (inferior_task,
// OBSOLETE 			      scan->start,
// OBSOLETE 			      scan->length,
// OBSOLETE 			      FALSE,
// OBSOLETE 			      scan->protection | VM_PROT_WRITE);
// OBSOLETE 	    CHK_GOTO_OUT ("vm_protect: enable write failed", ret);
// OBSOLETE 	  }
// OBSOLETE       }
// OBSOLETE 
// OBSOLETE     ret = vm_write (inferior_task,
// OBSOLETE 		    low_address,
// OBSOLETE 		    copied_memory,
// OBSOLETE 		    aligned_length);
// OBSOLETE     CHK_GOTO_OUT ("vm_write failed", ret);
// OBSOLETE 
// OBSOLETE     /* Set up the original region protections, if they were changed */
// OBSOLETE     for (scan = region_head; scan; scan = scan->next)
// OBSOLETE       {
// OBSOLETE 	boolean_t protection_changed = FALSE;
// OBSOLETE 
// OBSOLETE 	if (!(scan->protection & VM_PROT_WRITE))
// OBSOLETE 	  {
// OBSOLETE 	    ret = vm_protect (inferior_task,
// OBSOLETE 			      scan->start,
// OBSOLETE 			      scan->length,
// OBSOLETE 			      FALSE,
// OBSOLETE 			      scan->protection);
// OBSOLETE 	    CHK_GOTO_OUT ("vm_protect: enable write failed", ret);
// OBSOLETE 	  }
// OBSOLETE       }
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE out:
// OBSOLETE   if (deallocate)
// OBSOLETE     {
// OBSOLETE       obstack_free (&region_obstack, 0);
// OBSOLETE 
// OBSOLETE       (void) vm_deallocate (mach_task_self (),
// OBSOLETE 			    copied_memory,
// OBSOLETE 			    copy_count);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("%s %s", errstr, mach_error_string (ret));
// OBSOLETE       return 0;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return length;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Return 0 on failure, number of bytes handled otherwise.  TARGET is
// OBSOLETE    ignored. */
// OBSOLETE static int
// OBSOLETE m3_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
// OBSOLETE 		struct target_ops *target)
// OBSOLETE {
// OBSOLETE   int result;
// OBSOLETE 
// OBSOLETE   if (write)
// OBSOLETE     result = mach3_write_inferior (memaddr, myaddr, len);
// OBSOLETE   else
// OBSOLETE     result = mach3_read_inferior (memaddr, myaddr, len);
// OBSOLETE 
// OBSOLETE   return result;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE translate_state (int state)
// OBSOLETE {
// OBSOLETE   switch (state)
// OBSOLETE     {
// OBSOLETE     case TH_STATE_RUNNING:
// OBSOLETE       return ("R");
// OBSOLETE     case TH_STATE_STOPPED:
// OBSOLETE       return ("S");
// OBSOLETE     case TH_STATE_WAITING:
// OBSOLETE       return ("W");
// OBSOLETE     case TH_STATE_UNINTERRUPTIBLE:
// OBSOLETE       return ("U");
// OBSOLETE     case TH_STATE_HALTED:
// OBSOLETE       return ("H");
// OBSOLETE     default:
// OBSOLETE       return ("?");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE translate_cstate (int state)
// OBSOLETE {
// OBSOLETE   switch (state)
// OBSOLETE     {
// OBSOLETE     case CPROC_RUNNING:
// OBSOLETE       return "R";
// OBSOLETE     case CPROC_SWITCHING:
// OBSOLETE       return "S";
// OBSOLETE     case CPROC_BLOCKED:
// OBSOLETE       return "B";
// OBSOLETE     case CPROC_CONDWAIT:
// OBSOLETE       return "C";
// OBSOLETE     case CPROC_CONDWAIT | CPROC_SWITCHING:
// OBSOLETE       return "CS";
// OBSOLETE     default:
// OBSOLETE       return "?";
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* type == MACH_MSG_TYPE_COPY_SEND || type == MACH_MSG_TYPE_MAKE_SEND */
// OBSOLETE 
// OBSOLETE mach_port_t			/* no mach_port_name_t found in include files. */
// OBSOLETE map_inferior_port_name (mach_port_t inferior_name, mach_msg_type_name_t type)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   mach_msg_type_name_t acquired;
// OBSOLETE   mach_port_t iport;
// OBSOLETE 
// OBSOLETE   ret = mach_port_extract_right (inferior_task,
// OBSOLETE 				 inferior_name,
// OBSOLETE 				 type,
// OBSOLETE 				 &iport,
// OBSOLETE 				 &acquired);
// OBSOLETE   CHK ("mach_port_extract_right (map_inferior_port_name)", ret);
// OBSOLETE 
// OBSOLETE   if (acquired != MACH_MSG_TYPE_PORT_SEND)
// OBSOLETE     error ("Incorrect right extracted, (map_inferior_port_name)");
// OBSOLETE 
// OBSOLETE   ret = mach_port_deallocate (mach_task_self (),
// OBSOLETE 			      iport);
// OBSOLETE   CHK ("Deallocating mapped port (map_inferior_port_name)", ret);
// OBSOLETE 
// OBSOLETE   return iport;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Naming convention:
// OBSOLETE  *  Always return user defined name if found.
// OBSOLETE  *  _K == A kernel thread with no matching CPROC
// OBSOLETE  *  _C == A cproc with no current cthread
// OBSOLETE  *  _t == A cthread with no user defined name
// OBSOLETE  *
// OBSOLETE  * The digits that follow the _names are the SLOT number of the
// OBSOLETE  * kernel thread if there is such a thing, otherwise just a negation
// OBSOLETE  * of the sequential number of such cprocs.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static char buf[7];
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE get_thread_name (gdb_thread_t one_cproc, int id)
// OBSOLETE {
// OBSOLETE   if (one_cproc)
// OBSOLETE     if (one_cproc->cthread == NULL)
// OBSOLETE       {
// OBSOLETE 	/* cproc not mapped to any cthread */
// OBSOLETE 	sprintf (buf, "_C%d", id);
// OBSOLETE       }
// OBSOLETE     else if (!one_cproc->cthread->name)
// OBSOLETE       {
// OBSOLETE 	/* cproc and cthread, but no name */
// OBSOLETE 	sprintf (buf, "_t%d", id);
// OBSOLETE       }
// OBSOLETE     else
// OBSOLETE       return (char *) (one_cproc->cthread->name);
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       if (id < 0)
// OBSOLETE 	warning ("Inconsistency in thread name id %d", id);
// OBSOLETE 
// OBSOLETE       /* Kernel thread without cproc */
// OBSOLETE       sprintf (buf, "_K%d", id);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return buf;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE fetch_thread_info (mach_port_t task, gdb_thread_t *mthreads_out)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_array_t th_table;
// OBSOLETE   int th_count;
// OBSOLETE   gdb_thread_t mthreads = NULL;
// OBSOLETE   int index;
// OBSOLETE 
// OBSOLETE   ret = task_threads (task, &th_table, &th_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("Error getting inferior's thread list:%s",
// OBSOLETE 	       mach_error_string (ret));
// OBSOLETE       m3_kill_inferior ();
// OBSOLETE       return -1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   mthreads = (gdb_thread_t)
// OBSOLETE     obstack_alloc
// OBSOLETE     (cproc_obstack,
// OBSOLETE      th_count * sizeof (struct gdb_thread));
// OBSOLETE 
// OBSOLETE   for (index = 0; index < th_count; index++)
// OBSOLETE     {
// OBSOLETE       thread_t saved_thread = MACH_PORT_NULL;
// OBSOLETE       int mid;
// OBSOLETE 
// OBSOLETE       if (must_suspend_thread)
// OBSOLETE 	setup_thread (th_table[index], 1);
// OBSOLETE 
// OBSOLETE       if (th_table[index] != current_thread)
// OBSOLETE 	{
// OBSOLETE 	  saved_thread = current_thread;
// OBSOLETE 
// OBSOLETE 	  mid = switch_to_thread (th_table[index]);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       mthreads[index].name = th_table[index];
// OBSOLETE       mthreads[index].cproc = NULL;	/* map_cprocs_to_kernel_threads() */
// OBSOLETE       mthreads[index].in_emulator = FALSE;
// OBSOLETE       mthreads[index].slotid = index;
// OBSOLETE 
// OBSOLETE       mthreads[index].sp = read_register (SP_REGNUM);
// OBSOLETE       mthreads[index].fp = read_register (FP_REGNUM);
// OBSOLETE       mthreads[index].pc = read_pc ();
// OBSOLETE 
// OBSOLETE       if (MACH_PORT_VALID (saved_thread))
// OBSOLETE 	mid = switch_to_thread (saved_thread);
// OBSOLETE 
// OBSOLETE       if (must_suspend_thread)
// OBSOLETE 	setup_thread (th_table[index], 0);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   consume_send_rights (th_table, th_count);
// OBSOLETE   ret = vm_deallocate (mach_task_self (), (vm_address_t) th_table,
// OBSOLETE 		       (th_count * sizeof (mach_port_t)));
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("Error trying to deallocate thread list : %s",
// OBSOLETE 	       mach_error_string (ret));
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   *mthreads_out = mthreads;
// OBSOLETE 
// OBSOLETE   return th_count;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Current emulator always saves the USP on top of
// OBSOLETE  * emulator stack below struct emul_stack_top stuff.
// OBSOLETE  */
// OBSOLETE CORE_ADDR
// OBSOLETE fetch_usp_from_emulator_stack (CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   CORE_ADDR stack_pointer;
// OBSOLETE 
// OBSOLETE   sp = (sp & ~(EMULATOR_STACK_SIZE - 1)) +
// OBSOLETE     EMULATOR_STACK_SIZE - sizeof (struct emul_stack_top);
// OBSOLETE 
// OBSOLETE   if (mach3_read_inferior (sp,
// OBSOLETE 			   &stack_pointer,
// OBSOLETE 			   sizeof (CORE_ADDR)) != sizeof (CORE_ADDR))
// OBSOLETE     {
// OBSOLETE       warning ("Can't read user sp from emulator stack address 0x%x", sp);
// OBSOLETE       return 0;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return stack_pointer;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef MK67
// OBSOLETE 
// OBSOLETE /* get_emulation_vector() interface was changed after mk67 */
// OBSOLETE #define EMUL_VECTOR_COUNT 400	/* Value does not matter too much */
// OBSOLETE 
// OBSOLETE #endif /* MK67 */
// OBSOLETE 
// OBSOLETE /* Check if the emulator exists at task's address space.
// OBSOLETE  */
// OBSOLETE boolean_t
// OBSOLETE have_emulator_p (task_t task)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE #ifndef EMUL_VECTOR_COUNT
// OBSOLETE   vm_offset_t *emulation_vector;
// OBSOLETE   int n;
// OBSOLETE #else
// OBSOLETE   vm_offset_t emulation_vector[EMUL_VECTOR_COUNT];
// OBSOLETE   int n = EMUL_VECTOR_COUNT;
// OBSOLETE #endif
// OBSOLETE   int i;
// OBSOLETE   int vector_start;
// OBSOLETE 
// OBSOLETE   ret = task_get_emulation_vector (task,
// OBSOLETE 				   &vector_start,
// OBSOLETE #ifndef EMUL_VECTOR_COUNT
// OBSOLETE 				   &emulation_vector,
// OBSOLETE #else
// OBSOLETE 				   emulation_vector,
// OBSOLETE #endif
// OBSOLETE 				   &n);
// OBSOLETE   CHK ("task_get_emulation_vector", ret);
// OBSOLETE   xx_debug ("%d vectors from %d at 0x%08x\n",
// OBSOLETE 	    n, vector_start, emulation_vector);
// OBSOLETE 
// OBSOLETE   for (i = 0; i < n; i++)
// OBSOLETE     {
// OBSOLETE       vm_offset_t entry = emulation_vector[i];
// OBSOLETE 
// OBSOLETE       if (EMULATOR_BASE <= entry && entry <= EMULATOR_END)
// OBSOLETE 	return TRUE;
// OBSOLETE       else if (entry)
// OBSOLETE 	{
// OBSOLETE 	  static boolean_t informed = FALSE;
// OBSOLETE 	  if (!informed)
// OBSOLETE 	    {
// OBSOLETE 	      warning ("Emulation vector address 0x08%x outside emulator space",
// OBSOLETE 		       entry);
// OBSOLETE 	      informed = TRUE;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   return FALSE;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Map cprocs to kernel threads and vice versa.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE map_cprocs_to_kernel_threads (gdb_thread_t cprocs, gdb_thread_t mthreads,
// OBSOLETE 			      int thread_count)
// OBSOLETE {
// OBSOLETE   int index;
// OBSOLETE   gdb_thread_t scan;
// OBSOLETE   boolean_t all_mapped = TRUE;
// OBSOLETE   LONGEST stack_base;
// OBSOLETE   LONGEST stack_size;
// OBSOLETE 
// OBSOLETE   for (scan = cprocs; scan; scan = scan->next)
// OBSOLETE     {
// OBSOLETE       /* Default to: no kernel thread for this cproc */
// OBSOLETE       scan->reverse_map = -1;
// OBSOLETE 
// OBSOLETE       /* Check if the cproc is found by its stack */
// OBSOLETE       for (index = 0; index < thread_count; index++)
// OBSOLETE 	{
// OBSOLETE 	  stack_base =
// OBSOLETE 	    extract_signed_integer (scan->raw_cproc + CPROC_BASE_OFFSET,
// OBSOLETE 				    CPROC_BASE_SIZE);
// OBSOLETE 	  stack_size =
// OBSOLETE 	    extract_signed_integer (scan->raw_cproc + CPROC_SIZE_OFFSET,
// OBSOLETE 				    CPROC_SIZE_SIZE);
// OBSOLETE 	  if ((mthreads + index)->sp > stack_base &&
// OBSOLETE 	      (mthreads + index)->sp <= stack_base + stack_size)
// OBSOLETE 	    {
// OBSOLETE 	      (mthreads + index)->cproc = scan;
// OBSOLETE 	      scan->reverse_map = index;
// OBSOLETE 	      break;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       all_mapped &= (scan->reverse_map != -1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Check for threads that are currently in the emulator.
// OBSOLETE    * If so, they have a different stack, and the still unmapped
// OBSOLETE    * cprocs may well get mapped to these threads.
// OBSOLETE    * 
// OBSOLETE    * If:
// OBSOLETE    *  - cproc stack does not match any kernel thread stack pointer
// OBSOLETE    *  - there is at least one extra kernel thread
// OBSOLETE    *    that has no cproc mapped above.
// OBSOLETE    *  - some kernel thread stack pointer points to emulator space
// OBSOLETE    *  then we find the user stack pointer saved in the emulator
// OBSOLETE    *  stack, and try to map that to the cprocs.
// OBSOLETE    *
// OBSOLETE    * Also set in_emulator for kernel threads.
// OBSOLETE    */
// OBSOLETE 
// OBSOLETE   if (emulator_present)
// OBSOLETE     {
// OBSOLETE       for (index = 0; index < thread_count; index++)
// OBSOLETE 	{
// OBSOLETE 	  CORE_ADDR emul_sp;
// OBSOLETE 	  CORE_ADDR usp;
// OBSOLETE 
// OBSOLETE 	  gdb_thread_t mthread = (mthreads + index);
// OBSOLETE 	  emul_sp = mthread->sp;
// OBSOLETE 
// OBSOLETE 	  if (mthread->cproc == NULL &&
// OBSOLETE 	      EMULATOR_BASE <= emul_sp && emul_sp <= EMULATOR_END)
// OBSOLETE 	    {
// OBSOLETE 	      mthread->in_emulator = emulator_present;
// OBSOLETE 
// OBSOLETE 	      if (!all_mapped && cprocs)
// OBSOLETE 		{
// OBSOLETE 		  usp = fetch_usp_from_emulator_stack (emul_sp);
// OBSOLETE 
// OBSOLETE 		  /* @@ Could be more accurate */
// OBSOLETE 		  if (!usp)
// OBSOLETE 		    error ("Zero stack pointer read from emulator?");
// OBSOLETE 
// OBSOLETE 		  /* Try to match this stack pointer to the cprocs that
// OBSOLETE 		   * don't yet have a kernel thread.
// OBSOLETE 		   */
// OBSOLETE 		  for (scan = cprocs; scan; scan = scan->next)
// OBSOLETE 		    {
// OBSOLETE 
// OBSOLETE 		      /* Check is this unmapped CPROC stack contains
// OBSOLETE 		       * the user stack pointer saved in the
// OBSOLETE 		       * emulator.
// OBSOLETE 		       */
// OBSOLETE 		      if (scan->reverse_map == -1)
// OBSOLETE 			{
// OBSOLETE 			  stack_base =
// OBSOLETE 			    extract_signed_integer
// OBSOLETE 			    (scan->raw_cproc + CPROC_BASE_OFFSET,
// OBSOLETE 			     CPROC_BASE_SIZE);
// OBSOLETE 			  stack_size =
// OBSOLETE 			    extract_signed_integer
// OBSOLETE 			    (scan->raw_cproc + CPROC_SIZE_OFFSET,
// OBSOLETE 			     CPROC_SIZE_SIZE);
// OBSOLETE 			  if (usp > stack_base &&
// OBSOLETE 			      usp <= stack_base + stack_size)
// OBSOLETE 			    {
// OBSOLETE 			      mthread->cproc = scan;
// OBSOLETE 			      scan->reverse_map = index;
// OBSOLETE 			      break;
// OBSOLETE 			    }
// OBSOLETE 			}
// OBSOLETE 		    }
// OBSOLETE 		}
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Format of the thread_list command
// OBSOLETE  *
// OBSOLETE  *                   slot mid sel   name  emul ks susp  cstate wired   address
// OBSOLETE  */
// OBSOLETE #define TL_FORMAT "%-2.2s %5d%c %-10.10s %1.1s%s%-5.5s %-2.2s %-5.5s "
// OBSOLETE 
// OBSOLETE #define TL_HEADER "\n@    MID  Name        KState CState   Where\n"
// OBSOLETE 
// OBSOLETE void
// OBSOLETE print_tl_address (struct ui_file *stream, CORE_ADDR pc)
// OBSOLETE {
// OBSOLETE   if (!lookup_minimal_symbol_by_pc (pc))
// OBSOLETE     fprintf_filtered (stream, local_hex_format (), pc);
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       extern int addressprint;
// OBSOLETE       extern int asm_demangle;
// OBSOLETE 
// OBSOLETE       int store = addressprint;
// OBSOLETE       addressprint = 0;
// OBSOLETE       print_address_symbolic (pc, stream, asm_demangle, "");
// OBSOLETE       addressprint = store;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* For thread names, but also for gdb_message_port external name */
// OBSOLETE #define MAX_NAME_LEN 50
// OBSOLETE 
// OBSOLETE /* Returns the address of variable NAME or 0 if not found */
// OBSOLETE CORE_ADDR
// OBSOLETE lookup_address_of_variable (char *name)
// OBSOLETE {
// OBSOLETE   struct symbol *sym;
// OBSOLETE   CORE_ADDR symaddr = 0;
// OBSOLETE   struct minimal_symbol *msymbol;
// OBSOLETE 
// OBSOLETE   sym = lookup_symbol (name,
// OBSOLETE 		       (struct block *) NULL,
// OBSOLETE 		       VAR_NAMESPACE,
// OBSOLETE 		       (int *) NULL,
// OBSOLETE 		       (struct symtab **) NULL);
// OBSOLETE 
// OBSOLETE   if (sym)
// OBSOLETE     symaddr = SYMBOL_VALUE (sym);
// OBSOLETE 
// OBSOLETE   if (!symaddr)
// OBSOLETE     {
// OBSOLETE       msymbol = lookup_minimal_symbol (name, NULL, NULL);
// OBSOLETE 
// OBSOLETE       if (msymbol && msymbol->type == mst_data)
// OBSOLETE 	symaddr = SYMBOL_VALUE_ADDRESS (msymbol);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return symaddr;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static gdb_thread_t
// OBSOLETE get_cprocs (void)
// OBSOLETE {
// OBSOLETE   gdb_thread_t cproc_head;
// OBSOLETE   gdb_thread_t cproc_copy;
// OBSOLETE   CORE_ADDR their_cprocs;
// OBSOLETE   char *buf;
// OBSOLETE   char *name;
// OBSOLETE   cthread_t cthread;
// OBSOLETE   CORE_ADDR symaddr;
// OBSOLETE 
// OBSOLETE   buf = alloca (TARGET_PTR_BIT / HOST_CHAR_BIT);
// OBSOLETE   symaddr = lookup_address_of_variable ("cproc_list");
// OBSOLETE 
// OBSOLETE   if (!symaddr)
// OBSOLETE     {
// OBSOLETE       /* cproc_list is not in a file compiled with debugging
// OBSOLETE          symbols, but don't give up yet */
// OBSOLETE 
// OBSOLETE       symaddr = lookup_address_of_variable ("cprocs");
// OBSOLETE 
// OBSOLETE       if (symaddr)
// OBSOLETE 	{
// OBSOLETE 	  static int informed = 0;
// OBSOLETE 	  if (!informed)
// OBSOLETE 	    {
// OBSOLETE 	      informed++;
// OBSOLETE 	      warning ("Your program is loaded with an old threads library.");
// OBSOLETE 	      warning ("GDB does not know the old form of threads");
// OBSOLETE 	      warning ("so things may not work.");
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Stripped or no -lthreads loaded or "cproc_list" is in wrong segment. */
// OBSOLETE   if (!symaddr)
// OBSOLETE     return NULL;
// OBSOLETE 
// OBSOLETE   /* Get the address of the first cproc in the task */
// OBSOLETE   if (!mach3_read_inferior (symaddr,
// OBSOLETE 			    buf,
// OBSOLETE 			    TARGET_PTR_BIT / HOST_CHAR_BIT))
// OBSOLETE     error ("Can't read cproc master list at address (0x%x).", symaddr);
// OBSOLETE   their_cprocs = extract_address (buf, TARGET_PTR_BIT / HOST_CHAR_BIT);
// OBSOLETE 
// OBSOLETE   /* Scan the CPROCs in the task.
// OBSOLETE      CPROCs are chained with LIST field, not NEXT field, which
// OBSOLETE      chains mutexes, condition variables and queues */
// OBSOLETE 
// OBSOLETE   cproc_head = NULL;
// OBSOLETE 
// OBSOLETE   while (their_cprocs != (CORE_ADDR) 0)
// OBSOLETE     {
// OBSOLETE       CORE_ADDR cproc_copy_incarnation;
// OBSOLETE       cproc_copy = (gdb_thread_t) obstack_alloc (cproc_obstack,
// OBSOLETE 						 sizeof (struct gdb_thread));
// OBSOLETE 
// OBSOLETE       if (!mach3_read_inferior (their_cprocs,
// OBSOLETE 				&cproc_copy->raw_cproc[0],
// OBSOLETE 				CPROC_SIZE))
// OBSOLETE 	error ("Can't read next cproc at 0x%x.", their_cprocs);
// OBSOLETE 
// OBSOLETE       their_cprocs =
// OBSOLETE 	extract_address (cproc_copy->raw_cproc + CPROC_LIST_OFFSET,
// OBSOLETE 			 CPROC_LIST_SIZE);
// OBSOLETE       cproc_copy_incarnation =
// OBSOLETE 	extract_address (cproc_copy->raw_cproc + CPROC_INCARNATION_OFFSET,
// OBSOLETE 			 CPROC_INCARNATION_SIZE);
// OBSOLETE 
// OBSOLETE       if (cproc_copy_incarnation == (CORE_ADDR) 0)
// OBSOLETE 	cproc_copy->cthread = NULL;
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  /* This CPROC has an attached CTHREAD. Get its name */
// OBSOLETE 	  cthread = (cthread_t) obstack_alloc (cproc_obstack,
// OBSOLETE 					       sizeof (struct cthread));
// OBSOLETE 
// OBSOLETE 	  if (!mach3_read_inferior (cproc_copy_incarnation,
// OBSOLETE 				    cthread,
// OBSOLETE 				    sizeof (struct cthread)))
// OBSOLETE 	      error ("Can't read next thread at 0x%x.",
// OBSOLETE 		     cproc_copy_incarnation);
// OBSOLETE 
// OBSOLETE 	  cproc_copy->cthread = cthread;
// OBSOLETE 
// OBSOLETE 	  if (cthread->name)
// OBSOLETE 	    {
// OBSOLETE 	      name = (char *) obstack_alloc (cproc_obstack, MAX_NAME_LEN);
// OBSOLETE 
// OBSOLETE 	      if (!mach3_read_inferior (cthread->name, name, MAX_NAME_LEN))
// OBSOLETE 		error ("Can't read next thread's name at 0x%x.", cthread->name);
// OBSOLETE 
// OBSOLETE 	      cthread->name = name;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* insert in front */
// OBSOLETE       cproc_copy->next = cproc_head;
// OBSOLETE       cproc_head = cproc_copy;
// OBSOLETE     }
// OBSOLETE   return cproc_head;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifndef FETCH_CPROC_STATE
// OBSOLETE /*
// OBSOLETE  * Check if your machine does not grok the way this routine
// OBSOLETE  * fetches the FP,PC and SP of a cproc that is not
// OBSOLETE  * currently attached to any kernel thread (e.g. its cproc.context
// OBSOLETE  * field points to the place in stack where the context
// OBSOLETE  * is saved).
// OBSOLETE  *
// OBSOLETE  * If it doesn't, define your own routine.
// OBSOLETE  */
// OBSOLETE #define FETCH_CPROC_STATE(mth) mach3_cproc_state (mth)
// OBSOLETE 
// OBSOLETE int
// OBSOLETE mach3_cproc_state (gdb_thread_t mthread)
// OBSOLETE {
// OBSOLETE   int context;
// OBSOLETE 
// OBSOLETE   if (!mthread || !mthread->cproc)
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   context = extract_signed_integer
// OBSOLETE     (mthread->cproc->raw_cproc + CPROC_CONTEXT_OFFSET,
// OBSOLETE      CPROC_CONTEXT_SIZE);
// OBSOLETE   if (context == 0)
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   mthread->sp = context + MACHINE_CPROC_SP_OFFSET;
// OBSOLETE 
// OBSOLETE   if (mach3_read_inferior (context + MACHINE_CPROC_PC_OFFSET,
// OBSOLETE 			   &mthread->pc,
// OBSOLETE 			   sizeof (CORE_ADDR)) != sizeof (CORE_ADDR))
// OBSOLETE     {
// OBSOLETE       warning ("Can't read cproc pc from inferior");
// OBSOLETE       return -1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (mach3_read_inferior (context + MACHINE_CPROC_FP_OFFSET,
// OBSOLETE 			   &mthread->fp,
// OBSOLETE 			   sizeof (CORE_ADDR)) != sizeof (CORE_ADDR))
// OBSOLETE     {
// OBSOLETE       warning ("Can't read cproc fp from inferior");
// OBSOLETE       return -1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE #endif /* FETCH_CPROC_STATE */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE void
// OBSOLETE thread_list_command (void)
// OBSOLETE {
// OBSOLETE   thread_basic_info_data_t ths;
// OBSOLETE   int thread_count;
// OBSOLETE   gdb_thread_t cprocs;
// OBSOLETE   gdb_thread_t scan;
// OBSOLETE   int index;
// OBSOLETE   char *name;
// OBSOLETE   char selected;
// OBSOLETE   char *wired;
// OBSOLETE   int infoCnt;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   mach_port_t mid_or_port;
// OBSOLETE   gdb_thread_t their_threads;
// OBSOLETE   gdb_thread_t kthread;
// OBSOLETE 
// OBSOLETE   int neworder = 1;
// OBSOLETE 
// OBSOLETE   char *fmt = "There are %d kernel threads in task %d.\n";
// OBSOLETE 
// OBSOLETE   int tmid = map_port_name_to_mid (inferior_task, MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   thread_count = fetch_thread_info (inferior_task,
// OBSOLETE 				    &their_threads);
// OBSOLETE   if (thread_count == -1)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   if (thread_count == 1)
// OBSOLETE     fmt = "There is %d kernel thread in task %d.\n";
// OBSOLETE 
// OBSOLETE   printf_filtered (fmt, thread_count, tmid);
// OBSOLETE 
// OBSOLETE   puts_filtered (TL_HEADER);
// OBSOLETE 
// OBSOLETE   cprocs = get_cprocs ();
// OBSOLETE 
// OBSOLETE   map_cprocs_to_kernel_threads (cprocs, their_threads, thread_count);
// OBSOLETE 
// OBSOLETE   for (scan = cprocs; scan; scan = scan->next)
// OBSOLETE     {
// OBSOLETE       int mid;
// OBSOLETE       char buf[10];
// OBSOLETE       char slot[3];
// OBSOLETE       int cproc_state =
// OBSOLETE       extract_signed_integer
// OBSOLETE       (scan->raw_cproc + CPROC_STATE_OFFSET, CPROC_STATE_SIZE);
// OBSOLETE 
// OBSOLETE       selected = ' ';
// OBSOLETE 
// OBSOLETE       /* a wired cproc? */
// OBSOLETE       wired = (extract_address (scan->raw_cproc + CPROC_WIRED_OFFSET,
// OBSOLETE 				CPROC_WIRED_SIZE)
// OBSOLETE 	       ? "wired" : "");
// OBSOLETE 
// OBSOLETE       if (scan->reverse_map != -1)
// OBSOLETE 	kthread = (their_threads + scan->reverse_map);
// OBSOLETE       else
// OBSOLETE 	kthread = NULL;
// OBSOLETE 
// OBSOLETE       if (kthread)
// OBSOLETE 	{
// OBSOLETE 	  /* These cprocs have a kernel thread */
// OBSOLETE 
// OBSOLETE 	  mid = map_port_name_to_mid (kthread->name, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE 	  infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE 
// OBSOLETE 	  ret = thread_info (kthread->name,
// OBSOLETE 			     THREAD_BASIC_INFO,
// OBSOLETE 			     (thread_info_t) & ths,
// OBSOLETE 			     &infoCnt);
// OBSOLETE 
// OBSOLETE 	  if (ret != KERN_SUCCESS)
// OBSOLETE 	    {
// OBSOLETE 	      warning ("Unable to get basic info on thread %d : %s",
// OBSOLETE 		       mid,
// OBSOLETE 		       mach_error_string (ret));
// OBSOLETE 	      continue;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  /* Who is the first to have more than 100 threads */
// OBSOLETE 	  sprintf (slot, "%d", kthread->slotid % 100);
// OBSOLETE 
// OBSOLETE 	  if (kthread->name == current_thread)
// OBSOLETE 	    selected = '*';
// OBSOLETE 
// OBSOLETE 	  if (ths.suspend_count)
// OBSOLETE 	    sprintf (buf, "%d", ths.suspend_count);
// OBSOLETE 	  else
// OBSOLETE 	    buf[0] = '\000';
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE 	  if (ths.flags & TH_FLAGS_SWAPPED)
// OBSOLETE 	    strcat (buf, "S");
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE 	  if (ths.flags & TH_FLAGS_IDLE)
// OBSOLETE 	    strcat (buf, "I");
// OBSOLETE 
// OBSOLETE 	  printf_filtered (TL_FORMAT,
// OBSOLETE 			   slot,
// OBSOLETE 			   mid,
// OBSOLETE 			   selected,
// OBSOLETE 			   get_thread_name (scan, kthread->slotid),
// OBSOLETE 			   kthread->in_emulator ? "E" : "",
// OBSOLETE 			   translate_state (ths.run_state),
// OBSOLETE 			   buf,
// OBSOLETE 			   translate_cstate (cproc_state),
// OBSOLETE 			   wired);
// OBSOLETE 	  print_tl_address (gdb_stdout, kthread->pc);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  /* These cprocs don't have a kernel thread.
// OBSOLETE 	   * find out the calling frame with 
// OBSOLETE 	   * FETCH_CPROC_STATE.
// OBSOLETE 	   */
// OBSOLETE 
// OBSOLETE 	  struct gdb_thread state;
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE 	  /* jtv -> emcmanus: why do you want this here? */
// OBSOLETE 	  if (scan->incarnation == NULL)
// OBSOLETE 	    continue;		/* EMcM */
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE 	  printf_filtered (TL_FORMAT,
// OBSOLETE 			   "-",
// OBSOLETE 			   -neworder,	/* Pseudo MID */
// OBSOLETE 			   selected,
// OBSOLETE 			   get_thread_name (scan, -neworder),
// OBSOLETE 			   "",
// OBSOLETE 			   "-",	/* kernel state */
// OBSOLETE 			   "",
// OBSOLETE 			   translate_cstate (cproc_state),
// OBSOLETE 			   "");
// OBSOLETE 	  state.cproc = scan;
// OBSOLETE 
// OBSOLETE 	  if (FETCH_CPROC_STATE (&state) == -1)
// OBSOLETE 	    puts_filtered ("???");
// OBSOLETE 	  else
// OBSOLETE 	    print_tl_address (gdb_stdout, state.pc);
// OBSOLETE 
// OBSOLETE 	  neworder++;
// OBSOLETE 	}
// OBSOLETE       puts_filtered ("\n");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Scan for kernel threads without cprocs */
// OBSOLETE   for (index = 0; index < thread_count; index++)
// OBSOLETE     {
// OBSOLETE       if (!their_threads[index].cproc)
// OBSOLETE 	{
// OBSOLETE 	  int mid;
// OBSOLETE 
// OBSOLETE 	  char buf[10];
// OBSOLETE 	  char slot[3];
// OBSOLETE 
// OBSOLETE 	  mach_port_t name = their_threads[index].name;
// OBSOLETE 
// OBSOLETE 	  mid = map_port_name_to_mid (name, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE 	  infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE 
// OBSOLETE 	  ret = thread_info (name,
// OBSOLETE 			     THREAD_BASIC_INFO,
// OBSOLETE 			     (thread_info_t) & ths,
// OBSOLETE 			     &infoCnt);
// OBSOLETE 
// OBSOLETE 	  if (ret != KERN_SUCCESS)
// OBSOLETE 	    {
// OBSOLETE 	      warning ("Unable to get basic info on thread %d : %s",
// OBSOLETE 		       mid,
// OBSOLETE 		       mach_error_string (ret));
// OBSOLETE 	      continue;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  sprintf (slot, "%d", index % 100);
// OBSOLETE 
// OBSOLETE 	  if (name == current_thread)
// OBSOLETE 	    selected = '*';
// OBSOLETE 	  else
// OBSOLETE 	    selected = ' ';
// OBSOLETE 
// OBSOLETE 	  if (ths.suspend_count)
// OBSOLETE 	    sprintf (buf, "%d", ths.suspend_count);
// OBSOLETE 	  else
// OBSOLETE 	    buf[0] = '\000';
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE 	  if (ths.flags & TH_FLAGS_SWAPPED)
// OBSOLETE 	    strcat (buf, "S");
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE 	  if (ths.flags & TH_FLAGS_IDLE)
// OBSOLETE 	    strcat (buf, "I");
// OBSOLETE 
// OBSOLETE 	  printf_filtered (TL_FORMAT,
// OBSOLETE 			   slot,
// OBSOLETE 			   mid,
// OBSOLETE 			   selected,
// OBSOLETE 			   get_thread_name (NULL, index),
// OBSOLETE 			   their_threads[index].in_emulator ? "E" : "",
// OBSOLETE 			   translate_state (ths.run_state),
// OBSOLETE 			   buf,
// OBSOLETE 			   "",	/* No cproc state */
// OBSOLETE 			   "");	/* Can't be wired */
// OBSOLETE 	  print_tl_address (gdb_stdout, their_threads[index].pc);
// OBSOLETE 	  puts_filtered ("\n");
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   obstack_free (cproc_obstack, 0);
// OBSOLETE   obstack_init (cproc_obstack);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE thread_select_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int mid;
// OBSOLETE   thread_array_t thread_list;
// OBSOLETE   int thread_count;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int is_slot = 0;
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   if (!args)
// OBSOLETE     error_no_arg ("MID or @SLOTNUMBER to specify a thread to select");
// OBSOLETE 
// OBSOLETE   while (*args == ' ' || *args == '\t')
// OBSOLETE     args++;
// OBSOLETE 
// OBSOLETE   if (*args == '@')
// OBSOLETE     {
// OBSOLETE       is_slot++;
// OBSOLETE       args++;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   mid = atoi (args);
// OBSOLETE 
// OBSOLETE   if (mid == 0)
// OBSOLETE     if (!is_slot || *args != '0')	/* Rudimentary checks */
// OBSOLETE       error ("You must select threads by MID or @SLOTNUMBER");
// OBSOLETE 
// OBSOLETE   if (select_thread (inferior_task, mid, is_slot ? 2 : 1) != KERN_SUCCESS)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   if (from_tty)
// OBSOLETE     printf_filtered ("Thread %d selected\n",
// OBSOLETE 		     is_slot ? map_port_name_to_mid (current_thread,
// OBSOLETE 						   MACH_TYPE_THREAD) : mid);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE thread_trace (mach_port_t thread, boolean_t set)
// OBSOLETE {
// OBSOLETE   int flavor = TRACE_FLAVOR;
// OBSOLETE   unsigned int stateCnt = TRACE_FLAVOR_SIZE;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_state_data_t state;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (thread))
// OBSOLETE     {
// OBSOLETE       warning ("thread_trace: invalid thread");
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (must_suspend_thread)
// OBSOLETE     setup_thread (thread, 1);
// OBSOLETE 
// OBSOLETE   ret = thread_get_state (thread, flavor, state, &stateCnt);
// OBSOLETE   CHK ("thread_trace: error reading thread state", ret);
// OBSOLETE 
// OBSOLETE   if (set)
// OBSOLETE     {
// OBSOLETE       TRACE_SET (thread, state);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       if (!TRACE_CLEAR (thread, state))
// OBSOLETE 	{
// OBSOLETE 	  if (must_suspend_thread)
// OBSOLETE 	    setup_thread (thread, 0);
// OBSOLETE 	  return;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = thread_set_state (thread, flavor, state, stateCnt);
// OBSOLETE   CHK ("thread_trace: error writing thread state", ret);
// OBSOLETE   if (must_suspend_thread)
// OBSOLETE     setup_thread (thread, 0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef	FLUSH_INFERIOR_CACHE
// OBSOLETE 
// OBSOLETE /* When over-writing code on some machines the I-Cache must be flushed
// OBSOLETE    explicitly, because it is not kept coherent by the lazy hardware.
// OBSOLETE    This definitely includes breakpoints, for instance, or else we
// OBSOLETE    end up looping in mysterious Bpt traps */
// OBSOLETE 
// OBSOLETE flush_inferior_icache (CORE_ADDR pc, int amount)
// OBSOLETE {
// OBSOLETE   vm_machine_attribute_val_t flush = MATTR_VAL_ICACHE_FLUSH;
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   ret = vm_machine_attribute (inferior_task,
// OBSOLETE 			      pc,
// OBSOLETE 			      amount,
// OBSOLETE 			      MATTR_CACHE,
// OBSOLETE 			      &flush);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     warning ("Error flushing inferior's cache : %s",
// OBSOLETE 	     mach_error_string (ret));
// OBSOLETE }
// OBSOLETE #endif /* FLUSH_INFERIOR_CACHE */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static
// OBSOLETE suspend_all_threads (int from_tty)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_array_t thread_list;
// OBSOLETE   int thread_count, index;
// OBSOLETE   int infoCnt;
// OBSOLETE   thread_basic_info_data_t th_info;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   ret = task_threads (inferior_task, &thread_list, &thread_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("Could not suspend inferior threads.");
// OBSOLETE       m3_kill_inferior ();
// OBSOLETE       throw_exception (RETURN_ERROR);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   for (index = 0; index < thread_count; index++)
// OBSOLETE     {
// OBSOLETE       int mid;
// OBSOLETE 
// OBSOLETE       mid = map_port_name_to_mid (thread_list[index],
// OBSOLETE 				  MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE       ret = thread_suspend (thread_list[index]);
// OBSOLETE 
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	warning ("Error trying to suspend thread %d : %s",
// OBSOLETE 		 mid, mach_error_string (ret));
// OBSOLETE 
// OBSOLETE       if (from_tty)
// OBSOLETE 	{
// OBSOLETE 	  infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE 	  ret = thread_info (thread_list[index],
// OBSOLETE 			     THREAD_BASIC_INFO,
// OBSOLETE 			     (thread_info_t) & th_info,
// OBSOLETE 			     &infoCnt);
// OBSOLETE 	  CHK ("suspend can't get thread info", ret);
// OBSOLETE 
// OBSOLETE 	  warning ("Thread %d suspend count is %d",
// OBSOLETE 		   mid, th_info.suspend_count);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   consume_send_rights (thread_list, thread_count);
// OBSOLETE   ret = vm_deallocate (mach_task_self (),
// OBSOLETE 		       (vm_address_t) thread_list,
// OBSOLETE 		       (thread_count * sizeof (int)));
// OBSOLETE   CHK ("Error trying to deallocate thread list", ret);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE thread_suspend_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int mid;
// OBSOLETE   mach_port_t saved_thread;
// OBSOLETE   int infoCnt;
// OBSOLETE   thread_basic_info_data_t th_info;
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   if (!strcasecmp (args, "all"))
// OBSOLETE     {
// OBSOLETE       suspend_all_threads (from_tty);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   saved_thread = current_thread;
// OBSOLETE 
// OBSOLETE   mid = parse_thread_id (args, 0, 0);
// OBSOLETE 
// OBSOLETE   if (mid < 0)
// OBSOLETE     error ("You can suspend only existing kernel threads with MID or @SLOTNUMBER");
// OBSOLETE 
// OBSOLETE   if (mid == 0)
// OBSOLETE     mid = map_port_name_to_mid (current_thread, MACH_TYPE_THREAD);
// OBSOLETE   else if (select_thread (inferior_task, mid, 0) != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       if (current_thread)
// OBSOLETE 	current_thread = saved_thread;
// OBSOLETE       error ("Could not select thread %d", mid);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = thread_suspend (current_thread);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     warning ("thread_suspend failed : %s",
// OBSOLETE 	     mach_error_string (ret));
// OBSOLETE 
// OBSOLETE   infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE   ret = thread_info (current_thread,
// OBSOLETE 		     THREAD_BASIC_INFO,
// OBSOLETE 		     (thread_info_t) & th_info,
// OBSOLETE 		     &infoCnt);
// OBSOLETE   CHK ("suspend can't get thread info", ret);
// OBSOLETE 
// OBSOLETE   warning ("Thread %d suspend count is %d", mid, th_info.suspend_count);
// OBSOLETE 
// OBSOLETE   current_thread = saved_thread;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE resume_all_threads (int from_tty)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_array_t thread_list;
// OBSOLETE   int thread_count, index;
// OBSOLETE   int mid;
// OBSOLETE   int infoCnt;
// OBSOLETE   thread_basic_info_data_t th_info;
// OBSOLETE 
// OBSOLETE   ret = task_threads (inferior_task, &thread_list, &thread_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       m3_kill_inferior ();
// OBSOLETE       error ("task_threads", mach_error_string (ret));
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   for (index = 0; index < thread_count; index++)
// OBSOLETE     {
// OBSOLETE       infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE       ret = thread_info (thread_list[index],
// OBSOLETE 			 THREAD_BASIC_INFO,
// OBSOLETE 			 (thread_info_t) & th_info,
// OBSOLETE 			 &infoCnt);
// OBSOLETE       CHK ("resume_all can't get thread info", ret);
// OBSOLETE 
// OBSOLETE       mid = map_port_name_to_mid (thread_list[index],
// OBSOLETE 				  MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE       if (!th_info.suspend_count)
// OBSOLETE 	{
// OBSOLETE 	  if (mid != -1 && from_tty)
// OBSOLETE 	    warning ("Thread %d is not suspended", mid);
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       ret = thread_resume (thread_list[index]);
// OBSOLETE 
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	warning ("Error trying to resume thread %d : %s",
// OBSOLETE 		 mid, mach_error_string (ret));
// OBSOLETE       else if (mid != -1 && from_tty)
// OBSOLETE 	warning ("Thread %d suspend count is %d",
// OBSOLETE 		 mid, --th_info.suspend_count);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   consume_send_rights (thread_list, thread_count);
// OBSOLETE   ret = vm_deallocate (mach_task_self (),
// OBSOLETE 		       (vm_address_t) thread_list,
// OBSOLETE 		       (thread_count * sizeof (int)));
// OBSOLETE   CHK ("Error trying to deallocate thread list", ret);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE thread_resume_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int mid;
// OBSOLETE   mach_port_t saved_thread;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_basic_info_data_t th_info;
// OBSOLETE   int infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   if (!strcasecmp (args, "all"))
// OBSOLETE     {
// OBSOLETE       resume_all_threads (from_tty);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   saved_thread = current_thread;
// OBSOLETE 
// OBSOLETE   mid = parse_thread_id (args, 0, 0);
// OBSOLETE 
// OBSOLETE   if (mid < 0)
// OBSOLETE     error ("You can resume only existing kernel threads with MID or @SLOTNUMBER");
// OBSOLETE 
// OBSOLETE   if (mid == 0)
// OBSOLETE     mid = map_port_name_to_mid (current_thread, MACH_TYPE_THREAD);
// OBSOLETE   else if (select_thread (inferior_task, mid, 0) != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       if (current_thread)
// OBSOLETE 	current_thread = saved_thread;
// OBSOLETE       throw_exception (RETURN_ERROR);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = thread_info (current_thread,
// OBSOLETE 		     THREAD_BASIC_INFO,
// OBSOLETE 		     (thread_info_t) & th_info,
// OBSOLETE 		     &infoCnt);
// OBSOLETE   CHK ("resume can't get thread info", ret);
// OBSOLETE 
// OBSOLETE   if (!th_info.suspend_count)
// OBSOLETE     {
// OBSOLETE       warning ("Thread %d is not suspended", mid);
// OBSOLETE       goto out;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = thread_resume (current_thread);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     warning ("thread_resume failed : %s",
// OBSOLETE 	     mach_error_string (ret));
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       th_info.suspend_count--;
// OBSOLETE       warning ("Thread %d suspend count is %d", mid, th_info.suspend_count);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE out:
// OBSOLETE   current_thread = saved_thread;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE thread_kill_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int mid;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   int thread_count;
// OBSOLETE   thread_array_t thread_table;
// OBSOLETE   int index;
// OBSOLETE   mach_port_t thread_to_kill = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   if (!args)
// OBSOLETE     error_no_arg ("thread mid to kill from the inferior task");
// OBSOLETE 
// OBSOLETE   mid = parse_thread_id (args, 0, 0);
// OBSOLETE 
// OBSOLETE   if (mid < 0)
// OBSOLETE     error ("You can kill only existing kernel threads with MID or @SLOTNUMBER");
// OBSOLETE 
// OBSOLETE   if (mid)
// OBSOLETE     {
// OBSOLETE       ret = machid_mach_port (mid_server, mid_auth, mid, &thread_to_kill);
// OBSOLETE       CHK ("thread_kill_command: machid_mach_port map failed", ret);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     mid = map_port_name_to_mid (current_thread, MACH_TYPE_THREAD);
// OBSOLETE 
// OBSOLETE   /* Don't allow gdb to kill *any* thread in the system. Use mkill program for that */
// OBSOLETE   ret = task_threads (inferior_task, &thread_table, &thread_count);
// OBSOLETE   CHK ("Error getting inferior's thread list", ret);
// OBSOLETE 
// OBSOLETE   if (thread_to_kill == current_thread)
// OBSOLETE     {
// OBSOLETE       ret = thread_terminate (thread_to_kill);
// OBSOLETE       CHK ("Thread could not be terminated", ret);
// OBSOLETE 
// OBSOLETE       if (select_thread (inferior_task, 0, 1) != KERN_SUCCESS)
// OBSOLETE 	warning ("Last thread was killed, use \"kill\" command to kill task");
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     for (index = 0; index < thread_count; index++)
// OBSOLETE       if (thread_table[index] == thread_to_kill)
// OBSOLETE 	{
// OBSOLETE 	  ret = thread_terminate (thread_to_kill);
// OBSOLETE 	  CHK ("Thread could not be terminated", ret);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE   if (thread_count > 1)
// OBSOLETE     consume_send_rights (thread_table, thread_count);
// OBSOLETE 
// OBSOLETE   ret = vm_deallocate (mach_task_self (), (vm_address_t) thread_table,
// OBSOLETE 		       (thread_count * sizeof (mach_port_t)));
// OBSOLETE   CHK ("Error trying to deallocate thread list", ret);
// OBSOLETE 
// OBSOLETE   warning ("Thread %d killed", mid);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Task specific commands; add more if you like */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE task_resume_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   task_basic_info_data_t ta_info;
// OBSOLETE   int infoCnt = TASK_BASIC_INFO_COUNT;
// OBSOLETE   int mid = map_port_name_to_mid (inferior_task, MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   /* Would be trivial to change, but is it desirable? */
// OBSOLETE   if (args)
// OBSOLETE     error ("Currently gdb can resume only it's inferior task");
// OBSOLETE 
// OBSOLETE   ret = task_info (inferior_task,
// OBSOLETE 		   TASK_BASIC_INFO,
// OBSOLETE 		   (task_info_t) & ta_info,
// OBSOLETE 		   &infoCnt);
// OBSOLETE   CHK ("task_resume_command: task_info failed", ret);
// OBSOLETE 
// OBSOLETE   if (ta_info.suspend_count == 0)
// OBSOLETE     error ("Inferior task %d is not suspended", mid);
// OBSOLETE   else if (ta_info.suspend_count == 1 &&
// OBSOLETE 	   from_tty &&
// OBSOLETE 	!query ("Suspend count is now 1. Do you know what you are doing? "))
// OBSOLETE     error ("Task not resumed");
// OBSOLETE 
// OBSOLETE   ret = task_resume (inferior_task);
// OBSOLETE   CHK ("task_resume_command: task_resume", ret);
// OBSOLETE 
// OBSOLETE   if (ta_info.suspend_count == 1)
// OBSOLETE     {
// OBSOLETE       warning ("Inferior task %d is no longer suspended", mid);
// OBSOLETE       must_suspend_thread = 1;
// OBSOLETE       /* @@ This is not complete: Registers change all the time when not
// OBSOLETE          suspended! */
// OBSOLETE       registers_changed ();
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     warning ("Inferior task %d suspend count is now %d",
// OBSOLETE 	     mid, ta_info.suspend_count - 1);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE void
// OBSOLETE task_suspend_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   task_basic_info_data_t ta_info;
// OBSOLETE   int infoCnt = TASK_BASIC_INFO_COUNT;
// OBSOLETE   int mid = map_port_name_to_mid (inferior_task, MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   /* Would be trivial to change, but is it desirable? */
// OBSOLETE   if (args)
// OBSOLETE     error ("Currently gdb can suspend only it's inferior task");
// OBSOLETE 
// OBSOLETE   ret = task_suspend (inferior_task);
// OBSOLETE   CHK ("task_suspend_command: task_suspend", ret);
// OBSOLETE 
// OBSOLETE   must_suspend_thread = 0;
// OBSOLETE 
// OBSOLETE   ret = task_info (inferior_task,
// OBSOLETE 		   TASK_BASIC_INFO,
// OBSOLETE 		   (task_info_t) & ta_info,
// OBSOLETE 		   &infoCnt);
// OBSOLETE   CHK ("task_suspend_command: task_info failed", ret);
// OBSOLETE 
// OBSOLETE   warning ("Inferior task %d suspend count is now %d",
// OBSOLETE 	   mid, ta_info.suspend_count);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE get_size (int bytes)
// OBSOLETE {
// OBSOLETE   static char size[30];
// OBSOLETE   int zz = bytes / 1024;
// OBSOLETE 
// OBSOLETE   if (zz / 1024)
// OBSOLETE     sprintf (size, "%-2.1f M", ((float) bytes) / (1024.0 * 1024.0));
// OBSOLETE   else
// OBSOLETE     sprintf (size, "%d K", zz);
// OBSOLETE 
// OBSOLETE   return size;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Does this require the target task to be suspended?? I don't think so. */
// OBSOLETE void
// OBSOLETE task_info_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int mid = -5;
// OBSOLETE   mach_port_t task;
// OBSOLETE   kern_return_t ret;
// OBSOLETE   task_basic_info_data_t ta_info;
// OBSOLETE   int infoCnt = TASK_BASIC_INFO_COUNT;
// OBSOLETE   int page_size = round_page (1);
// OBSOLETE   int thread_count = 0;
// OBSOLETE 
// OBSOLETE   if (MACH_PORT_VALID (inferior_task))
// OBSOLETE     mid = map_port_name_to_mid (inferior_task,
// OBSOLETE 				MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   task = inferior_task;
// OBSOLETE 
// OBSOLETE   if (args)
// OBSOLETE     {
// OBSOLETE       int tmid = atoi (args);
// OBSOLETE 
// OBSOLETE       if (tmid <= 0)
// OBSOLETE 	error ("Invalid mid %d for task info", tmid);
// OBSOLETE 
// OBSOLETE       if (tmid != mid)
// OBSOLETE 	{
// OBSOLETE 	  mid = tmid;
// OBSOLETE 	  ret = machid_mach_port (mid_server, mid_auth, tmid, &task);
// OBSOLETE 	  CHK ("task_info_command: machid_mach_port map failed", ret);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (mid < 0)
// OBSOLETE     error ("You have to give the task MID as an argument");
// OBSOLETE 
// OBSOLETE   ret = task_info (task,
// OBSOLETE 		   TASK_BASIC_INFO,
// OBSOLETE 		   (task_info_t) & ta_info,
// OBSOLETE 		   &infoCnt);
// OBSOLETE   CHK ("task_info_command: task_info failed", ret);
// OBSOLETE 
// OBSOLETE   printf_filtered ("\nTask info for task %d:\n\n", mid);
// OBSOLETE   printf_filtered (" Suspend count : %d\n", ta_info.suspend_count);
// OBSOLETE   printf_filtered (" Base priority : %d\n", ta_info.base_priority);
// OBSOLETE   printf_filtered (" Virtual size  : %s\n", get_size (ta_info.virtual_size));
// OBSOLETE   printf_filtered (" Resident size : %s\n", get_size (ta_info.resident_size));
// OBSOLETE 
// OBSOLETE   {
// OBSOLETE     thread_array_t thread_list;
// OBSOLETE 
// OBSOLETE     ret = task_threads (task, &thread_list, &thread_count);
// OBSOLETE     CHK ("task_info_command: task_threads", ret);
// OBSOLETE 
// OBSOLETE     printf_filtered (" Thread count  : %d\n", thread_count);
// OBSOLETE 
// OBSOLETE     consume_send_rights (thread_list, thread_count);
// OBSOLETE     ret = vm_deallocate (mach_task_self (),
// OBSOLETE 			 (vm_address_t) thread_list,
// OBSOLETE 			 (thread_count * sizeof (int)));
// OBSOLETE     CHK ("Error trying to deallocate thread list", ret);
// OBSOLETE   }
// OBSOLETE   if (have_emulator_p (task))
// OBSOLETE     printf_filtered (" Emulator at   : 0x%x..0x%x\n",
// OBSOLETE 		     EMULATOR_BASE, EMULATOR_END);
// OBSOLETE   else
// OBSOLETE     printf_filtered (" No emulator.\n");
// OBSOLETE 
// OBSOLETE   if (thread_count && task == inferior_task)
// OBSOLETE     printf_filtered ("\nUse the \"thread list\" command to see the threads\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* You may either FORWARD the exception to the inferior, or KEEP
// OBSOLETE  * it and return to GDB command level.
// OBSOLETE  *
// OBSOLETE  * exception mid [ forward | keep ]
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE exception_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   char *scan = args;
// OBSOLETE   int exception;
// OBSOLETE   int len;
// OBSOLETE 
// OBSOLETE   if (!args)
// OBSOLETE     error_no_arg ("exception number action");
// OBSOLETE 
// OBSOLETE   while (*scan == ' ' || *scan == '\t')
// OBSOLETE     scan++;
// OBSOLETE 
// OBSOLETE   if ('0' <= *scan && *scan <= '9')
// OBSOLETE     while ('0' <= *scan && *scan <= '9')
// OBSOLETE       scan++;
// OBSOLETE   else
// OBSOLETE     error ("exception number action");
// OBSOLETE 
// OBSOLETE   exception = atoi (args);
// OBSOLETE   if (exception <= 0 || exception > MAX_EXCEPTION)
// OBSOLETE     error ("Allowed exception numbers are in range 1..%d",
// OBSOLETE 	   MAX_EXCEPTION);
// OBSOLETE 
// OBSOLETE   if (*scan != ' ' && *scan != '\t')
// OBSOLETE     error ("exception number must be followed by a space");
// OBSOLETE   else
// OBSOLETE     while (*scan == ' ' || *scan == '\t')
// OBSOLETE       scan++;
// OBSOLETE 
// OBSOLETE   args = scan;
// OBSOLETE   len = 0;
// OBSOLETE   while (*scan)
// OBSOLETE     {
// OBSOLETE       len++;
// OBSOLETE       scan++;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!len)
// OBSOLETE     error ("exception number action");
// OBSOLETE 
// OBSOLETE   if (!strncasecmp (args, "forward", len))
// OBSOLETE     exception_map[exception].forward = TRUE;
// OBSOLETE   else if (!strncasecmp (args, "keep", len))
// OBSOLETE     exception_map[exception].forward = FALSE;
// OBSOLETE   else
// OBSOLETE     error ("exception action is either \"keep\" or \"forward\"");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE print_exception_info (int exception)
// OBSOLETE {
// OBSOLETE   boolean_t forward = exception_map[exception].forward;
// OBSOLETE 
// OBSOLETE   printf_filtered ("%s\t(%d): ", exception_map[exception].name,
// OBSOLETE 		   exception);
// OBSOLETE   if (!forward)
// OBSOLETE     if (exception_map[exception].sigmap != SIG_UNKNOWN)
// OBSOLETE       printf_filtered ("keep and handle as signal %d\n",
// OBSOLETE 		       exception_map[exception].sigmap);
// OBSOLETE     else
// OBSOLETE       printf_filtered ("keep and handle as unknown signal %d\n",
// OBSOLETE 		       exception_map[exception].sigmap);
// OBSOLETE   else
// OBSOLETE     printf_filtered ("forward exception to inferior\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE exception_info (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int exception;
// OBSOLETE 
// OBSOLETE   if (!args)
// OBSOLETE     for (exception = 1; exception <= MAX_EXCEPTION; exception++)
// OBSOLETE       print_exception_info (exception);
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       exception = atoi (args);
// OBSOLETE 
// OBSOLETE       if (exception <= 0 || exception > MAX_EXCEPTION)
// OBSOLETE 	error ("Invalid exception number, values from 1 to %d allowed",
// OBSOLETE 	       MAX_EXCEPTION);
// OBSOLETE       print_exception_info (exception);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Check for actions for mach exceptions.
// OBSOLETE  */
// OBSOLETE mach3_exception_actions (WAITTYPE *w, boolean_t force_print_only, char *who)
// OBSOLETE {
// OBSOLETE   boolean_t force_print = FALSE;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   if (force_print_only ||
// OBSOLETE       exception_map[stop_exception].sigmap == SIG_UNKNOWN)
// OBSOLETE     force_print = TRUE;
// OBSOLETE   else
// OBSOLETE     WSETSTOP (*w, exception_map[stop_exception].sigmap);
// OBSOLETE 
// OBSOLETE   if (exception_map[stop_exception].print || force_print)
// OBSOLETE     {
// OBSOLETE       target_terminal_ours ();
// OBSOLETE 
// OBSOLETE       printf_filtered ("\n%s received %s exception : ",
// OBSOLETE 		       who,
// OBSOLETE 		       exception_map[stop_exception].name);
// OBSOLETE 
// OBSOLETE       wrap_here ("   ");
// OBSOLETE 
// OBSOLETE       switch (stop_exception)
// OBSOLETE 	{
// OBSOLETE 	case EXC_BAD_ACCESS:
// OBSOLETE 	  printf_filtered ("referencing address 0x%x : %s\n",
// OBSOLETE 			   stop_subcode,
// OBSOLETE 			   mach_error_string (stop_code));
// OBSOLETE 	  break;
// OBSOLETE 	case EXC_BAD_INSTRUCTION:
// OBSOLETE 	  printf_filtered
// OBSOLETE 	    ("illegal or undefined instruction. code %d subcode %d\n",
// OBSOLETE 	     stop_code, stop_subcode);
// OBSOLETE 	  break;
// OBSOLETE 	case EXC_ARITHMETIC:
// OBSOLETE 	  printf_filtered ("code %d\n", stop_code);
// OBSOLETE 	  break;
// OBSOLETE 	case EXC_EMULATION:
// OBSOLETE 	  printf_filtered ("code %d subcode %d\n", stop_code, stop_subcode);
// OBSOLETE 	  break;
// OBSOLETE 	case EXC_SOFTWARE:
// OBSOLETE 	  printf_filtered ("%s specific, code 0x%x\n",
// OBSOLETE 			   stop_code < 0xffff ? "hardware" : "os emulation",
// OBSOLETE 			   stop_code);
// OBSOLETE 	  break;
// OBSOLETE 	case EXC_BREAKPOINT:
// OBSOLETE 	  printf_filtered ("type %d (machine dependent)\n",
// OBSOLETE 			   stop_code);
// OBSOLETE 	  break;
// OBSOLETE 	default:
// OBSOLETE 	  internal_error (__FILE__, __LINE__,
// OBSOLETE 			  "Unknown exception");
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE setup_notify_port (int create_new)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (MACH_PORT_VALID (our_notify_port))
// OBSOLETE     {
// OBSOLETE       ret = mach_port_destroy (mach_task_self (), our_notify_port);
// OBSOLETE       CHK ("Could not destroy our_notify_port", ret);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   our_notify_port = MACH_PORT_NULL;
// OBSOLETE   notify_chain = (port_chain_t) NULL;
// OBSOLETE   port_chain_destroy (port_chain_obstack);
// OBSOLETE 
// OBSOLETE   if (create_new)
// OBSOLETE     {
// OBSOLETE       ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 				MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 				&our_notify_port);
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	internal_error (__FILE__, __LINE__,
// OBSOLETE 			"Creating notify port %s", mach_error_string (ret));
// OBSOLETE 
// OBSOLETE       ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 				   our_notify_port,
// OBSOLETE 				   inferior_wait_port_set);
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	internal_error (__FILE__, __LINE__,
// OBSOLETE 			"initial move member %s", mach_error_string (ret));
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Register our message port to the net name server
// OBSOLETE  *
// OBSOLETE  * Currently used only by the external stop-gdb program
// OBSOLETE  * since ^C does not work if you would like to enter
// OBSOLETE  * gdb command level while debugging your program.
// OBSOLETE  *
// OBSOLETE  * NOTE: If the message port is sometimes used for other
// OBSOLETE  * purposes also, the NAME must not be a guessable one.
// OBSOLETE  * Then, there should be a way to change it.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE char registered_name[MAX_NAME_LEN];
// OBSOLETE 
// OBSOLETE void
// OBSOLETE message_port_info (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   if (registered_name[0])
// OBSOLETE     printf_filtered ("gdb's message port name: '%s'\n",
// OBSOLETE 		     registered_name);
// OBSOLETE   else
// OBSOLETE     printf_filtered ("gdb's message port is not currently registered\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE gdb_register_port (char *name, mach_port_t port)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   static int already_signed = 0;
// OBSOLETE   int len;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (port) || !name || !*name)
// OBSOLETE     {
// OBSOLETE       warning ("Invalid registration request");
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!already_signed)
// OBSOLETE     {
// OBSOLETE       ret = mach_port_insert_right (mach_task_self (),
// OBSOLETE 				    our_message_port,
// OBSOLETE 				    our_message_port,
// OBSOLETE 				    MACH_MSG_TYPE_MAKE_SEND);
// OBSOLETE       CHK ("Failed to create a signature to our_message_port", ret);
// OBSOLETE       already_signed = 1;
// OBSOLETE     }
// OBSOLETE   else if (already_signed > 1)
// OBSOLETE     {
// OBSOLETE       ret = netname_check_out (name_server_port,
// OBSOLETE 			       registered_name,
// OBSOLETE 			       our_message_port);
// OBSOLETE       CHK ("Failed to check out gdb's message port", ret);
// OBSOLETE       registered_name[0] = '\000';
// OBSOLETE       already_signed = 1;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = netname_check_in (name_server_port,	/* Name server port */
// OBSOLETE 			  name,	/* Name of service */
// OBSOLETE 			  our_message_port,	/* Signature */
// OBSOLETE 			  port);	/* Creates a new send right */
// OBSOLETE   CHK ("Failed to check in the port", ret);
// OBSOLETE 
// OBSOLETE   len = 0;
// OBSOLETE   while (len < MAX_NAME_LEN && *(name + len))
// OBSOLETE     {
// OBSOLETE       registered_name[len] = *(name + len);
// OBSOLETE       len++;
// OBSOLETE     }
// OBSOLETE   registered_name[len] = '\000';
// OBSOLETE   already_signed = 2;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE struct cmd_list_element *cmd_thread_list;
// OBSOLETE struct cmd_list_element *cmd_task_list;
// OBSOLETE 
// OBSOLETE /*ARGSUSED */
// OBSOLETE static void
// OBSOLETE thread_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   printf_unfiltered ("\"thread\" must be followed by the name of a thread command.\n");
// OBSOLETE   help_list (cmd_thread_list, "thread ", -1, gdb_stdout);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*ARGSUSED */
// OBSOLETE static void
// OBSOLETE task_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   printf_unfiltered ("\"task\" must be followed by the name of a task command.\n");
// OBSOLETE   help_list (cmd_task_list, "task ", -1, gdb_stdout);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE add_mach_specific_commands (void)
// OBSOLETE {
// OBSOLETE   /* Thread handling commands */
// OBSOLETE 
// OBSOLETE   /* FIXME: Move our thread support into the generic thread.c stuff so we
// OBSOLETE      can share that code.  */
// OBSOLETE   add_prefix_cmd ("mthread", class_stack, thread_command,
// OBSOLETE 	  "Generic command for handling Mach threads in the debugged task.",
// OBSOLETE 		  &cmd_thread_list, "thread ", 0, &cmdlist);
// OBSOLETE 
// OBSOLETE   add_com_alias ("th", "mthread", class_stack, 1);
// OBSOLETE 
// OBSOLETE   add_cmd ("select", class_stack, thread_select_command,
// OBSOLETE 	   "Select and print MID of the selected thread",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE   add_cmd ("list", class_stack, thread_list_command,
// OBSOLETE 	   "List info of task's threads. Selected thread is marked with '*'",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE   add_cmd ("suspend", class_run, thread_suspend_command,
// OBSOLETE 	   "Suspend one or all of the threads in the selected task.",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE   add_cmd ("resume", class_run, thread_resume_command,
// OBSOLETE 	   "Resume one or all of the threads in the selected task.",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE   add_cmd ("kill", class_run, thread_kill_command,
// OBSOLETE 	   "Kill the specified thread MID from inferior task.",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE #if 0
// OBSOLETE   /* The rest of this support (condition_thread) was not merged.  It probably
// OBSOLETE      should not be merged in this form, but instead added to the generic GDB
// OBSOLETE      thread support.  */
// OBSOLETE   add_cmd ("break", class_breakpoint, condition_thread,
// OBSOLETE 	   "Breakpoint N will only be effective for thread MID or @SLOT\n\
// OBSOLETE 	    If MID/@SLOT is omitted allow all threads to break at breakpoint",
// OBSOLETE 	   &cmd_thread_list);
// OBSOLETE #endif
// OBSOLETE   /* Thread command shorthands (for backward compatibility) */
// OBSOLETE   add_alias_cmd ("ts", "mthread select", 0, 0, &cmdlist);
// OBSOLETE   add_alias_cmd ("tl", "mthread list", 0, 0, &cmdlist);
// OBSOLETE 
// OBSOLETE   /* task handling commands */
// OBSOLETE 
// OBSOLETE   add_prefix_cmd ("task", class_stack, task_command,
// OBSOLETE 		  "Generic command for handling debugged task.",
// OBSOLETE 		  &cmd_task_list, "task ", 0, &cmdlist);
// OBSOLETE 
// OBSOLETE   add_com_alias ("ta", "task", class_stack, 1);
// OBSOLETE 
// OBSOLETE   add_cmd ("suspend", class_run, task_suspend_command,
// OBSOLETE 	   "Suspend the inferior task.",
// OBSOLETE 	   &cmd_task_list);
// OBSOLETE   add_cmd ("resume", class_run, task_resume_command,
// OBSOLETE 	   "Resume the inferior task.",
// OBSOLETE 	   &cmd_task_list);
// OBSOLETE   add_cmd ("info", no_class, task_info_command,
// OBSOLETE 	   "Print information about the specified task.",
// OBSOLETE 	   &cmd_task_list);
// OBSOLETE 
// OBSOLETE   /* Print my message port name */
// OBSOLETE 
// OBSOLETE   add_info ("message-port", message_port_info,
// OBSOLETE 	    "Returns the name of gdb's message port in the netnameserver");
// OBSOLETE 
// OBSOLETE   /* Exception commands */
// OBSOLETE 
// OBSOLETE   add_info ("exceptions", exception_info,
// OBSOLETE 	    "What debugger does when program gets various exceptions.\n\
// OBSOLETE Specify an exception number as argument to print info on that\n\
// OBSOLETE exception only.");
// OBSOLETE 
// OBSOLETE   add_com ("exception", class_run, exception_command,
// OBSOLETE 	   "Specify how to handle an exception.\n\
// OBSOLETE Args are exception number followed by \"forward\" or \"keep\".\n\
// OBSOLETE `Forward' means forward the exception to the program's normal exception\n\
// OBSOLETE handler.\n\
// OBSOLETE `Keep' means reenter debugger if this exception happens, and GDB maps\n\
// OBSOLETE the exception to some signal (see info exception)\n\
// OBSOLETE Normally \"keep\" is used to return to GDB on exception.");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_dead_name (mach_port_t notify, mach_port_t name)
// OBSOLETE {
// OBSOLETE   kern_return_t kr = KERN_SUCCESS;
// OBSOLETE 
// OBSOLETE   /* Find the thing that notified */
// OBSOLETE   port_chain_t element = port_chain_member (notify_chain, name);
// OBSOLETE 
// OBSOLETE   /* Take name of from unreceived dead name notification list */
// OBSOLETE   notify_chain = port_chain_delete (notify_chain, name);
// OBSOLETE 
// OBSOLETE   if (!element)
// OBSOLETE     error ("Received a dead name notify from unchained port (0x%x)", name);
// OBSOLETE 
// OBSOLETE   switch (element->type)
// OBSOLETE     {
// OBSOLETE 
// OBSOLETE     case MACH_TYPE_THREAD:
// OBSOLETE       target_terminal_ours_for_output ();
// OBSOLETE       if (name == current_thread)
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered ("\nCurrent thread %d died", element->mid);
// OBSOLETE 	  current_thread = MACH_PORT_NULL;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	printf_filtered ("\nThread %d died", element->mid);
// OBSOLETE 
// OBSOLETE       break;
// OBSOLETE 
// OBSOLETE     case MACH_TYPE_TASK:
// OBSOLETE       target_terminal_ours_for_output ();
// OBSOLETE       if (name != inferior_task)
// OBSOLETE 	printf_filtered ("Task %d died, but it was not the selected task",
// OBSOLETE 			 element->mid);
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered ("Current task %d died", element->mid);
// OBSOLETE 
// OBSOLETE 	  mach_port_destroy (mach_task_self (), name);
// OBSOLETE 	  inferior_task = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE 	  if (notify_chain)
// OBSOLETE 	    warning ("There were still unreceived dead_name_notifications???");
// OBSOLETE 
// OBSOLETE 	  /* Destroy the old notifications */
// OBSOLETE 	  setup_notify_port (0);
// OBSOLETE 
// OBSOLETE 	}
// OBSOLETE       break;
// OBSOLETE 
// OBSOLETE     default:
// OBSOLETE       error ("Unregistered dead_name 0x%x notification received. Type is %d, mid is 0x%x",
// OBSOLETE 	     name, element->type, element->mid);
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_msg_accepted (mach_port_t notify, mach_port_t name)
// OBSOLETE {
// OBSOLETE   warning ("do_mach_notify_msg_accepted : notify %x, name %x",
// OBSOLETE 	   notify, name);
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_no_senders (mach_port_t notify, mach_port_mscount_t mscount)
// OBSOLETE {
// OBSOLETE   warning ("do_mach_notify_no_senders : notify %x, mscount %x",
// OBSOLETE 	   notify, mscount);
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_port_deleted (mach_port_t notify, mach_port_t name)
// OBSOLETE {
// OBSOLETE   warning ("do_mach_notify_port_deleted : notify %x, name %x",
// OBSOLETE 	   notify, name);
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_port_destroyed (mach_port_t notify, mach_port_t rights)
// OBSOLETE {
// OBSOLETE   warning ("do_mach_notify_port_destroyed : notify %x, rights %x",
// OBSOLETE 	   notify, rights);
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE kern_return_t
// OBSOLETE do_mach_notify_send_once (mach_port_t notify)
// OBSOLETE {
// OBSOLETE #ifdef DUMP_SYSCALL
// OBSOLETE   /* MANY of these are generated. */
// OBSOLETE   warning ("do_mach_notify_send_once : notify %x",
// OBSOLETE 	   notify);
// OBSOLETE #endif
// OBSOLETE   return KERN_SUCCESS;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Kills the inferior. It's gone when you call this */
// OBSOLETE static void
// OBSOLETE kill_inferior_fast (void)
// OBSOLETE {
// OBSOLETE   WAITTYPE w;
// OBSOLETE 
// OBSOLETE   if (PIDGET (inferior_ptid) == 0 || PIDGET (inferior_ptid) == 1)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   /* kill() it, since the Unix server does not otherwise notice when
// OBSOLETE    * killed with task_terminate().
// OBSOLETE    */
// OBSOLETE   if (PIDGET (inferior_ptid) > 0)
// OBSOLETE     kill (PIDGET (inferior_ptid), SIGKILL);
// OBSOLETE 
// OBSOLETE   /* It's propably terminate already */
// OBSOLETE   (void) task_terminate (inferior_task);
// OBSOLETE 
// OBSOLETE   inferior_task = MACH_PORT_NULL;
// OBSOLETE   current_thread = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE   wait3 (&w, WNOHANG, 0);
// OBSOLETE 
// OBSOLETE   setup_notify_port (0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_kill_inferior (void)
// OBSOLETE {
// OBSOLETE   kill_inferior_fast ();
// OBSOLETE   target_mourn_inferior ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Clean up after the inferior dies.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_mourn_inferior (void)
// OBSOLETE {
// OBSOLETE   unpush_target (&m3_ops);
// OBSOLETE   generic_mourn_inferior ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Fork an inferior process, and start debugging it.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_create_inferior (char *exec_file, char *allargs, char **env)
// OBSOLETE {
// OBSOLETE   fork_inferior (exec_file, allargs, env, m3_trace_me, m3_trace_him, NULL, NULL);
// OBSOLETE   /* We are at the first instruction we care about.  */
// OBSOLETE   /* Pedal to the metal... */
// OBSOLETE   proceed ((CORE_ADDR) -1, 0, 0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Mark our target-struct as eligible for stray "run" and "attach"
// OBSOLETE    commands.  */
// OBSOLETE static int
// OBSOLETE m3_can_run (void)
// OBSOLETE {
// OBSOLETE   return 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Mach 3.0 does not need ptrace for anything
// OBSOLETE  * Make sure nobody uses it on mach.
// OBSOLETE  */
// OBSOLETE ptrace (int a, int b, int c, int d)
// OBSOLETE {
// OBSOLETE   error ("Lose, Lose! Somebody called ptrace\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Resume execution of the inferior process.
// OBSOLETE    If STEP is nonzero, single-step it.
// OBSOLETE    If SIGNAL is nonzero, give it that signal.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m3_resume (ptid_t ptid, int step, enum target_signal signal)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (step)
// OBSOLETE     {
// OBSOLETE       thread_basic_info_data_t th_info;
// OBSOLETE       unsigned int infoCnt = THREAD_BASIC_INFO_COUNT;
// OBSOLETE 
// OBSOLETE       /* There is no point in single stepping when current_thread
// OBSOLETE        * is dead.
// OBSOLETE        */
// OBSOLETE       if (!MACH_PORT_VALID (current_thread))
// OBSOLETE 	error ("No thread selected; can not single step");
// OBSOLETE 
// OBSOLETE       /* If current_thread is suspended, tracing it would never return.
// OBSOLETE        */
// OBSOLETE       ret = thread_info (current_thread,
// OBSOLETE 			 THREAD_BASIC_INFO,
// OBSOLETE 			 (thread_info_t) & th_info,
// OBSOLETE 			 &infoCnt);
// OBSOLETE       CHK ("child_resume: can't get thread info", ret);
// OBSOLETE 
// OBSOLETE       if (th_info.suspend_count)
// OBSOLETE 	error ("Can't trace a suspended thread. Use \"thread resume\" command to resume it");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   vm_read_cache_valid = FALSE;
// OBSOLETE 
// OBSOLETE   if (signal && PIDGET (inferior_ptid) > 0)	/* Do not signal, if attached by MID */
// OBSOLETE     kill (PIDGET (inferior_ptid), target_signal_to_host (signal));
// OBSOLETE 
// OBSOLETE   if (step)
// OBSOLETE     {
// OBSOLETE       suspend_all_threads (0);
// OBSOLETE 
// OBSOLETE       setup_single_step (current_thread, TRUE);
// OBSOLETE 
// OBSOLETE       ret = thread_resume (current_thread);
// OBSOLETE       CHK ("thread_resume", ret);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = task_resume (inferior_task);
// OBSOLETE   if (ret == KERN_FAILURE)
// OBSOLETE     warning ("Task was not suspended");
// OBSOLETE   else
// OBSOLETE     CHK ("Resuming task", ret);
// OBSOLETE 
// OBSOLETE   /* HACK HACK This is needed by the multiserver system HACK HACK */
// OBSOLETE   while ((ret = task_resume (inferior_task)) == KERN_SUCCESS)
// OBSOLETE     /* make sure it really runs */ ;
// OBSOLETE   /* HACK HACK This is needed by the multiserver system HACK HACK */
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef ATTACH_DETACH
// OBSOLETE 
// OBSOLETE /* Start debugging the process with the given task */
// OBSOLETE void
// OBSOLETE task_attach (task_t tid)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   inferior_task = tid;
// OBSOLETE 
// OBSOLETE   ret = task_suspend (inferior_task);
// OBSOLETE   CHK ("task_attach: task_suspend", ret);
// OBSOLETE 
// OBSOLETE   must_suspend_thread = 0;
// OBSOLETE 
// OBSOLETE   setup_notify_port (1);
// OBSOLETE 
// OBSOLETE   request_notify (inferior_task, MACH_NOTIFY_DEAD_NAME, MACH_TYPE_TASK);
// OBSOLETE 
// OBSOLETE   setup_exception_port ();
// OBSOLETE 
// OBSOLETE   emulator_present = have_emulator_p (inferior_task);
// OBSOLETE 
// OBSOLETE   attach_flag = 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Well, we can call error also here and leave the
// OBSOLETE  * target stack inconsistent. Sigh.
// OBSOLETE  * Fix this sometime (the only way to fail here is that
// OBSOLETE  * the task has no threads at all, which is rare, but
// OBSOLETE  * possible; or if the target task has died, which is also
// OBSOLETE  * possible, but unlikely, since it has been suspended.
// OBSOLETE  * (Someone must have killed it))
// OBSOLETE  */
// OBSOLETE void
// OBSOLETE attach_to_thread (void)
// OBSOLETE {
// OBSOLETE   if (select_thread (inferior_task, 0, 1) != KERN_SUCCESS)
// OBSOLETE     error ("Could not select any threads to attach to");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE mid_attach (int mid)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   ret = machid_mach_port (mid_server, mid_auth, mid, &inferior_task);
// OBSOLETE   CHK ("mid_attach: machid_mach_port", ret);
// OBSOLETE 
// OBSOLETE   task_attach (inferior_task);
// OBSOLETE 
// OBSOLETE   return mid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* 
// OBSOLETE  * Start debugging the process whose unix process-id is PID.
// OBSOLETE  * A negative "pid" value is legal and signifies a mach_id not a unix pid.
// OBSOLETE  *
// OBSOLETE  * Prevent (possible unwanted) dangerous operations by enabled users
// OBSOLETE  * like "atta 0" or "atta foo" (equal to the previous :-) and
// OBSOLETE  * "atta pidself". Anyway, the latter is allowed by specifying a MID.
// OBSOLETE  */
// OBSOLETE static int
// OBSOLETE m3_do_attach (int pid)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   if (pid == 0)
// OBSOLETE     error ("MID=0, Debugging the master unix server does not compute");
// OBSOLETE 
// OBSOLETE   /* Foo. This assumes gdb has a unix pid */
// OBSOLETE   if (pid == getpid ())
// OBSOLETE     error ("I will debug myself only by mid. (Gdb would suspend itself!)");
// OBSOLETE 
// OBSOLETE   if (pid < 0)
// OBSOLETE     {
// OBSOLETE       mid_attach (-(pid));
// OBSOLETE 
// OBSOLETE       /* inferior_ptid will be NEGATIVE! */
// OBSOLETE       inferior_ptid = pid_to_ptid (pid);
// OBSOLETE 
// OBSOLETE       return PIDGET (inferior_ptid);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   inferior_task = task_by_pid (pid);
// OBSOLETE   if (!MACH_PORT_VALID (inferior_task))
// OBSOLETE     error ("Cannot map Unix pid %d to Mach task port", pid);
// OBSOLETE 
// OBSOLETE   task_attach (inferior_task);
// OBSOLETE 
// OBSOLETE   inferior_ptid = pid_to_ptid (pid);
// OBSOLETE 
// OBSOLETE   return PIDGET (inferior_ptid);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Attach to process PID, then initialize for debugging it
// OBSOLETE    and wait for the trace-trap that results from attaching.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_attach (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   char *exec_file;
// OBSOLETE   int pid;
// OBSOLETE 
// OBSOLETE   if (!args)
// OBSOLETE     error_no_arg ("process-id to attach");
// OBSOLETE 
// OBSOLETE   pid = atoi (args);
// OBSOLETE 
// OBSOLETE   if (pid == getpid ())		/* Trying to masturbate? */
// OBSOLETE     error ("I refuse to debug myself!");
// OBSOLETE 
// OBSOLETE   if (from_tty)
// OBSOLETE     {
// OBSOLETE       exec_file = (char *) get_exec_file (0);
// OBSOLETE 
// OBSOLETE       if (exec_file)
// OBSOLETE 	printf_unfiltered ("Attaching to program `%s', %s\n", exec_file,
// OBSOLETE 	                   target_pid_to_str (pid_to_ptid (pid)));
// OBSOLETE       else
// OBSOLETE 	printf_unfiltered ("Attaching to %s\n",
// OBSOLETE 	                   target_pid_to_str (pid_to_ptid (pid)));
// OBSOLETE 
// OBSOLETE       gdb_flush (gdb_stdout);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   m3_do_attach (pid_to_ptid (pid));
// OBSOLETE   inferior_ptid = pid_to_ptid (pid);
// OBSOLETE   push_target (&m3_ops);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE deallocate_inferior_ports (void)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE   thread_array_t thread_list;
// OBSOLETE   int thread_count, index;
// OBSOLETE 
// OBSOLETE   if (!MACH_PORT_VALID (inferior_task))
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   ret = task_threads (inferior_task, &thread_list, &thread_count);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       warning ("deallocate_inferior_ports: task_threads",
// OBSOLETE 	       mach_error_string (ret));
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Get rid of send rights to task threads */
// OBSOLETE   for (index = 0; index < thread_count; index++)
// OBSOLETE     {
// OBSOLETE       int rights;
// OBSOLETE       ret = mach_port_get_refs (mach_task_self (),
// OBSOLETE 				thread_list[index],
// OBSOLETE 				MACH_PORT_RIGHT_SEND,
// OBSOLETE 				&rights);
// OBSOLETE       CHK ("deallocate_inferior_ports: get refs", ret);
// OBSOLETE 
// OBSOLETE       if (rights > 0)
// OBSOLETE 	{
// OBSOLETE 	  ret = mach_port_mod_refs (mach_task_self (),
// OBSOLETE 				    thread_list[index],
// OBSOLETE 				    MACH_PORT_RIGHT_SEND,
// OBSOLETE 				    -rights);
// OBSOLETE 	  CHK ("deallocate_inferior_ports: mod refs", ret);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = mach_port_mod_refs (mach_task_self (),
// OBSOLETE 			    inferior_exception_port,
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    -1);
// OBSOLETE   CHK ("deallocate_inferior_ports: cannot get rid of exception port", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_deallocate (mach_task_self (),
// OBSOLETE 			      inferior_task);
// OBSOLETE   CHK ("deallocate_task_port: deallocating inferior_task", ret);
// OBSOLETE 
// OBSOLETE   current_thread = MACH_PORT_NULL;
// OBSOLETE   inferior_task = MACH_PORT_NULL;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Stop debugging the process whose number is PID
// OBSOLETE    and continue it with signal number SIGNAL.
// OBSOLETE    SIGNAL = 0 means just continue it.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_do_detach (int signal)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   MACH_ERROR_NO_INFERIOR;
// OBSOLETE 
// OBSOLETE   if (current_thread != MACH_PORT_NULL)
// OBSOLETE     {
// OBSOLETE       /* Store the gdb's view of the thread we are deselecting
// OBSOLETE        * before we detach.
// OBSOLETE        * @@ I am really not sure if this is ever needeed.
// OBSOLETE        */
// OBSOLETE       target_prepare_to_store ();
// OBSOLETE       target_store_registers (-1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   ret = task_set_special_port (inferior_task,
// OBSOLETE 			       TASK_EXCEPTION_PORT,
// OBSOLETE 			       inferior_old_exception_port);
// OBSOLETE   CHK ("task_set_special_port", ret);
// OBSOLETE 
// OBSOLETE   /* Discard all requested notifications */
// OBSOLETE   setup_notify_port (0);
// OBSOLETE 
// OBSOLETE   if (remove_breakpoints ())
// OBSOLETE     warning ("Could not remove breakpoints when detaching");
// OBSOLETE 
// OBSOLETE   if (signal && PIDGET (inferior_ptid) > 0)
// OBSOLETE     kill (PIDGET (inferior_ptid), signal);
// OBSOLETE 
// OBSOLETE   /* the task might be dead by now */
// OBSOLETE   (void) task_resume (inferior_task);
// OBSOLETE 
// OBSOLETE   deallocate_inferior_ports ();
// OBSOLETE 
// OBSOLETE   attach_flag = 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Take a program previously attached to and detaches it.
// OBSOLETE    The program resumes execution and will no longer stop
// OBSOLETE    on signals, etc.  We'd better not have left any breakpoints
// OBSOLETE    in the program or it'll die when it hits one.  For this
// OBSOLETE    to work, it may be necessary for the process to have been
// OBSOLETE    previously attached.  It *might* work if the program was
// OBSOLETE    started via fork.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_detach (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int siggnal = 0;
// OBSOLETE 
// OBSOLETE   if (from_tty)
// OBSOLETE     {
// OBSOLETE       char *exec_file = get_exec_file (0);
// OBSOLETE       if (exec_file == 0)
// OBSOLETE 	exec_file = "";
// OBSOLETE       printf_unfiltered ("Detaching from program: %s %s\n",
// OBSOLETE 			 exec_file, target_pid_to_str (inferior_ptid));
// OBSOLETE       gdb_flush (gdb_stdout);
// OBSOLETE     }
// OBSOLETE   if (args)
// OBSOLETE     siggnal = atoi (args);
// OBSOLETE 
// OBSOLETE   m3_do_detach (siggnal);
// OBSOLETE   inferior_ptid = null_ptid;
// OBSOLETE   unpush_target (&m3_ops);	/* Pop out of handling an inferior */
// OBSOLETE }
// OBSOLETE #endif /* ATTACH_DETACH */
// OBSOLETE 
// OBSOLETE /* Get ready to modify the registers array.  On machines which store
// OBSOLETE    individual registers, this doesn't need to do anything.  On machines
// OBSOLETE    which store all the registers in one fell swoop, this makes sure
// OBSOLETE    that registers contains all the registers from the program being
// OBSOLETE    debugged.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_prepare_to_store (void)
// OBSOLETE {
// OBSOLETE #ifdef CHILD_PREPARE_TO_STORE
// OBSOLETE   CHILD_PREPARE_TO_STORE ();
// OBSOLETE #endif
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Print status information about what we're accessing.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_files_info (struct target_ops *ignore)
// OBSOLETE {
// OBSOLETE   /* FIXME: should print MID and all that crap.  */
// OBSOLETE   printf_unfiltered ("\tUsing the running image of %s %s.\n",
// OBSOLETE       attach_flag ? "attached" : "child", target_pid_to_str (inferior_ptid));
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_open (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   error ("Use the \"run\" command to start a Unix child process.");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef DUMP_SYSCALL
// OBSOLETE #define STR(x) #x
// OBSOLETE 
// OBSOLETE char *bsd1_names[] =
// OBSOLETE {
// OBSOLETE   "execve",
// OBSOLETE   "fork",
// OBSOLETE   "take_signal",
// OBSOLETE   "sigreturn",
// OBSOLETE   "getrusage",
// OBSOLETE   "chdir",
// OBSOLETE   "chroot",
// OBSOLETE   "open",
// OBSOLETE   "creat",
// OBSOLETE   "mknod",
// OBSOLETE   "link",
// OBSOLETE   "symlink",
// OBSOLETE   "unlink",
// OBSOLETE   "access",
// OBSOLETE   "stat",
// OBSOLETE   "readlink",
// OBSOLETE   "chmod",
// OBSOLETE   "chown",
// OBSOLETE   "utimes",
// OBSOLETE   "truncate",
// OBSOLETE   "rename",
// OBSOLETE   "mkdir",
// OBSOLETE   "rmdir",
// OBSOLETE   "xutimes",
// OBSOLETE   "mount",
// OBSOLETE   "umount",
// OBSOLETE   "acct",
// OBSOLETE   "setquota",
// OBSOLETE   "write_short",
// OBSOLETE   "write_long",
// OBSOLETE   "send_short",
// OBSOLETE   "send_long",
// OBSOLETE   "sendto_short",
// OBSOLETE   "sendto_long",
// OBSOLETE   "select",
// OBSOLETE   "task_by_pid",
// OBSOLETE   "recvfrom_short",
// OBSOLETE   "recvfrom_long",
// OBSOLETE   "setgroups",
// OBSOLETE   "setrlimit",
// OBSOLETE   "sigvec",
// OBSOLETE   "sigstack",
// OBSOLETE   "settimeofday",
// OBSOLETE   "adjtime",
// OBSOLETE   "setitimer",
// OBSOLETE   "sethostname",
// OBSOLETE   "bind",
// OBSOLETE   "accept",
// OBSOLETE   "connect",
// OBSOLETE   "setsockopt",
// OBSOLETE   "getsockopt",
// OBSOLETE   "getsockname",
// OBSOLETE   "getpeername",
// OBSOLETE   "init_process",
// OBSOLETE   "table_set",
// OBSOLETE   "table_get",
// OBSOLETE   "pioctl",
// OBSOLETE   "emulator_error",
// OBSOLETE   "readwrite",
// OBSOLETE   "share_wakeup",
// OBSOLETE   0,
// OBSOLETE   "maprw_request_it",
// OBSOLETE   "maprw_release_it",
// OBSOLETE   "maprw_remap",
// OBSOLETE   "pid_by_task",
// OBSOLETE };
// OBSOLETE 
// OBSOLETE int bsd1_nnames = sizeof (bsd1_names) / sizeof (bsd1_names[0]);
// OBSOLETE 
// OBSOLETE char *
// OBSOLETE name_str (int name, char *buf)
// OBSOLETE {
// OBSOLETE   switch (name)
// OBSOLETE     {
// OBSOLETE     case MACH_MSG_TYPE_BOOLEAN:
// OBSOLETE       return "boolean";
// OBSOLETE     case MACH_MSG_TYPE_INTEGER_16:
// OBSOLETE       return "short";
// OBSOLETE     case MACH_MSG_TYPE_INTEGER_32:
// OBSOLETE       return "long";
// OBSOLETE     case MACH_MSG_TYPE_CHAR:
// OBSOLETE       return "char";
// OBSOLETE     case MACH_MSG_TYPE_BYTE:
// OBSOLETE       return "byte";
// OBSOLETE     case MACH_MSG_TYPE_REAL:
// OBSOLETE       return "real";
// OBSOLETE     case MACH_MSG_TYPE_STRING:
// OBSOLETE       return "string";
// OBSOLETE     default:
// OBSOLETE       sprintf (buf, "%d", name);
// OBSOLETE       return buf;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE char *
// OBSOLETE id_str (int id, char *buf)
// OBSOLETE {
// OBSOLETE   char *p;
// OBSOLETE   if (id >= 101000 && id < 101000 + bsd1_nnames)
// OBSOLETE     {
// OBSOLETE       if (p = bsd1_names[id - 101000])
// OBSOLETE 	return p;
// OBSOLETE     }
// OBSOLETE   if (id == 102000)
// OBSOLETE     return "psignal_retry";
// OBSOLETE   if (id == 100000)
// OBSOLETE     return "syscall";
// OBSOLETE   sprintf (buf, "%d", id);
// OBSOLETE   return buf;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE print_msg (mach_msg_header_t *mp)
// OBSOLETE {
// OBSOLETE   char *fmt_x = "%20s : 0x%08x\n";
// OBSOLETE   char *fmt_d = "%20s : %10d\n";
// OBSOLETE   char *fmt_s = "%20s : %s\n";
// OBSOLETE   char buf[100];
// OBSOLETE 
// OBSOLETE   puts_filtered ("\n");
// OBSOLETE #define pr(fmt,h,x) printf_filtered(fmt,STR(x),(h).x)
// OBSOLETE   pr (fmt_x, (*mp), msgh_bits);
// OBSOLETE   pr (fmt_d, (*mp), msgh_size);
// OBSOLETE   pr (fmt_x, (*mp), msgh_remote_port);
// OBSOLETE   pr (fmt_x, (*mp), msgh_local_port);
// OBSOLETE   pr (fmt_d, (*mp), msgh_kind);
// OBSOLETE   printf_filtered (fmt_s, STR (msgh_id), id_str (mp->msgh_id, buf));
// OBSOLETE 
// OBSOLETE   if (debug_level > 1)
// OBSOLETE     {
// OBSOLETE       char *p, *ep, *dp;
// OBSOLETE       int plen;
// OBSOLETE       p = (char *) mp;
// OBSOLETE       ep = p + mp->msgh_size;
// OBSOLETE       p += sizeof (*mp);
// OBSOLETE       for (; p < ep; p += plen)
// OBSOLETE 	{
// OBSOLETE 	  mach_msg_type_t *tp;
// OBSOLETE 	  mach_msg_type_long_t *tlp;
// OBSOLETE 	  int name, size, number;
// OBSOLETE 	  tp = (mach_msg_type_t *) p;
// OBSOLETE 	  if (tp->msgt_longform)
// OBSOLETE 	    {
// OBSOLETE 	      tlp = (mach_msg_type_long_t *) tp;
// OBSOLETE 	      name = tlp->msgtl_name;
// OBSOLETE 	      size = tlp->msgtl_size;
// OBSOLETE 	      number = tlp->msgtl_number;
// OBSOLETE 	      plen = sizeof (*tlp);
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      name = tp->msgt_name;
// OBSOLETE 	      size = tp->msgt_size;
// OBSOLETE 	      number = tp->msgt_number;
// OBSOLETE 	      plen = sizeof (*tp);
// OBSOLETE 	    }
// OBSOLETE 	  printf_filtered ("name=%-16s size=%2d number=%7d inline=%d long=%d deal=%d\n",
// OBSOLETE 			name_str (name, buf), size, number, tp->msgt_inline,
// OBSOLETE 			   tp->msgt_longform, tp->msgt_deallocate);
// OBSOLETE 	  dp = p + plen;
// OBSOLETE 	  if (tp->msgt_inline)
// OBSOLETE 	    {
// OBSOLETE 	      int l;
// OBSOLETE 	      l = size * number / 8;
// OBSOLETE 	      l = (l + sizeof (long) - 1) & ~((sizeof (long)) - 1);
// OBSOLETE 	      plen += l;
// OBSOLETE 	      print_data (dp, size, number);
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      plen += sizeof (int *);
// OBSOLETE 	    }
// OBSOLETE 	  printf_filtered ("plen=%d\n", plen);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE print_data (char *p, int size, int number)
// OBSOLETE {
// OBSOLETE   int *ip;
// OBSOLETE   short *sp;
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   switch (size)
// OBSOLETE     {
// OBSOLETE     case 8:
// OBSOLETE       for (i = 0; i < number; i++)
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered (" %02x", p[i]);
// OBSOLETE 	}
// OBSOLETE       break;
// OBSOLETE     case 16:
// OBSOLETE       sp = (short *) p;
// OBSOLETE       for (i = 0; i < number; i++)
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered (" %04x", sp[i]);
// OBSOLETE 	}
// OBSOLETE       break;
// OBSOLETE     case 32:
// OBSOLETE       ip = (int *) p;
// OBSOLETE       for (i = 0; i < number; i++)
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered (" %08x", ip[i]);
// OBSOLETE 	}
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE   puts_filtered ("\n");
// OBSOLETE }
// OBSOLETE #endif /* DUMP_SYSCALL */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m3_stop (void)
// OBSOLETE {
// OBSOLETE   error ("to_stop target function not implemented");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE m3_pid_to_exec_file (int pid)
// OBSOLETE {
// OBSOLETE   error ("to_pid_to_exec_file target function not implemented");
// OBSOLETE   return NULL;			/* To keep all compilers happy. */
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE init_m3_ops (void)
// OBSOLETE {
// OBSOLETE   m3_ops.to_shortname = "mach";
// OBSOLETE   m3_ops.to_longname = "Mach child process";
// OBSOLETE   m3_ops.to_doc = "Mach child process (started by the \"run\" command).";
// OBSOLETE   m3_ops.to_open = m3_open;
// OBSOLETE   m3_ops.to_attach = m3_attach;
// OBSOLETE   m3_ops.to_detach = m3_detach;
// OBSOLETE   m3_ops.to_resume = m3_resume;
// OBSOLETE   m3_ops.to_wait = mach_really_wait;
// OBSOLETE   m3_ops.to_fetch_registers = fetch_inferior_registers;
// OBSOLETE   m3_ops.to_store_registers = store_inferior_registers;
// OBSOLETE   m3_ops.to_prepare_to_store = m3_prepare_to_store;
// OBSOLETE   m3_ops.to_xfer_memory = m3_xfer_memory;
// OBSOLETE   m3_ops.to_files_info = m3_files_info;
// OBSOLETE   m3_ops.to_insert_breakpoint = memory_insert_breakpoint;
// OBSOLETE   m3_ops.to_remove_breakpoint = memory_remove_breakpoint;
// OBSOLETE   m3_ops.to_terminal_init = terminal_init_inferior;
// OBSOLETE   m3_ops.to_terminal_inferior = terminal_inferior;
// OBSOLETE   m3_ops.to_terminal_ours_for_output = terminal_ours_for_output;
// OBSOLETE   m3_ops.to_terminal_save_ours = terminal_save_ours;
// OBSOLETE   m3_ops.to_terminal_ours = terminal_ours;
// OBSOLETE   m3_ops.to_terminal_info = child_terminal_info;
// OBSOLETE   m3_ops.to_kill = m3_kill_inferior;
// OBSOLETE   m3_ops.to_create_inferior = m3_create_inferior;
// OBSOLETE   m3_ops.to_mourn_inferior = m3_mourn_inferior;
// OBSOLETE   m3_ops.to_can_run = m3_can_run;
// OBSOLETE   m3_ops.to_stop = m3_stop;
// OBSOLETE   m3_ops.to_pid_to_exec_file = m3_pid_to_exec_file;
// OBSOLETE   m3_ops.to_stratum = process_stratum;
// OBSOLETE   m3_ops.to_has_all_memory = 1;
// OBSOLETE   m3_ops.to_has_memory = 1;
// OBSOLETE   m3_ops.to_has_stack = 1;
// OBSOLETE   m3_ops.to_has_registers = 1;
// OBSOLETE   m3_ops.to_has_execution = 1;
// OBSOLETE   m3_ops.to_magic = OPS_MAGIC;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_m3_nat (void)
// OBSOLETE {
// OBSOLETE   kern_return_t ret;
// OBSOLETE 
// OBSOLETE   init_m3_ops ();
// OBSOLETE   add_target (&m3_ops);
// OBSOLETE 
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_PORT_SET,
// OBSOLETE 			    &inferior_wait_port_set);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     internal_error (__FILE__, __LINE__,
// OBSOLETE 		    "initial port set %s", mach_error_string (ret));
// OBSOLETE 
// OBSOLETE   /* mach_really_wait now waits for this */
// OBSOLETE   currently_waiting_for = inferior_wait_port_set;
// OBSOLETE 
// OBSOLETE   ret = netname_look_up (name_server_port, hostname, "MachID", &mid_server);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     {
// OBSOLETE       mid_server = MACH_PORT_NULL;
// OBSOLETE 
// OBSOLETE       warning ("initialize machid: netname_lookup_up(MachID) : %s",
// OBSOLETE 	       mach_error_string (ret));
// OBSOLETE       warning ("Some (most?) features disabled...");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   mid_auth = mach_privileged_host_port ();
// OBSOLETE   if (mid_auth == MACH_PORT_NULL)
// OBSOLETE     mid_auth = mach_task_self ();
// OBSOLETE 
// OBSOLETE   obstack_init (port_chain_obstack);
// OBSOLETE 
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    &thread_exception_port);
// OBSOLETE   CHK ("Creating thread_exception_port for single stepping", ret);
// OBSOLETE 
// OBSOLETE   ret = mach_port_insert_right (mach_task_self (),
// OBSOLETE 				thread_exception_port,
// OBSOLETE 				thread_exception_port,
// OBSOLETE 				MACH_MSG_TYPE_MAKE_SEND);
// OBSOLETE   CHK ("Inserting send right to thread_exception_port", ret);
// OBSOLETE 
// OBSOLETE   /* Allocate message port */
// OBSOLETE   ret = mach_port_allocate (mach_task_self (),
// OBSOLETE 			    MACH_PORT_RIGHT_RECEIVE,
// OBSOLETE 			    &our_message_port);
// OBSOLETE   if (ret != KERN_SUCCESS)
// OBSOLETE     warning ("Creating message port %s", mach_error_string (ret));
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       char buf[MAX_NAME_LEN];
// OBSOLETE       ret = mach_port_move_member (mach_task_self (),
// OBSOLETE 				   our_message_port,
// OBSOLETE 				   inferior_wait_port_set);
// OBSOLETE       if (ret != KERN_SUCCESS)
// OBSOLETE 	warning ("message move member %s", mach_error_string (ret));
// OBSOLETE 
// OBSOLETE 
// OBSOLETE       /* @@@@ No way to change message port name currently */
// OBSOLETE       /* Foo. This assumes gdb has a unix pid */
// OBSOLETE       sprintf (buf, "gdb-%d", getpid ());
// OBSOLETE       gdb_register_port (buf, our_message_port);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Heap for thread commands */
// OBSOLETE   obstack_init (cproc_obstack);
// OBSOLETE 
// OBSOLETE   add_mach_specific_commands ();
// OBSOLETE }
