/* Common definitions for remote server for GDB.
   Copyright (C) 1993, 1995, 1997, 1998, 1999, 2000, 2002, 2003, 2004, 2005,
   2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SERVER_H
#define SERVER_H

#include "config.h"

#ifdef __MINGW32CE__
#include "wincecompat.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <setjmp.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if !HAVE_DECL_STRERROR
#ifndef strerror
extern char *strerror (int);	/* X3.159-1989  4.11.6.2 */
#endif
#endif

#if !HAVE_DECL_PERROR
#ifndef perror
extern void perror (const char *);
#endif
#endif

#if !HAVE_DECL_MEMMEM
extern void *memmem (const void *, size_t , const void *, size_t);
#endif

#ifndef ATTR_NORETURN
#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7))
#define ATTR_NORETURN __attribute__ ((noreturn))
#else
#define ATTR_NORETURN           /* nothing */
#endif
#endif

#ifndef ATTR_FORMAT
#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 4))
#define ATTR_FORMAT(type, x, y) __attribute__ ((format(type, x, y)))
#else
#define ATTR_FORMAT(type, x, y) /* nothing */
#endif
#endif

#ifndef ATTR_MALLOC
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define ATTR_MALLOC __attribute__ ((__malloc__))
#else
#define ATTR_MALLOC             /* nothing */
#endif
#endif

/* A type used for binary buffers.  */
typedef unsigned char gdb_byte;

/* FIXME: This should probably be autoconf'd for.  It's an integer type at
   least the size of a (void *).  */
typedef long long CORE_ADDR;

typedef unsigned long long ULONGEST;

/* The ptid struct is a collection of the various "ids" necessary
   for identifying the inferior.  This consists of the process id
   (pid), thread id (tid), and other fields necessary for uniquely
   identifying the inferior process/thread being debugged.  When
   manipulating ptids, the constructors, accessors, and predicate
   declared in server.h should be used.  These are as follows:

      ptid_build	- Make a new ptid from a pid, lwp, and tid.
      pid_to_ptid	- Make a new ptid from just a pid.
      ptid_get_pid	- Fetch the pid component of a ptid.
      ptid_get_lwp	- Fetch the lwp component of a ptid.
      ptid_get_tid	- Fetch the tid component of a ptid.
      ptid_equal	- Test to see if two ptids are equal.

   Please do NOT access the struct ptid members directly (except, of
   course, in the implementation of the above ptid manipulation
   functions).  */

struct ptid
  {
    /* Process id */
    int pid;

    /* Lightweight process id */
    long lwp;

    /* Thread id */
    long tid;
  };

typedef struct ptid ptid_t;

/* The -1 ptid, often used to indicate either an error condition or a
   "don't care" condition, i.e, "run all threads".  */
extern ptid_t minus_one_ptid;

/* The null or zero ptid, often used to indicate no process.  */
extern ptid_t null_ptid;

/* Attempt to find and return an existing ptid with the given PID,
   LWP, and TID components.  If none exists, create a new one and
   return that.  */
ptid_t ptid_build (int pid, long lwp, long tid);

/* Create a ptid from just a pid.  */
ptid_t pid_to_ptid (int pid);

/* Fetch the pid (process id) component from a ptid.  */
int ptid_get_pid (ptid_t ptid);

/* Fetch the lwp (lightweight process) component from a ptid.  */
long ptid_get_lwp (ptid_t ptid);

/* Fetch the tid (thread id) component from a ptid.  */
long ptid_get_tid (ptid_t ptid);

/* Compare two ptids to see if they are equal.  */
extern int ptid_equal (ptid_t p1, ptid_t p2);

/* Return true if this ptid represents a process id.  */
extern int ptid_is_pid (ptid_t ptid);

/* Generic information for tracking a list of ``inferiors'' - threads,
   processes, etc.  */
struct inferior_list
{
  struct inferior_list_entry *head;
  struct inferior_list_entry *tail;
};
struct inferior_list_entry
{
  ptid_t id;
  struct inferior_list_entry *next;
};

/* Opaque type for user-visible threads.  */
struct thread_info;

struct dll_info
{
  struct inferior_list_entry entry;
  char *name;
  CORE_ADDR base_addr;
};

struct sym_cache;
struct breakpoint;
struct process_info_private;

struct process_info
{
  struct inferior_list_entry head;

