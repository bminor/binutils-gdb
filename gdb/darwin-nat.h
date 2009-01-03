/* Common things used by the various darwin files
   Copyright (C) 1995, 1996, 1997, 1999, 2000, 2007, 2008, 2009
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DARWIN_NAT_H__
#define __DARWIN_NAT_H__

#include <mach/mach.h>
#include "gdb_assert.h"

/* Define the threads vector type.  */
DEF_VEC_I (thread_t);

/* Describe the mach exception handling state for a task.  This state is saved
   before being changed and restored when a process is detached.
   For more information on these fields see task_get_exception_ports manual
   page.  */
struct darwin_exception_info
{
  /* Exceptions handled by the port.  */
  exception_mask_t masks[EXC_TYPES_COUNT];

  /* Ports receiving exception messages.  */
  mach_port_t ports[EXC_TYPES_COUNT];

  /* Type of messages sent.  */
  exception_behavior_t behaviors[EXC_TYPES_COUNT];

  /* Type of state to be sent.  */
  thread_state_flavor_t flavors[EXC_TYPES_COUNT];

  /* Number of elements set.  */
  mach_msg_type_number_t count;
};
typedef struct darwin_exception_info darwin_exception_info;

/* Describe an inferior.  */
struct darwin_inferior
{
  /* Inferior PID.  */
  int pid;

  /* Corresponding task port.  */
  task_t task;

  /* Previous port for request notification on task.  */
  mach_port_t prev_not_port;

  /* Initial exception handling.  */
  darwin_exception_info exception_info;

  /* Sorted vector of known threads.  */
  VEC(thread_t) *threads;
};
typedef struct darwin_inferior darwin_inferior;

/* Current inferior.  */
extern darwin_inferior *darwin_inf;

/* Exception port.  */
extern mach_port_t darwin_ex_port;

/* Notification port.  */
extern mach_port_t darwin_not_port;

/* Port set.  */
extern mach_port_t darwin_port_set;

/* A copy of mach_host_self ().  */
extern mach_port_t darwin_host_self;

/* ASSERT_FUNCTION is defined in gdb_assert.h (or not).  */
#ifdef ASSERT_FUNCTION
#define MACH_CHECK_ERROR(ret) \
  mach_check_error (ret, __FILE__, __LINE__, ASSERT_FUNCTION)
#else
#define MACH_CHECK_ERROR(ret) \
  mach_check_error (ret, __FILE__, __LINE__, "??")
#endif

extern void mach_check_error (kern_return_t ret, const char *file,
			      unsigned int line, const char *func);

void darwin_set_sstep (thread_t thread, int enable);

/* This one is called in darwin-nat.c, but needs to be provided by the
   platform specific nat code.  It allows each platform to add platform specific
   stuff to the darwin_ops.  */
extern void darwin_complete_target (struct target_ops *target);

void darwin_check_osabi (darwin_inferior *inf, thread_t thread);

#endif /* __DARWIN_NAT_H__ */
