/* Common definitions for remote server for GDB.
   Copyright (C) 1993-2013 Free Software Foundation, Inc.

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
#include "build-gnulib-gdbserver/config.h"

#ifdef __MINGW32CE__
#include "wincecompat.h"
#endif

#include "libiberty.h"
#include "ansidecl.h"
#include "version.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <setjmp.h>

/* For gnulib's PATH_MAX.  */
#include "pathmax.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
/* On some systems such as MinGW, alloca is declared in malloc.h
   (there is no alloca.h).  */
#if HAVE_MALLOC_H
#include <malloc.h>
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

#if !HAVE_DECL_VASPRINTF
extern int vasprintf(char **strp, const char *fmt, va_list ap);
#endif
#if !HAVE_DECL_VSNPRINTF
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif

#ifdef IN_PROCESS_AGENT
#  define PROG "ipa"
#else
#  define PROG "gdbserver"
#endif

/* A type used for binary buffers.  */
typedef unsigned char gdb_byte;

#include "ptid.h"
#include "buffer.h"
#include "xml-utils.h"
#include "gdb_locale.h"

/* FIXME: This should probably be autoconf'd for.  It's an integer type at
   least the size of a (void *).  */
typedef long long CORE_ADDR;

typedef long long LONGEST;
typedef unsigned long long ULONGEST;

#include "regcache.h"
#include "gdb/signals.h"
#include "gdb_signals.h"
#include "target.h"
#include "mem-break.h"
#include "gdbthread.h"
#include "inferiors.h"

struct dll_info
{
  struct inferior_list_entry entry;
  char *name;
  CORE_ADDR base_addr;
};

/* Target-specific functions */

void initialize_low ();

/* From dll.c.  */

extern struct inferior_list all_dlls;
extern int dlls_changed;
extern void clear_dlls (void);

void loaded_dll (const char *name, CORE_ADDR base_addr);
void unloaded_dll (const char *name, CORE_ADDR base_addr);

/* Public variables in server.c */

extern ptid_t cont_thread;
extern ptid_t general_thread;

extern int server_waiting;
extern int debug_threads;
extern int debug_hw_points;
extern int pass_signals[];
extern int program_signals[];
extern int program_signals_p;

extern jmp_buf toplevel;

extern int disable_packet_vCont;
extern int disable_packet_Tthread;
extern int disable_packet_qC;
extern int disable_packet_qfThreadInfo;

extern int run_once;
extern int multi_process;
extern int non_stop;

extern int disable_randomization;

#if USE_WIN32API
#include <winsock2.h>
typedef SOCKET gdb_fildes_t;
#else
typedef int gdb_fildes_t;
#endif

/* Functions from event-loop.c.  */
typedef void *gdb_client_data;
typedef int (handler_func) (int, gdb_client_data);
typedef int (callback_handler_func) (gdb_client_data);

extern void delete_file_handler (gdb_fildes_t fd);
extern void add_file_handler (gdb_fildes_t fd, handler_func *proc,
			      gdb_client_data client_data);
extern int append_callback_event (callback_handler_func *proc,
				   gdb_client_data client_data);
extern void delete_callback_event (int id);

extern void start_event_loop (void);
extern void initialize_event_loop (void);

/* Functions from server.c.  */
extern int handle_serial_event (int err, gdb_client_data client_data);
extern int handle_target_event (int err, gdb_client_data client_data);

/* Functions from hostio.c.  */
extern int handle_vFile (char *, int, int *);

/* Functions from hostio-errno.c.  */
extern void hostio_last_error_from_errno (char *own_buf);

#include "remote-utils.h"

#include "common-utils.h"
#include "utils.h"

#include "gdb_assert.h"

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES(N) (((N)-32)/2)

/* Buffer sizes for transferring memory, registers, etc.   Set to a constant
   value to accomodate multiple register formats.  This value must be at least
   as large as the largest register set supported by gdbserver.  */
#define PBUFSIZ 16384

#endif /* SERVER_H */