  int attached;

  /* The symbol cache.  */
  struct sym_cache *symbol_cache;

  /* If this flag has been set, assume symbol cache misses are
     failures.  */
  int all_symbols_looked_up;

  /* The list of memory breakpoints.  */
  struct breakpoint *breakpoints;

  /* Private target data.  */
  struct process_info_private *private;
};

/* Return a pointer to the process that corresponds to the current
   thread (current_inferior).  It is an error to call this if there is
   no current thread selected.  */

struct process_info *current_process (void);
struct process_info *get_thread_process (struct thread_info *);

#include "regcache.h"
#include "gdb/signals.h"
#include "gdb_signals.h"
#include "target.h"
#include "mem-break.h"

/* Target-specific functions */

void initialize_low ();

/* From inferiors.c.  */

extern struct inferior_list all_processes;
extern struct inferior_list all_threads;
extern struct inferior_list all_dlls;
extern int dlls_changed;

void initialize_inferiors (void);

void add_inferior_to_list (struct inferior_list *list,
			   struct inferior_list_entry *new_inferior);
void for_each_inferior (struct inferior_list *list,
			void (*action) (struct inferior_list_entry *));

extern struct thread_info *current_inferior;
void remove_inferior (struct inferior_list *list,
		      struct inferior_list_entry *entry);
void remove_thread (struct thread_info *thread);
void add_thread (ptid_t ptid, void *target_data);

struct process_info *add_process (int pid, int attached);
void remove_process (struct process_info *process);
struct process_info *find_process_pid (int pid);
int have_started_inferiors_p (void);
int have_attached_inferiors_p (void);

struct thread_info *find_thread_ptid (ptid_t ptid);

ptid_t thread_id_to_gdb_id (ptid_t);
ptid_t thread_to_gdb_id (struct thread_info *);
ptid_t gdb_id_to_thread_id (ptid_t);
struct thread_info *gdb_id_to_thread (unsigned int);
void clear_inferiors (void);
struct inferior_list_entry *find_inferior
     (struct inferior_list *,
      int (*func) (struct inferior_list_entry *,
		   void *),
      void *arg);
struct inferior_list_entry *find_inferior_id (struct inferior_list *list,
					      ptid_t id);
void *inferior_target_data (struct thread_info *);
void set_inferior_target_data (struct thread_info *, void *);
void *inferior_regcache_data (struct thread_info *);
void set_inferior_regcache_data (struct thread_info *, void *);
void add_pid_to_list (struct inferior_list *list, unsigned long pid);
int pull_pid_from_list (struct inferior_list *list, unsigned long pid);

void loaded_dll (const char *name, CORE_ADDR base_addr);
void unloaded_dll (const char *name, CORE_ADDR base_addr);

/* Public variables in server.c */

extern ptid_t cont_thread;
extern ptid_t general_thread;
extern ptid_t step_thread;

extern int server_waiting;
extern int debug_threads;
extern int debug_hw_points;
extern int pass_signals[];

extern jmp_buf toplevel;

extern int disable_packet_vCont;
extern int disable_packet_Tthread;
extern int disable_packet_qC;
extern int disable_packet_qfThreadInfo;

extern int multi_process;
extern int non_stop;

/* Functions from event-loop.c.  */
typedef void *gdb_client_data;
typedef void (handler_func) (int, gdb_client_data);

extern void delete_file_handler (int fd);
extern void add_file_handler (int fd, handler_func *proc,
			      gdb_client_data client_data);

extern void start_event_loop (void);

/* Functions from server.c.  */
extern void handle_serial_event (int err, gdb_client_data client_data);
extern void handle_target_event (int err, gdb_client_data client_data);

extern void push_event (ptid_t ptid, struct target_waitstatus *status);

/* Functions from hostio.c.  */
extern int handle_vFile (char *, int, int *);

/* Functions from hostio-errno.c.  */
extern void hostio_last_error_from_errno (char *own_buf);

/* From remote-utils.c */

extern int remote_debug;
extern int all_symbols_looked_up;
extern int noack_mode;
extern int transport_is_reliable;

ptid_t read_ptid (char *buf, char **obuf);
char *write_ptid (char *buf, ptid_t ptid);

int putpkt (char *buf);
int putpkt_binary (char *buf, int len);
int putpkt_notif (char *buf);
int getpkt (char *buf);
void remote_open (char *name);
void remote_close (void);
void write_ok (char *buf);
void write_enn (char *buf);
void initialize_async_io (void);
void enable_async_io (void);
void disable_async_io (void);
void check_remote_input_interrupt_request (void);
void convert_ascii_to_int (char *from, unsigned char *to, int n);
void convert_int_to_ascii (unsigned char *from, char *to, int n);
void new_thread_notify (int id);
void dead_thread_notify (int id);
void prepare_resume_reply (char *buf, ptid_t ptid,
			   struct target_waitstatus *status);

const char *decode_address_to_semicolon (CORE_ADDR *addrp, const char *start);
void decode_address (CORE_ADDR *addrp, const char *start, int len);
void decode_m_packet (char *from, CORE_ADDR * mem_addr_ptr,
		      unsigned int *len_ptr);
void decode_M_packet (char *from, CORE_ADDR * mem_addr_ptr,
		      unsigned int *len_ptr, unsigned char *to);
int decode_X_packet (char *from, int packet_len, CORE_ADDR * mem_addr_ptr,
		     unsigned int *len_ptr, unsigned char *to);
int decode_xfer_write (char *buf, int packet_len, char **annex,
		       CORE_ADDR *offset, unsigned int *len,
		       unsigned char *data);
int decode_search_memory_packet (const char *buf, int packet_len,
				 CORE_ADDR *start_addrp,
				 CORE_ADDR *search_space_lenp,
				 gdb_byte *pattern, unsigned int *pattern_lenp);

int unhexify (char *bin, const char *hex, int count);
int hexify (char *hex, const char *bin, int count);
int remote_escape_output (const gdb_byte *buffer, int len,
			  gdb_byte *out_buf, int *out_len,
			  int out_maxlen);

void clear_symbol_cache (struct sym_cache **symcache_p);
int look_up_one_symbol (const char *name, CORE_ADDR *addrp);

void monitor_output (const char *msg);

char *xml_escape_text (const char *text);

/* Simple growing buffer.  */

struct buffer
{
  char *buffer;
  size_t buffer_size; /* allocated size */
  size_t used_size; /* actually used size */
};

/* Append DATA of size SIZE to the end of BUFFER.  Grows the buffer to
   accommodate the new data.  */
void buffer_grow (struct buffer *buffer, const char *data, size_t size);

/* Release any memory held by BUFFER.  */
void buffer_free (struct buffer *buffer);

/* Initialize BUFFER.  BUFFER holds no memory afterwards.  */
void buffer_init (struct buffer *buffer);

/* Return a pointer into BUFFER data, effectivelly transfering
   ownership of the buffer memory to the caller.  Calling buffer_free
   afterwards has no effect on the returned data.  */
char* buffer_finish (struct buffer *buffer);

/* Simple printf to BUFFER function.  Current implemented formatters:
   %s - grow an xml escaped text in OBSTACK.  */
void buffer_xml_printf (struct buffer *buffer, const char *format, ...)
  ATTR_FORMAT (printf, 2, 3);;

#define buffer_grow_str(BUFFER,STRING)         \
  buffer_grow (BUFFER, STRING, strlen (STRING))
#define buffer_grow_str0(BUFFER,STRING)                        \
  buffer_grow (BUFFER, STRING, strlen (STRING) + 1)

/* Functions from utils.c */

void *xmalloc (size_t) ATTR_MALLOC;
void *xcalloc (size_t, size_t) ATTR_MALLOC;
char *xstrdup (const char *) ATTR_MALLOC;
void freeargv (char **argv);
void perror_with_name (const char *string);
void error (const char *string,...) ATTR_NORETURN ATTR_FORMAT (printf, 1, 2);
void fatal (const char *string,...) ATTR_NORETURN ATTR_FORMAT (printf, 1, 2);
void warning (const char *string,...) ATTR_FORMAT (printf, 1, 2);
char *paddress (CORE_ADDR addr);

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES(N) (((N)-32)/2)

/* Buffer sizes for transferring memory, registers, etc.   Set to a constant
   value to accomodate multiple register formats.  This value must be at least
   as large as the largest register set supported by gdbserver.  */
#define PBUFSIZ 16384

/* Version information, from version.c.  */
extern const char version[];
extern const char host_name[];

#endif /* SERVER_H */
