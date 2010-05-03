/* Tracepoint code for remote server for GDB.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

#include "server.h"
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

static void trace_debug_1 (const char *, ...) ATTR_FORMAT (printf, 1, 2);

static void
trace_debug_1 (const char *fmt, ...)
{
  char buf[1024];
  va_list ap;

  va_start (ap, fmt);
  vsprintf (buf, fmt, ap);
  fprintf (stderr, "gdbserver/tracepoint: %s\n", buf);
  va_end (ap);
}

#define trace_debug(FMT, args...)		\
  do {						\
    if (debug_threads)				\
      trace_debug_1 ((FMT), ##args);		\
  } while (0)

static int
tracepoint_handler (CORE_ADDR address)
{
  trace_debug ("tracepoint_handler: tracepoint at 0x%s hit",
	       paddress (address));
  return 0;
}

/* This enum must exactly match what is documented in
   gdb/doc/agentexpr.texi, including all the numerical values.  */

enum gdb_agent_op
  {
    gdb_agent_op_float = 0x01,
    gdb_agent_op_add = 0x02,
    gdb_agent_op_sub = 0x03,
    gdb_agent_op_mul = 0x04,
    gdb_agent_op_div_signed = 0x05,
    gdb_agent_op_div_unsigned = 0x06,
    gdb_agent_op_rem_signed = 0x07,
    gdb_agent_op_rem_unsigned = 0x08,
    gdb_agent_op_lsh = 0x09,
    gdb_agent_op_rsh_signed = 0x0a,
    gdb_agent_op_rsh_unsigned = 0x0b,
    gdb_agent_op_trace = 0x0c,
    gdb_agent_op_trace_quick = 0x0d,
    gdb_agent_op_log_not = 0x0e,
    gdb_agent_op_bit_and = 0x0f,
    gdb_agent_op_bit_or = 0x10,
    gdb_agent_op_bit_xor = 0x11,
    gdb_agent_op_bit_not = 0x12,
    gdb_agent_op_equal = 0x13,
    gdb_agent_op_less_signed = 0x14,
    gdb_agent_op_less_unsigned = 0x15,
    gdb_agent_op_ext = 0x16,
    gdb_agent_op_ref8 = 0x17,
    gdb_agent_op_ref16 = 0x18,
    gdb_agent_op_ref32 = 0x19,
    gdb_agent_op_ref64 = 0x1a,
    gdb_agent_op_ref_float = 0x1b,
    gdb_agent_op_ref_double = 0x1c,
    gdb_agent_op_ref_long_double = 0x1d,
    gdb_agent_op_l_to_d = 0x1e,
    gdb_agent_op_d_to_l = 0x1f,
    gdb_agent_op_if_goto = 0x20,
    gdb_agent_op_goto = 0x21,
    gdb_agent_op_const8 = 0x22,
    gdb_agent_op_const16 = 0x23,
    gdb_agent_op_const32 = 0x24,
    gdb_agent_op_const64 = 0x25,
    gdb_agent_op_reg = 0x26,
    gdb_agent_op_end = 0x27,
    gdb_agent_op_dup = 0x28,
    gdb_agent_op_pop = 0x29,
    gdb_agent_op_zero_ext = 0x2a,
    gdb_agent_op_swap = 0x2b,
    gdb_agent_op_getv = 0x2c,
    gdb_agent_op_setv = 0x2d,
    gdb_agent_op_tracev = 0x2e,
    gdb_agent_op_trace16 = 0x30,
    gdb_agent_op_last
  };

static const char *gdb_agent_op_names [gdb_agent_op_last] =
  {
    "?undef?",
    "float",
    "add",
    "sub",
    "mul",
    "div_signed",
    "div_unsigned",
    "rem_signed",
    "rem_unsigned",
    "lsh",
    "rsh_signed",
    "rsh_unsigned",
    "trace",
    "trace_quick",
    "log_not",
    "bit_and",
    "bit_or",
    "bit_xor",
    "bit_not",
    "equal",
    "less_signed",
    "less_unsigned",
    "ext",
    "ref8",
    "ref16",
    "ref32",
    "ref64",
    "ref_float",
    "ref_double",
    "ref_long_double",
    "l_to_d",
    "d_to_l",
    "if_goto",
    "goto",
    "const8",
    "const16",
    "const32",
    "const64",
    "reg",
    "end",
    "dup",
    "pop",
    "zero_ext",
    "swap",
    "getv",
    "setv",
    "tracev",
    "?undef?",
    "trace16",
  };

struct agent_expr
{
  int length;

  unsigned char *bytes;
};

/* Base action.  Concrete actions inherit this.  */

struct tracepoint_action
{
  char type;
};

/* An 'M' (collect memory) action.  */
struct collect_memory_action
{
  struct tracepoint_action base;

  ULONGEST addr;
  ULONGEST len;
  int basereg;
};

/* An 'R' (collect registers) action.  */

struct collect_registers_action
{
  struct tracepoint_action base;
};

/* An 'X' (evaluate expression) action.  */

struct eval_expr_action
{
  struct tracepoint_action base;

  struct agent_expr *expr;
};

/* An 'L' (collect static trace data) action.  */
struct collect_static_trace_data_action
{
  struct tracepoint_action base;
};

/* This structure describes a piece of the source-level definition of
   the tracepoint.  The contents are not interpreted by the target,
   but preserved verbatim for uploading upon reconnection.  */

struct source_string
{
  /* The type of string, such as "cond" for a conditional.  */
  char *type;

  /* The source-level string itself.  For the sake of target
     debugging, we store it in plaintext, even though it is always
     transmitted in hex.  */
  char *str;

  /* Link to the next one in the list.  We link them in the order
     received, in case some make up an ordered list of commands or
     some such.  */
  struct source_string *next;
};

struct tracepoint_hit_ctx;

/* The definition of a tracepoint.  */

/* Tracepoints may have multiple locations, each at a different
   address.  This can occur with optimizations, template
   instantiation, etc.  Since the locations may be in different
   scopes, the conditions and actions may be different for each
   location.  Our target version of tracepoints is more like GDB's
   notion of "breakpoint locations", but we have almost nothing that
   is not per-location, so we bother having two kinds of objects.  The
   key consequence is that numbers are not unique, and that it takes
   both number and address to identify a tracepoint uniquely.  */

struct tracepoint
{
  /* The number of the tracepoint, as specified by GDB.  Several
     tracepoint objects here may share a number.  */
  int number;

  /* Address at which the tracepoint is supposed to trigger.  Several
     tracepoints may share an address.  */
  CORE_ADDR address;

  /* True if the tracepoint is currently enabled.  */
  int enabled;

  /* The number of single steps that will be performed after each
     tracepoint hit.  */
  long step_count;

  /* The number of times the tracepoint may be hit before it will
     terminate the entire tracing run.  */
  long pass_count;

  /* Pointer to the agent expression that is the tracepoint's
     conditional, or NULL if the tracepoint is unconditional.  */
  struct agent_expr *cond;

  /* The list of actions to take when the tracepoint triggers.  */
  int numactions;
  struct tracepoint_action **actions;
  /* Same, but in string/packet form.  */
  char **actions_str;

  /* The list of actions to take while in a stepping loop.  */
  int num_step_actions;
  struct tracepoint_action **step_actions;
  /* Same, but in string/packet form.  */
  char **step_actions_str;

  /* Count of the times we've hit this tracepoint during the run.
     Note that while-stepping steps are not counted as "hits".  */
  long hit_count;

  /* The collection of strings that describe the tracepoint as it was
     entered into GDB.  These are not used by the target, but are
     reported back to GDB upon reconnection.  */
  struct source_string *source_strings;

  /* Handle returned by the breakpoint module when we inserted the
     trap.  NULL if we haven't inserted it yet.  */
  void *handle;

  /* Link to the next tracepoint in the list.  */
  struct tracepoint *next;
};

/* Given `while-stepping', a thread may be collecting data for more
   than one tracepoint simultaneously.  On the other hand, the same
   tracepoint with a while-stepping action may be hit by more than one
   thread simultaneously (but not quite, each thread could be handling
   a different step).  Each thread holds a list of these objects,
   representing the current step of each while-stepping action being
   collected.  */

struct wstep_state
{
  struct wstep_state *next;

  /* The tracepoint number.  */
  int tp_number;
  /* The tracepoint's address.  */
  CORE_ADDR tp_address;

  /* The number of the current step in this 'while-stepping'
     action.  */
  long current_step;
};

/* The linked list of all tracepoints.  */

static struct tracepoint *tracepoints;

/* Pointer to the last tracepoint in the list, new tracepoints are
   linked in at the end.  */

static struct tracepoint *last_tracepoint;

/* The first tracepoint to exceed its pass count.  */

static struct tracepoint *stopping_tracepoint;

/* True if the trace buffer is full or otherwise no longer usable.  */

static int trace_buffer_is_full;

/* Enumeration of the different kinds of things that can happen during
   agent expression evaluation.  */

enum eval_result_type
  {
    expr_eval_no_error,
    expr_eval_empty_expression,
    expr_eval_empty_stack,
    expr_eval_stack_overflow,
    expr_eval_stack_underflow,
    expr_eval_unhandled_opcode,
    expr_eval_unrecognized_opcode,
    expr_eval_divide_by_zero,
    expr_eval_invalid_goto
  };

static enum eval_result_type expr_eval_result = expr_eval_no_error;

static const char *eval_result_names[] =
  {
    "terror:in the attic",  /* this should never be reported */
    "terror:empty expression",
    "terror:empty stack",
    "terror:stack overflow",
    "terror:stack underflow",
    "terror:unhandled opcode",
    "terror:unrecognized opcode",
    "terror:divide by zero"
  };

/* The tracepoint in which the error occurred.  */

static struct tracepoint *error_tracepoint;

struct trace_state_variable
{
  /* This is the name of the variable as used in GDB.  The target
     doesn't use the name, but needs to have it for saving and
     reconnection purposes.  */
  char *name;

  /* This number identifies the variable uniquely.  Numbers may be
     assigned either by the target (in the case of builtin variables),
     or by GDB, and are presumed unique during the course of a trace
     experiment.  */
  int number;

  /* The variable's initial value, a 64-bit signed integer always.  */
  LONGEST initial_value;

  /* The variable's value, a 64-bit signed integer always.  */
  LONGEST value;

  /* Pointer to a getter function, used to supply computed values.  */
  LONGEST (*getter) (void);

  /* Link to the next variable.  */
  struct trace_state_variable *next;
};

/* Linked list of all trace state variables.  */

static struct trace_state_variable *trace_state_variables;

/* The results of tracing go into a fixed-size space known as the
   "trace buffer".  Because usage follows a limited number of
   patterns, we manage it ourselves rather than with malloc.  Basic
   rules are that we create only one trace frame at a time, each is
   variable in size, they are never moved once created, and we only
   discard if we are doing a circular buffer, and then only the oldest
   ones.  Each trace frame includes its own size, so we don't need to
   link them together, and the trace frame number is relative to the
   first one, so we don't need to record numbers.  A trace frame also
   records the number of the tracepoint that created it.  The data
   itself is a series of blocks, each introduced by a single character
   and with a defined format.  Each type of block has enough
   type/length info to allow scanners to jump quickly from one block
   to the next without reading each byte in the block.  */

/* Trace buffer management would be simple - advance a free pointer
   from beginning to end, then stop - were it not for the circular
   buffer option, which is a useful way to prevent a trace run from
   stopping prematurely because the buffer filled up.  In the circular
   case, the location of the first trace frame (trace_buffer_start)
   moves as old trace frames are discarded.  Also, since we grow trace
   frames incrementally as actions are performed, we wrap around to
   the beginning of the trace buffer.  This is per-block, so each
   block within a trace frame remains contiguous.  Things get messy
   when the wrapped-around trace frame is the one being discarded; the
   free space ends up in two parts at opposite ends of the buffer.  */

#ifndef ATTR_PACKED
#  if defined(__GNUC__)
#    define ATTR_PACKED __attribute__ ((packed))
#  else
#    define ATTR_PACKED /* nothing */
#  endif
#endif

/* The data collected at a tracepoint hit.  This object should be as
   small as possible, since there may be a great many of them.  We do
   not need to keep a frame number, because they are all sequential
   and there are no deletions; so the Nth frame in the buffer is
   always frame number N.  */

struct traceframe
{
  /* Number of the tracepoint that collected this traceframe.  A value
     of 0 indicates the current end of the trace buffer.  We make this
     a 16-bit field because it's never going to happen that GDB's
     numbering of tracepoints reaches 32,000.  */
  int tpnum : 16;

  /* The size of the data in this trace frame.  We limit this to 32
     bits, even on a 64-bit target, because it's just implausible that
     one is validly going to collect 4 gigabytes of data at a single
     tracepoint hit.  */
  unsigned int data_size : 32;

  /* The base of the trace data, which is contiguous from this point.  */
  unsigned char data[0];

} ATTR_PACKED traceframe_t;

/* The traceframe to be used as the source of data to send back to
   GDB.  A value of -1 means to get data from the live program.  */

int current_traceframe = -1;

/* This flag is true if the trace buffer is circular, meaning that
   when it fills, the oldest trace frames are discarded in order to
   make room.  */

static int circular_trace_buffer;

/* Pointer to the block of memory that traceframes all go into.  */

static unsigned char *trace_buffer_lo;

/* Pointer to the end of the trace buffer, more precisely to the byte
   after the end of the buffer.  */

static unsigned char *trace_buffer_hi;

/* Pointer to the first trace frame in the buffer.  In the
   non-circular case, this is equal to trace_buffer_lo, otherwise it
   moves around in the buffer.  */

static unsigned char *trace_buffer_start;

/* Pointer to the free part of the trace buffer.  Note that we clear
   several bytes at and after this pointer, so that traceframe
   scans/searches terminate properly.  */

static unsigned char *trace_buffer_free;

/* Pointer to the byte after the end of the free part.  Note that this
   may be smaller than trace_buffer_free in the circular case, and
   means that the free part is in two pieces.  Initially it is equal
   to trace_buffer_hi, then is generally equivalent to
   trace_buffer_start.  */

static unsigned char *trace_buffer_end_free;

/* Pointer to the wraparound.  If not equal to trace_buffer_hi, then
   this is the point at which the trace data breaks, and resumes at
   trace_buffer_lo.  */

static unsigned char *trace_buffer_wrap;

/* Macro that returns a pointer to the first traceframe in the buffer.  */

#define FIRST_TRACEFRAME() ((struct traceframe *) trace_buffer_start)

/* Macro that returns a pointer to the next traceframe in the buffer.
   If the computed location is beyond the wraparound point, subtract
   the offset of the wraparound.  */

#define NEXT_TRACEFRAME_1(TF) \
  (((unsigned char *) (TF)) + sizeof (struct traceframe) + (TF)->data_size)

#define NEXT_TRACEFRAME(TF) \
  ((struct traceframe *) (NEXT_TRACEFRAME_1 (TF)  \
			  - ((NEXT_TRACEFRAME_1 (TF) >= trace_buffer_wrap) \
			     ? (trace_buffer_wrap - trace_buffer_lo)	\
			     : 0)))

/* The difference between these counters represents the total number
   of complete traceframes present in the trace buffer.  */

static unsigned int traceframe_write_count;
static unsigned int traceframe_read_count;

/* Convenience macro.  */

#define traceframe_count \
  ((unsigned int) (traceframe_write_count - traceframe_read_count))

/* The count of all traceframes created in the current run, including
   ones that were discarded to make room.  */

static int traceframes_created;

/* Read-only regions are address ranges whose contents don't change,
   and so can be read from target memory even while looking at a trace
   frame.  Without these, disassembly for instance will likely fail,
   because the program code is not usually collected into a trace
   frame.  This data structure does not need to be very complicated or
   particularly efficient, it's only going to be used occasionally,
   and only by some commands.  */

struct readonly_region
{
  /* The bounds of the region.  */
  CORE_ADDR start, end;

  /* Link to the next one.  */
  struct readonly_region *next;
};

/* Linked list of readonly regions.  This list stays in effect from
   one tstart to the next.  */

static struct readonly_region *readonly_regions;

/* The global that controls tracing overall.  */

int tracing;

/* Controls whether tracing should continue after GDB disconnects.  */

int disconnected_tracing;

/* The reason for the last tracing run to have stopped.  We initialize
   to a distinct string so that GDB can distinguish between "stopped
   after running" and "stopped because never run" cases.  */

static const char *tracing_stop_reason = "tnotrun";

static int tracing_stop_tpnum;

/* Functions local to this file.  */

/* Base "class" for tracepoint type specific data to be passed down to
   collect_data_at_tracepoint. */
struct tracepoint_hit_ctx
{
  /* empty */
};

/* Trap tracepoint specific data to be passed down to
   collect_data_at_tracepoint.  */

struct trap_tracepoint_ctx
{
  struct tracepoint_hit_ctx base;

  struct regcache *regcache;
};

static struct agent_expr *parse_agent_expr (char **actparm);
static char *unparse_agent_expr (struct agent_expr *aexpr);
static enum eval_result_type eval_agent_expr (struct tracepoint_hit_ctx *ctx,
					      struct traceframe *tframe,
					      struct agent_expr *aexpr,
					      ULONGEST *rslt);

static int agent_mem_read (struct traceframe *tframe,
			   unsigned char *to, CORE_ADDR from, ULONGEST len);
static int agent_tsv_read (struct traceframe *tframe, int n);

static CORE_ADDR traceframe_get_pc (struct traceframe *tframe);
static int traceframe_read_tsv (int num, LONGEST *val);

static int condition_true_at_tracepoint (struct tracepoint_hit_ctx *ctx,
					 struct tracepoint *tpoint);

static void clear_readonly_regions (void);
static void clear_installed_tracepoints (void);

static void collect_data_at_tracepoint (struct tracepoint_hit_ctx *ctx,
					CORE_ADDR stop_pc,
					struct tracepoint *tpoint);

static void collect_data_at_step (struct tracepoint_hit_ctx *ctx,
				  CORE_ADDR stop_pc,
				  struct tracepoint *tpoint, int current_step);

static void do_action_at_tracepoint (struct tracepoint_hit_ctx *ctx,
				     CORE_ADDR stop_pc,
				     struct tracepoint *tpoint,
				     struct traceframe *tframe,
				     struct tracepoint_action *taction);

/* Record that an error occurred during expression evaluation.  */

static void
record_tracepoint_error (struct tracepoint *tpoint, const char *which,
			 enum eval_result_type rtype)
{
  trace_debug ("Tracepoint %d at %s %s eval reports error %d",
	       tpoint->number, paddress (tpoint->address), which, rtype);

  expr_eval_result = rtype;
  error_tracepoint = tpoint;
}

/* Trace buffer management.  */

static void
clear_trace_buffer (void)
{
  trace_buffer_start = trace_buffer_lo;
  trace_buffer_free = trace_buffer_lo;
  trace_buffer_end_free = trace_buffer_hi;
  trace_buffer_wrap = trace_buffer_hi;
  /* A traceframe with zeroed fields marks the end of trace data.  */
  ((struct traceframe *) trace_buffer_free)->tpnum = 0;
  ((struct traceframe *) trace_buffer_free)->data_size = 0;
  traceframe_read_count = traceframe_write_count = 0;
  traceframes_created = 0;
}

static void
init_trace_buffer (unsigned char *buf, int bufsize)
{
  trace_buffer_lo = buf;
  trace_buffer_hi = trace_buffer_lo + bufsize;

  clear_trace_buffer ();
}

/* Carve out a piece of the trace buffer, returning NULL in case of
   failure.  */

static void *
trace_buffer_alloc (size_t amt)
{
  unsigned char *rslt;
  struct traceframe *oldest;
  unsigned char *new_start;

  trace_debug ("Want to allocate %ld+%ld bytes in trace buffer",
	       (long) amt, (long) sizeof (struct traceframe));

  /* Account for the EOB marker.  */
  amt += sizeof (struct traceframe);

  /* Offsets are easier to grok for debugging than raw addresses,
     especially for the small trace buffer sizes that are useful for
     testing.  */
  trace_debug ("Trace buffer start=%d free=%d endfree=%d wrap=%d hi=%d",
	       (int) (trace_buffer_start - trace_buffer_lo),
	       (int) (trace_buffer_free - trace_buffer_lo),
	       (int) (trace_buffer_end_free - trace_buffer_lo),
	       (int) (trace_buffer_wrap - trace_buffer_lo),
	       (int) (trace_buffer_hi - trace_buffer_lo));

  /* The algorithm here is to keep trying to get a contiguous block of
     the requested size, possibly discarding older traceframes to free
     up space.  Since free space might come in one or two pieces,
     depending on whether discarded traceframes wrapped around at the
     high end of the buffer, we test both pieces after each
     discard.  */
  while (1)
    {
      /* First, if we have two free parts, try the upper one first.  */
      if (trace_buffer_end_free < trace_buffer_free)
	{
	  if (trace_buffer_free + amt <= trace_buffer_hi)
	    /* We have enough in the upper part.  */
	    break;
	  else
	    {
	      /* Our high part of free space wasn't enough.  Give up
		 on it for now, set wraparound.  We will recover the
		 space later, if/when the wrapped-around traceframe is
		 discarded.  */
	      trace_debug ("Upper part too small, setting wraparound");
	      trace_buffer_wrap = trace_buffer_free;
	      trace_buffer_free = trace_buffer_lo;
	    }
	}

      /* The normal case.  */
      if (trace_buffer_free + amt <= trace_buffer_end_free)
	break;

      /* If we're here, then neither part is big enough, and
	 non-circular trace buffers are now full.  */
      if (!circular_trace_buffer)
	{
	  trace_debug ("Not enough space in the trace buffer");
	  return NULL;
	}

      trace_debug ("Need more space in the trace buffer");

      /* If we have a circular buffer, we can try discarding the
	 oldest traceframe and see if that helps.  */
      oldest = FIRST_TRACEFRAME ();
      if (oldest->tpnum == 0)
	{
	  /* Not good; we have no traceframes to free.  Perhaps we're
	     asking for a block that is larger than the buffer?  In
	     any case, give up.  */
	  trace_debug ("No traceframes to discard");
	  return NULL;
	}

      --traceframe_write_count;

      new_start = (unsigned char *) NEXT_TRACEFRAME (oldest);
      /* If we freed the traceframe that wrapped around, go back
	 to the non-wrap case.  */
      if (new_start < trace_buffer_start)
	{
	  trace_debug ("Discarding past the wraparound");
	  trace_buffer_wrap = trace_buffer_hi;
	}
      trace_buffer_start = new_start;
      trace_buffer_end_free = trace_buffer_start;

      trace_debug ("Discarded a traceframe\n"
		   "Trace buffer, start=%d free=%d endfree=%d wrap=%d hi=%d",
		   (int) (trace_buffer_start - trace_buffer_lo),
		   (int) (trace_buffer_free - trace_buffer_lo),
		   (int) (trace_buffer_end_free - trace_buffer_lo),
		   (int) (trace_buffer_wrap - trace_buffer_lo),
		   (int) (trace_buffer_hi - trace_buffer_lo));

      /* Now go back around the loop.  The discard might have resulted
	 in either one or two pieces of free space, so we want to try
	 both before freeing any more traceframes.  */
    }

  /* If we get here, we know we can provide the asked-for space.  */

  rslt = trace_buffer_free;

  /* Adjust the request back down, now that we know we have space for
     the marker.  */
  trace_buffer_free += (amt - sizeof (struct traceframe));

  /* We have a new piece of the trace buffer.  Hurray!  */

  /* Add an EOB marker just past this allocation.  */
  ((struct traceframe *) trace_buffer_free)->tpnum = 0;
  ((struct traceframe *) trace_buffer_free)->data_size = 0;

  /* Adjust the request back down, now that we know we have space for
     the marker.  */
  amt -= sizeof (struct traceframe);

  if (debug_threads)
    {
      trace_debug ("Allocated %d bytes", (int) amt);
      trace_debug ("Trace buffer start=%d free=%d endfree=%d wrap=%d hi=%d",
		   (int) (trace_buffer_start - trace_buffer_lo),
		   (int) (trace_buffer_free - trace_buffer_lo),
		   (int) (trace_buffer_end_free - trace_buffer_lo),
		   (int) (trace_buffer_wrap - trace_buffer_lo),
		   (int) (trace_buffer_hi - trace_buffer_lo));
    }

  return rslt;
}

/* Return the total free space.  This is not necessarily the largest
   block we can allocate, because of the two-part case.  */

static int
free_space (void)
{
  if (trace_buffer_free <= trace_buffer_end_free)
    return trace_buffer_end_free - trace_buffer_free;
  else
    return ((trace_buffer_end_free - trace_buffer_lo)
	    + (trace_buffer_hi - trace_buffer_free));
}

/* An 'S' in continuation packets indicates remainder are for
   while-stepping.  */

static int seen_step_action_flag;

/* Create a tracepoint (location) with given number and address.  */

static struct tracepoint *
add_tracepoint (int num, CORE_ADDR addr)
{
  struct tracepoint *tpoint;

  tpoint = xmalloc (sizeof (struct tracepoint));
  tpoint->number = num;
  tpoint->address = addr;
  tpoint->numactions = 0;
  tpoint->actions = NULL;
  tpoint->actions_str = NULL;
  tpoint->cond = NULL;
  tpoint->num_step_actions = 0;
  tpoint->step_actions = NULL;
  tpoint->step_actions_str = NULL;
  tpoint->source_strings = NULL;
  tpoint->handle = NULL;
  tpoint->next = NULL;

  if (!last_tracepoint)
    tracepoints = tpoint;
  else
    last_tracepoint->next = tpoint;
  last_tracepoint = tpoint;

  seen_step_action_flag = 0;

  return tpoint;
}

/* Return the tracepoint with the given number and address, or NULL.  */

static struct tracepoint *
find_tracepoint (int id, CORE_ADDR addr)
{
  struct tracepoint *tpoint;

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    if (tpoint->number == id && tpoint->address == addr)
      return tpoint;

  return NULL;
}

/* There may be several tracepoints with the same number (because they
   are "locations", in GDB parlance); return the next one after the
   given tracepoint, or search from the beginning of the list if the
   first argument is NULL.  */

static struct tracepoint *
find_next_tracepoint_by_number (struct tracepoint *prev_tp, int num)
{
  struct tracepoint *tpoint;

  if (prev_tp)
    tpoint = prev_tp->next;
  else
    tpoint = tracepoints;
  for (; tpoint; tpoint = tpoint->next)
    if (tpoint->number == num)
      return tpoint;

  return NULL;
}

static char *
save_string (const char *str, size_t len)
{
  char *s;

  s = xmalloc (len + 1);
  memcpy (s, str, len);
  s[len] = '\0';

  return s;
}

/* Append another action to perform when the tracepoint triggers.  */

static void
add_tracepoint_action (struct tracepoint *tpoint, char *packet)
{
  char *act;

  if (*packet == 'S')
    {
      seen_step_action_flag = 1;
      ++packet;
    }

  act = packet;

  while (*act)
    {
      char *act_start = act;
      struct tracepoint_action *action = NULL;

      switch (*act)
	{
	case 'M':
	  {
	    struct collect_memory_action *maction;
	    ULONGEST basereg;
	    int is_neg;

	    maction = xmalloc (sizeof *maction);
	    maction->base.type = *act;
	    action = &maction->base;

	    ++act;
	    is_neg = (*act == '-');
	    if (*act == '-')
	      ++act;
	    act = unpack_varlen_hex (act, &basereg);
	    ++act;
	    act = unpack_varlen_hex (act, &maction->addr);
	    ++act;
	    act = unpack_varlen_hex (act, &maction->len);
	    maction->basereg = (is_neg
				? - (int) basereg
				: (int) basereg);
	    trace_debug ("Want to collect %s bytes at 0x%s (basereg %d)",
			 pulongest (maction->len),
			 paddress (maction->addr), maction->basereg);
	    break;
	  }
	case 'R':
	  {
	    struct collect_registers_action *raction;

	    raction = xmalloc (sizeof *raction);
	    raction->base.type = *act;
	    action = &raction->base;

	    trace_debug ("Want to collect registers");
	    ++act;
	    /* skip past hex digits of mask for now */
	    while (isxdigit(*act))
	      ++act;
	    break;
	  }
	case 'S':
	  trace_debug ("Unexpected step action, ignoring");
	  ++act;
	  break;
	case 'X':
	  {
	    struct eval_expr_action *xaction;

	    xaction = xmalloc (sizeof (*xaction));
	    xaction->base.type = *act;
	    action = &xaction->base;

	    trace_debug ("Want to evaluate expression");
	    xaction->expr = parse_agent_expr (&act);
	    break;
	  }
	default:
	  trace_debug ("unknown trace action '%c', ignoring...", *act);
	  break;
	case '-':
	  break;
	}

      if (action == NULL)
	break;

      if (seen_step_action_flag)
	{
	  tpoint->num_step_actions++;

	  tpoint->step_actions
	    = xrealloc (tpoint->step_actions,
			(sizeof (*tpoint->step_actions)
			 * tpoint->num_step_actions));
	  tpoint->step_actions_str
	    = xrealloc (tpoint->step_actions_str,
			(sizeof (*tpoint->step_actions_str)
			 * tpoint->num_step_actions));
	  tpoint->step_actions[tpoint->num_step_actions - 1] = action;
	  tpoint->step_actions_str[tpoint->num_step_actions - 1]
	    = save_string (act_start, act - act_start);
	}
      else
	{
	  tpoint->numactions++;
	  tpoint->actions
	    = xrealloc (tpoint->actions,
			sizeof (*tpoint->actions) * tpoint->numactions);
	  tpoint->actions_str
	    = xrealloc (tpoint->actions_str,
			sizeof (*tpoint->actions_str) * tpoint->numactions);
	  tpoint->actions[tpoint->numactions - 1] = action;
	  tpoint->actions_str[tpoint->numactions - 1]
	    = save_string (act_start, act - act_start);
	}
    }
}

/* Find or create a trace state variable with the given number.  */

static struct trace_state_variable *
get_trace_state_variable (int num)
{
  struct trace_state_variable *tsv;

  /* Search for an existing variable.  */
  for (tsv = trace_state_variables; tsv; tsv = tsv->next)
    if (tsv->number == num)
      return tsv;

  return NULL;
}

/* Find or create a trace state variable with the given number.  */

static struct trace_state_variable *
create_trace_state_variable (int num)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);
  if (tsv != NULL)
    return tsv;

  /* Create a new variable.  */
  tsv = xmalloc (sizeof (struct trace_state_variable));
  tsv->number = num;
  tsv->initial_value = 0;
  tsv->value = 0;
  tsv->getter = NULL;
  tsv->name = NULL;
  tsv->next = trace_state_variables;
  trace_state_variables = tsv;

  return tsv;
}

static LONGEST
get_trace_state_variable_value (int num)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping value get", num);
      return 0;
    }

  /* Call a getter function if we have one.  While it's tempting to
     set up something to only call the getter once per tracepoint hit,
     it could run afoul of thread races. Better to let the getter
     handle it directly, if necessary to worry about it.  */
  if (tsv->getter)
    tsv->value = (tsv->getter) ();

  trace_debug ("get_trace_state_variable_value(%d) ==> %s",
	       num, plongest (tsv->value));

  return tsv->value;
}

static void
set_trace_state_variable_value (int num, LONGEST val)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping value set", num);
      return;
    }

  tsv->value = val;
}

static void
set_trace_state_variable_name (int num, const char *name)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping name set", num);
      return;
    }

  tsv->name = (char *) name;
}

static void
set_trace_state_variable_getter (int num, LONGEST (*getter) (void))
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping getter set", num);
      return;
    }

  tsv->getter = getter;
}

/* Add a raw traceframe for the given tracepoint.  */

static struct traceframe *
add_traceframe (struct tracepoint *tpoint)
{
  struct traceframe *tframe;

  tframe = trace_buffer_alloc (sizeof (struct traceframe));

  if (tframe == NULL)
    return NULL;

  tframe->tpnum = tpoint->number;
  tframe->data_size = 0;

  return tframe;
}

/* Add a block to the traceframe currently being worked on.  */

static unsigned char *
add_traceframe_block (struct traceframe *tframe, int amt)
{
  unsigned char *block;

  if (!tframe)
    return NULL;

  block = trace_buffer_alloc (amt);

  if (!block)
    return NULL;

  tframe->data_size += amt;

  return block;
}

/* Flag that the current traceframe is finished.  */

static void
finish_traceframe (struct traceframe *tframe)
{
  ++traceframe_write_count;
  ++traceframes_created;
}

/* Given a traceframe number NUM, find the NUMth traceframe in the
   buffer.  */

static struct traceframe *
find_traceframe (int num)
{
  struct traceframe *tframe;
  int tfnum = 0;

  for (tframe = FIRST_TRACEFRAME ();
       tframe->tpnum != 0;
       tframe = NEXT_TRACEFRAME (tframe))
    {
      if (tfnum == num)
	return tframe;
      ++tfnum;
    }

  return NULL;
}

static CORE_ADDR
get_traceframe_address (struct traceframe *tframe)
{
  CORE_ADDR addr;
  struct tracepoint *tpoint;

  addr = traceframe_get_pc (tframe);

  if (addr)
    return addr;

  /* Fallback strategy, will be incorrect for while-stepping frames
     and multi-location tracepoints.  */
  tpoint = find_next_tracepoint_by_number (NULL, tframe->tpnum);
  return tpoint->address;
}

/* Search for the next traceframe whose address is inside or outside
   the given range.  */

static struct traceframe *
find_next_traceframe_in_range (CORE_ADDR lo, CORE_ADDR hi, int inside_p,
			       int *tfnump)
{
  struct traceframe *tframe;
  CORE_ADDR tfaddr;

  *tfnump = current_traceframe + 1;
  tframe = find_traceframe (*tfnump);
  /* The search is not supposed to wrap around.  */
  if (!tframe)
    {
      *tfnump = -1;
      return NULL;
    }

  for (; tframe->tpnum != 0; tframe = NEXT_TRACEFRAME (tframe))
    {
      tfaddr = get_traceframe_address (tframe);
      if (inside_p
	  ? (lo <= tfaddr && tfaddr <= hi)
	  : (lo > tfaddr || tfaddr > hi))
	return tframe;
      ++*tfnump;
    }

  *tfnump = -1;
  return NULL;
}

/* Search for the next traceframe recorded by the given tracepoint.
   Note that for multi-location tracepoints, this will find whatever
   location appears first.  */

static struct traceframe *
find_next_traceframe_by_tracepoint (int num, int *tfnump)
{
  struct traceframe *tframe;

  *tfnump = current_traceframe + 1;
  tframe = find_traceframe (*tfnump);
  /* The search is not supposed to wrap around.  */
  if (!tframe)
    {
      *tfnump = -1;
      return NULL;
    }

  for (; tframe->tpnum != 0; tframe = NEXT_TRACEFRAME (tframe))
    {
      if (tframe->tpnum == num)
	return tframe;
      ++*tfnump;
    }

  *tfnump = -1;
  return NULL;
}

/* Clear all past trace state.  */

static void
cmd_qtinit (char *packet)
{
  struct trace_state_variable *tsv, *prev, *next;

  /* Make sure we don't try to read from a trace frame.  */
  current_traceframe = -1;

  trace_debug ("Initializing the trace");

  clear_installed_tracepoints ();
  clear_readonly_regions ();

  tracepoints = NULL;
  last_tracepoint = NULL;

  /* Clear out any leftover trace state variables.  Ones with target
     defined getters should be kept however.  */
  prev = NULL;
  tsv = trace_state_variables;
  while (tsv)
    {
      trace_debug ("Looking at var %d", tsv->number);
      if (tsv->getter == NULL)
	{
	  next = tsv->next;
	  if (prev)
	    prev->next = next;
	  else
	    trace_state_variables = next;
	  trace_debug ("Deleting var %d", tsv->number);
	  free (tsv);
	  tsv = next;
	}
      else
	{
	  prev = tsv;
	  tsv = tsv->next;
	}
    }

  clear_trace_buffer ();

  write_ok (packet);
}

/* Restore the program to its pre-tracing state.  This routine may be called
   in error situations, so it needs to be careful about only restoring
   from known-valid bits.  */

static void
clear_installed_tracepoints (void)
{
  struct tracepoint *tpoint;
  struct tracepoint *prev_stpoint;

  pause_all (1);
  cancel_breakpoints ();

  prev_stpoint = NULL;

  /* Restore any bytes overwritten by tracepoints.  */
  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      if (!tpoint->enabled)
	continue;

      /* Catch the case where we might try to remove a tracepoint that
	 was never actually installed.  */
      if (tpoint->handle == NULL)
	{
	  trace_debug ("Tracepoint %d at 0x%s was "
		       "never installed, nothing to clear",
		       tpoint->number, paddress (tpoint->address));
	  continue;
	}

      delete_breakpoint (tpoint->handle);
      tpoint->handle = NULL;
    }

  unpause_all (1);
}

/* Parse a packet that defines a tracepoint.  */

static void
cmd_qtdp (char *own_buf)
{
  int tppacket;
  ULONGEST num;
  ULONGEST addr;
  ULONGEST count;
  struct tracepoint *tpoint;
  char *actparm;
  char *packet = own_buf;

  packet += strlen ("QTDP:");

  /* A hyphen at the beginning marks a packet specifying actions for a
     tracepoint already supplied.  */
  tppacket = 1;
  if (*packet == '-')
    {
      tppacket = 0;
      ++packet;
    }
  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &addr);
  ++packet; /* skip a colon */

  /* See if we already have this tracepoint.  */
  tpoint = find_tracepoint (num, addr);

  if (tppacket)
    {
      /* Duplicate tracepoints are never allowed.  */
      if (tpoint)
	{
	  trace_debug ("Tracepoint error: tracepoint %d"
		       " at 0x%s already exists",
		       (int) num, paddress (addr));
	  write_enn (own_buf);
	  return;
	}

      tpoint = add_tracepoint (num, addr);

      tpoint->enabled = (*packet == 'E');
      ++packet; /* skip 'E' */
      ++packet; /* skip a colon */
      packet = unpack_varlen_hex (packet, &count);
      tpoint->step_count = count;
      ++packet; /* skip a colon */
      packet = unpack_varlen_hex (packet, &count);
      tpoint->pass_count = count;
      /* See if we have any of the additional optional fields.  */
      while (*packet == ':')
	{
	  ++packet;
	  if (*packet == 'X')
	    {
	      actparm = (char *) packet;
	      tpoint->cond = parse_agent_expr (&actparm);
	      packet = actparm;
	    }
	  else if (*packet == '-')
	    break;
	  else if (*packet == '\0')
	    break;
	  else
	    trace_debug ("Unknown optional tracepoint field");
	}
      if (*packet == '-')
	trace_debug ("Also has actions\n");

      trace_debug ("Defined tracepoint %d at 0x%s, "
		   "enabled %d step %ld pass %ld",
		   tpoint->number, paddress (tpoint->address),
		   tpoint->enabled,
		   tpoint->step_count, tpoint->pass_count);
    }
  else if (tpoint)
    add_tracepoint_action (tpoint, packet);
  else
    {
      trace_debug ("Tracepoint error: tracepoint %d at 0x%s not found",
		   (int) num, paddress (addr));
      write_enn (own_buf);
      return;
    }

  write_ok (own_buf);
}

static void
cmd_qtdpsrc (char *own_buf)
{
  ULONGEST num, addr, start, slen;
  struct tracepoint *tpoint;
  char *packet = own_buf;
  char *saved, *srctype, *src;
  size_t nbytes;
  struct source_string *last, *newlast;

  packet += strlen ("QTDPsrc:");

  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &addr);
  ++packet; /* skip a colon */

  /* See if we already have this tracepoint.  */
  tpoint = find_tracepoint (num, addr);

  if (!tpoint)
    {
      trace_debug ("Tracepoint error: tracepoint %d at 0x%s not found",
		   (int) num, paddress (addr));
      write_enn (own_buf);
      return;
    }

  saved = packet;
  packet = strchr (packet, ':');
  srctype = xmalloc (packet - saved + 1);
  memcpy (srctype, saved, packet - saved);
  srctype[packet - saved] = '\0';
  ++packet;
  packet = unpack_varlen_hex (packet, &start);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &slen);
  ++packet; /* skip a colon */
  src = xmalloc (slen + 1);
  nbytes = unhexify (src, packet, strlen (packet) / 2);
  src[nbytes] = '\0';

  newlast = xmalloc (sizeof (struct source_string));
  newlast->type = srctype;
  newlast->str = src;
  newlast->next = NULL;
  /* Always add a source string to the end of the list;
     this keeps sequences of actions/commands in the right
     order.  */
  if (tpoint->source_strings)
    {
      for (last = tpoint->source_strings; last->next; last = last->next)
	;
      last->next = newlast;
    }
  else
    tpoint->source_strings = newlast;

  write_ok (own_buf);
}

static void
cmd_qtdv (char *own_buf)
{
  ULONGEST num, val, builtin;
  char *varname;
  size_t nbytes;
  struct trace_state_variable *tsv;
  char *packet = own_buf;

  packet += strlen ("QTDV:");

  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &val);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &builtin);
  ++packet; /* skip a colon */

  nbytes = strlen (packet) / 2;
  varname = xmalloc (nbytes + 1);
  nbytes = unhexify (varname, packet, nbytes);
  varname[nbytes] = '\0';

  tsv = create_trace_state_variable (num);
  tsv->initial_value = (LONGEST) val;
  tsv->name = varname;

  set_trace_state_variable_value (num, (LONGEST) val);

  write_ok (own_buf);
}

static void
cmd_qtv (char *own_buf)
{
  ULONGEST num;
  LONGEST val;
  int err;
  char *packet = own_buf;

  packet += strlen ("qTV:");
  packet = unpack_varlen_hex (packet, &num);

  if (current_traceframe >= 0)
    {
      err = traceframe_read_tsv ((int) num, &val);
      if (err)
	{
	  strcpy (own_buf, "U");
	  return;
	}
    }
  /* Only make tsv's be undefined before the first trace run.  After a
     trace run is over, the user might want to see the last value of
     the tsv, and it might not be available in a traceframe.  */
  else if (!tracing && strcmp (tracing_stop_reason, "tnotrun") == 0)
    {
      strcpy (own_buf, "U");
      return;
    }
  else
    val = get_trace_state_variable_value (num);

  sprintf (own_buf, "V%s", phex_nz (val, 0));
}

/* Clear out the list of readonly regions.  */

static void
clear_readonly_regions (void)
{
  struct readonly_region *roreg;

  while (readonly_regions)
    {
      roreg = readonly_regions;
      readonly_regions = readonly_regions->next;
      free (roreg);
    }
}

/* Parse the collection of address ranges whose contents GDB believes
   to be unchanging and so can be read directly from target memory
   even while looking at a traceframe.  */

static void
cmd_qtro (char *own_buf)
{
  ULONGEST start, end;
  struct readonly_region *roreg;
  char *packet = own_buf;

  trace_debug ("Want to mark readonly regions");

  clear_readonly_regions ();

  packet += strlen ("QTro");

  while (*packet == ':')
    {
      ++packet;  /* skip a colon */
      packet = unpack_varlen_hex (packet, &start);
      ++packet;  /* skip a comma */
      packet = unpack_varlen_hex (packet, &end);
      roreg = xmalloc (sizeof (struct readonly_region));
      roreg->start = start;
      roreg->end = end;
      roreg->next = readonly_regions;
      readonly_regions = roreg;
      trace_debug ("Added readonly region from 0x%s to 0x%s",
		   paddress (roreg->start), paddress (roreg->end));
    }

  write_ok (own_buf);
}

/* Test to see if the given range is in our list of readonly ranges.
   We only test for being entirely within a range, GDB is not going to
   send a single memory packet that spans multiple regions.  */

int
in_readonly_region (CORE_ADDR addr, ULONGEST length)
{
  struct readonly_region *roreg;

  for (roreg = readonly_regions; roreg; roreg = roreg->next)
    if (roreg->start <= addr && (addr + length - 1) <= roreg->end)
      return 1;

  return 0;
}

static void
cmd_qtstart (char *packet)
{
  struct tracepoint *tpoint;
  int slow_tracepoint_count;

  trace_debug ("Starting the trace");

  slow_tracepoint_count = 0;

  *packet = '\0';

  /* Pause all threads temporarily while we patch tracepoints.  */
  pause_all (1);

  /* Install tracepoints.  */
  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      /* Ensure all the hit counts start at zero.  */
      tpoint->hit_count = 0;

      if (!tpoint->enabled)
	continue;

      ++slow_tracepoint_count;

      /* Tracepoints are installed as memory breakpoints.  Just go
	 ahead and install the trap.  The breakpoints module handles
	 duplicated breakpoints, and the memory read routine handles
	 un-patching traps from memory reads.  */
      tpoint->handle = set_breakpoint_at (tpoint->address, tracepoint_handler);

      /* Any failure is sufficient cause to give up.  */
      if (tpoint->handle == NULL)
	break;
    }

  /* Any error in tracepoint insertion is unacceptable; better to
     address the problem now, than end up with a useless or misleading
     trace run.  */
  if (tpoint != NULL)
    {
      clear_installed_tracepoints ();
      if (*packet == '\0')
	write_enn (packet);
      unpause_all (1);
      return;
    }

  stopping_tracepoint = NULL;
  trace_buffer_is_full = 0;
  expr_eval_result = expr_eval_no_error;
  error_tracepoint = NULL;

  /* Tracing is now active, hits will now start being logged.  */
  tracing = 1;

  unpause_all (1);

  write_ok (packet);
}

/* End a tracing run, filling in a stop reason to report back to GDB,
   and removing the tracepoints from the code.  */

void
stop_tracing (void)
{
  if (!tracing)
    {
      trace_debug ("Tracing is already off, ignoring");
      return;
    }

  trace_debug ("Stopping the trace");

  /* Pause all threads before removing breakpoints from memory.  */
  pause_all (1);
  /* Since we're removing breakpoints, cancel breakpoint hits,
     possibly related to the breakpoints we're about to delete.  */
  cancel_breakpoints ();

  /* Stop logging. Tracepoints can still be hit, but they will not be
     recorded.  */
  tracing = 0;

  tracing_stop_reason = "t???";
  tracing_stop_tpnum = 0;
  if (stopping_tracepoint)
    {
      trace_debug ("Stopping the trace because "
		   "tracepoint %d was hit %ld times",
		   stopping_tracepoint->number,
		   stopping_tracepoint->pass_count);
      tracing_stop_reason = "tpasscount";
      tracing_stop_tpnum = stopping_tracepoint->number;
    }
  else if (trace_buffer_is_full)
    {
      trace_debug ("Stopping the trace because the trace buffer is full");
      tracing_stop_reason = "tfull";
    }
  else if (expr_eval_result != expr_eval_no_error)
    {
      trace_debug ("Stopping the trace because of an expression eval error");
      tracing_stop_reason = eval_result_names[expr_eval_result];
      tracing_stop_tpnum = error_tracepoint->number;
    }
  else if (!gdb_connected ())
    {
      trace_debug ("Stopping the trace because GDB disconnected");
      tracing_stop_reason = "tdisconnected";
    }
  else
    {
      trace_debug ("Stopping the trace because of a tstop command");
      tracing_stop_reason = "tstop";
    }

  stopping_tracepoint = NULL;
  error_tracepoint = NULL;

  /* Clear out the tracepoints.  */
  clear_installed_tracepoints ();

  unpause_all (1);
}

static void
cmd_qtstop (char *packet)
{
  stop_tracing ();
  write_ok (packet);
}

static void
cmd_qtdisconnected (char *own_buf)
{
  ULONGEST setting;
  char *packet = own_buf;

  packet += strlen ("QTDisconnected:");

  unpack_varlen_hex (packet, &setting);

  write_ok (own_buf);

  disconnected_tracing = setting;
}

static void
cmd_qtframe (char *own_buf)
{
  ULONGEST frame, pc, lo, hi, num;
  int tfnum, tpnum;
  struct traceframe *tframe;
  char *packet = own_buf;

  packet += strlen ("QTFrame:");

  if (strncmp (packet, "pc:", strlen ("pc:")) == 0)
    {
      packet += strlen ("pc:");
      packet = unpack_varlen_hex (packet, &pc);
      trace_debug ("Want to find next traceframe at pc=0x%s", paddress (pc));
      tframe = find_next_traceframe_in_range (pc, pc, 1, &tfnum);
    }
  else if (strncmp (packet, "range:", strlen ("range:")) == 0)
    {
      packet += strlen ("range:");
      packet = unpack_varlen_hex (packet, &lo);
      ++packet;
      packet = unpack_varlen_hex (packet, &hi);
      trace_debug ("Want to find next traceframe in the range 0x%s to 0x%s",
		   paddress (lo), paddress (hi));
      tframe = find_next_traceframe_in_range (lo, hi, 1, &tfnum);
    }
  else if (strncmp (packet, "outside:", strlen ("outside:")) == 0)
    {
      packet += strlen ("outside:");
      packet = unpack_varlen_hex (packet, &lo);
      ++packet;
      packet = unpack_varlen_hex (packet, &hi);
      trace_debug ("Want to find next traceframe "
		   "outside the range 0x%s to 0x%s",
		   paddress (lo), paddress (hi));
      tframe = find_next_traceframe_in_range (lo, hi, 0, &tfnum);
    }
  else if (strncmp (packet, "tdp:", strlen ("tdp:")) == 0)
    {
      packet += strlen ("tdp:");
      packet = unpack_varlen_hex (packet, &num);
      tpnum = (int) num;
      trace_debug ("Want to find next traceframe for tracepoint %d", tpnum);
      tframe = find_next_traceframe_by_tracepoint (tpnum, &tfnum);
    }
  else
    {
      unpack_varlen_hex (packet, &frame);
      tfnum = (int) frame;
      if (tfnum == -1)
	{
	  trace_debug ("Want to stop looking at traceframes");
	  current_traceframe = -1;
	  write_ok (own_buf);
	  return;
	}
      trace_debug ("Want to look at traceframe %d", tfnum);
      tframe = find_traceframe (tfnum);
    }

  if (tframe)
    {
      current_traceframe = tfnum;
      sprintf (own_buf, "F%xT%x", tfnum, tframe->tpnum);
    }
  else
    sprintf (own_buf, "F-1");
}

static void
cmd_qtstatus (char *packet)
{
  char *stop_reason_rsp = NULL;

  trace_debug ("Returning trace status as %d, stop reason %s",
	       tracing, tracing_stop_reason);

  stop_reason_rsp = (char *) tracing_stop_reason;

  /* The user visible error string in terror needs to be hex encoded.
     We leave it as plain string in `tracepoint_stop_reason' to ease
     debugging.  */
  if (strncmp (stop_reason_rsp, "terror:", strlen ("terror:")) == 0)
    {
      const char *result_name;
      int hexstr_len;
      char *p;

      result_name = stop_reason_rsp + strlen ("terror:");
      hexstr_len = strlen (result_name) * 2;
      p = stop_reason_rsp = alloca (strlen ("terror:") + hexstr_len + 1);
      strcpy (p, "terror:");
      p += strlen (p);
      convert_int_to_ascii ((gdb_byte *) result_name, p, strlen (result_name));
    }

  sprintf (packet,
	   "T%d;"
	   "%s:%x;"
	   "tframes:%x;tcreated:%x;"
	   "tfree:%x;tsize:%s;"
	   "circular:%d;"
	   "disconn:%d",
	   tracing ? 1 : 0,
	   stop_reason_rsp, tracing_stop_tpnum,
	   traceframe_count, traceframes_created,
	   free_space (), phex_nz (trace_buffer_hi - trace_buffer_lo, 0),
	   circular_trace_buffer,
	   disconnected_tracing);
}

/* State variables to help return all the tracepoint bits.  */
static struct tracepoint *cur_tpoint;
static int cur_action;
static int cur_step_action;
static struct source_string *cur_source_string;
static struct trace_state_variable *cur_tsv;

/* Compose a response that is an imitation of the syntax by which the
   tracepoint was originally downloaded.  */

static void
response_tracepoint (char *packet, struct tracepoint *tpoint)
{
  char *buf;

  sprintf (packet, "T%x:%s:%c:%lx:%lx", tpoint->number,
	   paddress (tpoint->address),
	   (tpoint->enabled ? 'E' : 'D'), tpoint->step_count,
	   tpoint->pass_count);

  if (tpoint->cond)
    {
      buf = unparse_agent_expr (tpoint->cond);
      sprintf (packet + strlen (packet), ":X%x,%s",
	       tpoint->cond->length, buf);
      free (buf);
    }
}

/* Compose a response that is an imitation of the syntax by which the
   tracepoint action was originally downloaded (with the difference
   that due to the way we store the actions, this will output a packet
   per action, while GDB could have combined more than one action
   per-packet.  */

static void
response_action (char *packet, struct tracepoint *tpoint,
		 char *taction, int step)
{
  sprintf (packet, "%c%x:%s:%s",
	   (step ? 'S' : 'A'), tpoint->number, paddress (tpoint->address),
	   taction);
}

/* Compose a response that is an imitation of the syntax by which the
   tracepoint source piece was originally downloaded.  */

static void
response_source (char *packet,
		 struct tracepoint *tpoint, struct source_string *src)
{
  char *buf;
  int len;

  len = strlen (src->str);
  buf = alloca (len * 2 + 1);
  convert_int_to_ascii ((gdb_byte *) src->str, buf, len);

  sprintf (packet, "Z%x:%s:%s:%x:%x:%s",
	   tpoint->number, paddress (tpoint->address),
	   src->type, 0, len, buf);
}

/* Return the first piece of tracepoint definition, and initialize the
   state machine that will iterate through all the tracepoint
   bits.  */

static void
cmd_qtfp (char *packet)
{
  trace_debug ("Returning first tracepoint definition piece");

  cur_tpoint = tracepoints;
  cur_action = cur_step_action = -1;
  cur_source_string = NULL;

  if (cur_tpoint)
    response_tracepoint (packet, cur_tpoint);
  else
    strcpy (packet, "l");
}

/* Return additional pieces of tracepoint definition.  Each action and
   stepping action must go into its own packet, because of packet size
   limits, and so we use state variables to deliver one piece at a
   time.  */

static void
cmd_qtsp (char *packet)
{
  trace_debug ("Returning subsequent tracepoint definition piece");

  if (!cur_tpoint)
    {
      /* This case would normally never occur, but be prepared for
	 GDB misbehavior.  */
      strcpy (packet, "l");
    }
  else if (cur_action < cur_tpoint->numactions - 1)
    {
      ++cur_action;
      response_action (packet, cur_tpoint,
		       cur_tpoint->actions_str[cur_action], 0);
    }
  else if (cur_step_action < cur_tpoint->num_step_actions - 1)
    {
      ++cur_step_action;
      response_action (packet, cur_tpoint,
		       cur_tpoint->step_actions_str[cur_step_action], 1);
    }
  else if ((cur_source_string
	    ? cur_source_string->next
	    : cur_tpoint->source_strings))
    {
      if (cur_source_string)
	cur_source_string = cur_source_string->next;
      else
	cur_source_string = cur_tpoint->source_strings;
      response_source (packet, cur_tpoint, cur_source_string);
    }
  else
    {
      cur_tpoint = cur_tpoint->next;
      cur_action = cur_step_action = -1;
      cur_source_string = NULL;
      if (cur_tpoint)
	response_tracepoint (packet, cur_tpoint);
      else
	strcpy (packet, "l");
    }
}

/* Compose a response that is an imitation of the syntax by which the
   trace state variable was originally downloaded.  */

static void
response_tsv (char *packet, struct trace_state_variable *tsv)
{
  char *buf = (char *) "";
  int namelen;

  if (tsv->name)
    {
      namelen = strlen (tsv->name);
      buf = alloca (namelen * 2 + 1);
      convert_int_to_ascii ((gdb_byte *) tsv->name, buf, namelen);
    }

  sprintf (packet, "%x:%s:%x:%s", tsv->number, phex_nz (tsv->initial_value, 0),
	   tsv->getter ? 1 : 0, buf);
}

/* Return the first trace state variable definition, and initialize
   the state machine that will iterate through all the tsv bits.  */

static void
cmd_qtfv (char *packet)
{
  trace_debug ("Returning first trace state variable definition");

  cur_tsv = trace_state_variables;

  if (cur_tsv)
    response_tsv (packet, cur_tsv);
  else
    strcpy (packet, "l");
}

/* Return additional trace state variable definitions. */

static void
cmd_qtsv (char *packet)
{
  trace_debug ("Returning first trace state variable definition");

  if (!cur_tpoint)
    {
      /* This case would normally never occur, but be prepared for
	 GDB misbehavior.  */
      strcpy (packet, "l");
    }
  else if (cur_tsv)
    {
      cur_tsv = cur_tsv->next;
      if (cur_tsv)
	response_tsv (packet, cur_tsv);
      else
	strcpy (packet, "l");
    }
  else
    strcpy (packet, "l");
}

/* Respond to qTBuffer packet with a block of raw data from the trace
   buffer.  GDB may ask for a lot, but we are allowed to reply with
   only as much as will fit within packet limits or whatever.  */

static void
cmd_qtbuffer (char *own_buf)
{
  ULONGEST offset, num, tot;
  unsigned char *tbp;
  char *packet = own_buf;

  packet += strlen ("qTBuffer:");

  packet = unpack_varlen_hex (packet, &offset);
  ++packet; /* skip a comma */
  packet = unpack_varlen_hex (packet, &num);

  trace_debug ("Want to get trace buffer, %d bytes at offset 0x%s",
	       (int) num, pulongest (offset));

  tot = (trace_buffer_hi - trace_buffer_lo) - free_space ();

  /* If we're right at the end, reply specially that we're done.  */
  if (offset == tot)
    {
      strcpy (own_buf, "l");
      return;
    }

  /* Object to any other out-of-bounds request.  */
  if (offset > tot)
    {
      write_enn (own_buf);
      return;
    }

  /* Compute the pointer corresponding to the given offset, accounting
     for wraparound.  */
  tbp = trace_buffer_start + offset;
  if (tbp >= trace_buffer_wrap)
    tbp -= (trace_buffer_wrap - trace_buffer_lo);

  /* Trim to the remaining bytes if we're close to the end.  */
  if (num > tot - offset)
    num = tot - offset;

  /* Trim to available packet size.  */
  if (num >= (PBUFSIZ - 16) / 2 )
    num = (PBUFSIZ - 16) / 2;

  convert_int_to_ascii (tbp, own_buf, num);
  own_buf[num] = '\0';
}

static void
cmd_bigqtbuffer (char *own_buf)
{
  ULONGEST val;
  char *packet = own_buf;

  packet += strlen ("QTBuffer:");

  if (strncmp ("circular:", packet, strlen ("circular:")) == 0)
    {
      packet += strlen ("circular:");
      packet = unpack_varlen_hex (packet, &val);
      circular_trace_buffer = val;
      trace_debug ("Trace buffer is now %s",
		   circular_trace_buffer ? "circular" : "linear");
      write_ok (own_buf);
    }
  else
    write_enn (own_buf);
}

int
handle_tracepoint_general_set (char *packet)
{
  if (strcmp ("QTinit", packet) == 0)
    {
      cmd_qtinit (packet);
      return 1;
    }
  else if (strncmp ("QTDP:", packet, strlen ("QTDP:")) == 0)
    {
      cmd_qtdp (packet);
      return 1;
    }
  else if (strncmp ("QTDPsrc:", packet, strlen ("QTDPsrc:")) == 0)
    {
      cmd_qtdpsrc (packet);
      return 1;
    }
  else if (strncmp ("QTDV:", packet, strlen ("QTDV:")) == 0)
    {
      cmd_qtdv (packet);
      return 1;
    }
  else if (strncmp ("QTro:", packet, strlen ("QTro:")) == 0)
    {
      cmd_qtro (packet);
      return 1;
    }
  else if (strcmp ("QTStart", packet) == 0)
    {
      cmd_qtstart (packet);
      return 1;
    }
  else if (strcmp ("QTStop", packet) == 0)
    {
      cmd_qtstop (packet);
      return 1;
    }
  else if (strncmp ("QTDisconnected:", packet,
		    strlen ("QTDisconnected:")) == 0)
    {
      cmd_qtdisconnected (packet);
      return 1;
    }
  else if (strncmp ("QTFrame:", packet, strlen ("QTFrame:")) == 0)
    {
      cmd_qtframe (packet);
      return 1;
    }
  else if (strncmp ("QTBuffer:", packet, strlen ("QTBuffer:")) == 0)
    {
      cmd_bigqtbuffer (packet);
      return 1;
    }

  return 0;
}

int
handle_tracepoint_query (char *packet)
{
  if (strcmp ("qTStatus", packet) == 0)
    {
      cmd_qtstatus (packet);
      return 1;
    }
  else if (strcmp ("qTfP", packet) == 0)
    {
      cmd_qtfp (packet);
      return 1;
    }
  else if (strcmp ("qTsP", packet) == 0)
    {
      cmd_qtsp (packet);
      return 1;
    }
  else if (strcmp ("qTfV", packet) == 0)
    {
      cmd_qtfv (packet);
      return 1;
    }
  else if (strcmp ("qTsV", packet) == 0)
    {
      cmd_qtsv (packet);
      return 1;
    }
  else if (strncmp ("qTV:", packet, strlen ("qTV:")) == 0)
    {
      cmd_qtv (packet);
      return 1;
    }
  else if (strncmp ("qTBuffer:", packet, strlen ("qTBuffer:")) == 0)
    {
      cmd_qtbuffer (packet);
      return 1;
    }

  return 0;
}

/* Call this when thread TINFO has hit the tracepoint defined by
   TP_NUMBER and TP_ADDRESS, and that tracepoint has a while-stepping
   action.  This adds a while-stepping collecting state item to the
   threads' collecting state list, so that we can keep track of
   multiple simultaneous while-stepping actions being collected by the
   same thread.  This can happen in cases like:

    ff0001  INSN1 <-- TP1, while-stepping 10 collect $regs
    ff0002  INSN2
    ff0003  INSN3 <-- TP2, collect $regs
    ff0004  INSN4 <-- TP3, while-stepping 10 collect $regs
    ff0005  INSN5

   Notice that when instruction INSN5 is reached, the while-stepping
   actions of both TP1 and TP3 are still being collected, and that TP2
   had been collected meanwhile.  The whole range of ff0001-ff0005
   should be single-stepped, due to at least TP1's while-stepping
   action covering the whole range.  */

static void
add_while_stepping_state (struct thread_info *tinfo,
			  int tp_number, CORE_ADDR tp_address)
{
  struct wstep_state *wstep;

  wstep = xmalloc (sizeof (*wstep));
  wstep->next = tinfo->while_stepping;

  wstep->tp_number = tp_number;
  wstep->tp_address = tp_address;
  wstep->current_step = 0;

  tinfo->while_stepping = wstep;
}

/* Release the while-stepping collecting state WSTEP.  */

static void
release_while_stepping_state (struct wstep_state *wstep)
{
  free (wstep);
}

/* Release all while-stepping collecting states currently associated
   with thread TINFO.  */

void
release_while_stepping_state_list (struct thread_info *tinfo)
{
  struct wstep_state *head;

  while (tinfo->while_stepping)
    {
      head = tinfo->while_stepping;
      tinfo->while_stepping = head->next;
      release_while_stepping_state (head);
    }
}

/* If TINFO was handling a 'while-stepping' action, the step has
   finished, so collect any step data needed, and check if any more
   steps are required.  Return true if the thread was indeed
   collecting tracepoint data, false otherwise.  */

int
tracepoint_finished_step (struct thread_info *tinfo, CORE_ADDR stop_pc)
{
  struct tracepoint *tpoint;
  struct wstep_state *wstep;
  struct wstep_state **wstep_link;
  struct trap_tracepoint_ctx ctx;

  /* Check if we were indeed collecting data for one of more
     tracepoints with a 'while-stepping' count.  */
  if (tinfo->while_stepping == NULL)
    return 0;

  if (!tracing)
    {
      /* We're not even tracing anymore.  Stop this thread from
	 collecting.  */
      release_while_stepping_state_list (tinfo);

      /* The thread had stopped due to a single-step request indeed
	 explained by a tracepoint.  */
      return 1;
    }

  wstep = tinfo->while_stepping;
  wstep_link = &tinfo->while_stepping;

  trace_debug ("Thread %s finished a single-step for tracepoint %d at 0x%s",
	       target_pid_to_str (tinfo->entry.id),
	       wstep->tp_number, paddress (wstep->tp_address));

  ctx.regcache = get_thread_regcache (tinfo, 1);

  while (wstep != NULL)
    {
      tpoint = find_tracepoint (wstep->tp_number, wstep->tp_address);
      if (tpoint == NULL)
	{
	  trace_debug ("NO TRACEPOINT %d at 0x%s FOR THREAD %s!",
		       wstep->tp_number, paddress (wstep->tp_address),
		       target_pid_to_str (tinfo->entry.id));

	  /* Unlink.  */
	  *wstep_link = wstep->next;
	  release_while_stepping_state (wstep);
	  continue;
	}

      /* We've just finished one step.  */
      ++wstep->current_step;

      /* Collect data.  */
      collect_data_at_step ((struct tracepoint_hit_ctx *) &ctx,
			    stop_pc, tpoint, wstep->current_step);

      if (wstep->current_step >= tpoint->step_count)
	{
	  /* The requested numbers of steps have occurred.  */
	  trace_debug ("Thread %s done stepping for tracepoint %d at 0x%s",
		       target_pid_to_str (tinfo->entry.id),
		       wstep->tp_number, paddress (wstep->tp_address));

	  /* Unlink the wstep.  */
	  *wstep_link = wstep->next;
	  release_while_stepping_state (wstep);
	  wstep = *wstep_link;

	  /* Only check the hit count now, which ensure that we do all
	     our stepping before stopping the run.  */
	  if (tpoint->pass_count > 0
	      && tpoint->hit_count >= tpoint->pass_count
	      && stopping_tracepoint == NULL)
	    stopping_tracepoint = tpoint;
	}
      else
	{
	  /* Keep single-stepping until the requested numbers of steps
	     have occurred.  */
	  wstep_link = &wstep->next;
	  wstep = *wstep_link;
	}

      if (stopping_tracepoint
	  || trace_buffer_is_full
	  || expr_eval_result != expr_eval_no_error)
	{
	  stop_tracing ();
	  break;
	}
    }

  return 1;
}

/* Return true if TINFO just hit a tracepoint.  Collect data if
   so.  */

int
tracepoint_was_hit (struct thread_info *tinfo, CORE_ADDR stop_pc)
{
  struct tracepoint *tpoint;
  int ret = 0;
  struct trap_tracepoint_ctx ctx;

  /* Not tracing, don't handle.  */
  if (!tracing)
    return 0;

  ctx.regcache = get_thread_regcache (tinfo, 1);

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      if (tpoint->enabled && stop_pc == tpoint->address)
	{
	  trace_debug ("Thread %s at address of tracepoint %d at 0x%s",
		       target_pid_to_str (tinfo->entry.id),
		       tpoint->number, paddress (tpoint->address));

	  /* Test the condition if present, and collect if true.  */
	  if (!tpoint->cond
	      || (condition_true_at_tracepoint
		  ((struct tracepoint_hit_ctx *) &ctx, tpoint)))
	    collect_data_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
					stop_pc, tpoint);

	  if (stopping_tracepoint
	      || trace_buffer_is_full
	      || expr_eval_result != expr_eval_no_error)
	    {
	      stop_tracing ();
	    }
	  /* If the tracepoint had a 'while-stepping' action, then set
	     the thread to collect this tracepoint on the following
	     single-steps.  */
	  else if (tpoint->step_count > 0)
	    {
	      add_while_stepping_state (tinfo,
					tpoint->number, tpoint->address);
	    }

	  ret = 1;
	}
    }

  return ret;
}

/* Create a trace frame for the hit of the given tracepoint in the
   given thread.  */

static void
collect_data_at_tracepoint (struct tracepoint_hit_ctx *ctx, CORE_ADDR stop_pc,
			    struct tracepoint *tpoint)
{
  struct traceframe *tframe;
  int acti;

  /* Only count it as a hit when we actually collect data.  */
  tpoint->hit_count++;

  /* If we've exceeded a defined pass count, record the event for
     later, and finish the collection for this hit.  This test is only
     for nonstepping tracepoints, stepping tracepoints test at the end
     of their while-stepping loop.  */
  if (tpoint->pass_count > 0
      && tpoint->hit_count >= tpoint->pass_count
      && tpoint->step_count == 0
      && stopping_tracepoint == NULL)
    stopping_tracepoint = tpoint;

  trace_debug ("Making new traceframe for tracepoint %d at 0x%s, hit %ld",
	       tpoint->number, paddress (tpoint->address), tpoint->hit_count);

  tframe = add_traceframe (tpoint);

  if (tframe)
    {
      for (acti = 0; acti < tpoint->numactions; ++acti)
	{
	  trace_debug ("Tracepoint %d at 0x%s about to do action '%s'",
		       tpoint->number, paddress (tpoint->address),
		       tpoint->actions_str[acti]);

	  do_action_at_tracepoint (ctx, stop_pc, tpoint, tframe,
				   tpoint->actions[acti]);
	}

      finish_traceframe (tframe);
    }

  if (tframe == NULL && tracing)
    trace_buffer_is_full = 1;
}

static void
collect_data_at_step (struct tracepoint_hit_ctx *ctx,
		      CORE_ADDR stop_pc,
		      struct tracepoint *tpoint, int current_step)
{
  struct traceframe *tframe;
  int acti;

  trace_debug ("Making new step traceframe for "
	       "tracepoint %d at 0x%s, step %d of %ld, hit %ld",
	       tpoint->number, paddress (tpoint->address),
	       current_step, tpoint->step_count,
	       tpoint->hit_count);

  tframe = add_traceframe (tpoint);

  if (tframe)
    {
      for (acti = 0; acti < tpoint->num_step_actions; ++acti)
	{
	  trace_debug ("Tracepoint %d at 0x%s about to do step action '%s'",
		       tpoint->number, paddress (tpoint->address),
		       tpoint->step_actions_str[acti]);

	  do_action_at_tracepoint (ctx, stop_pc, tpoint, tframe,
				   tpoint->step_actions[acti]);
	}

      finish_traceframe (tframe);
    }

  if (tframe == NULL && tracing)
    trace_buffer_is_full = 1;
}

static struct regcache *
get_context_regcache (struct tracepoint_hit_ctx *ctx)
{
  struct trap_tracepoint_ctx *tctx = (struct trap_tracepoint_ctx *) ctx;
  struct regcache *regcache = tctx->regcache;

  gdb_assert (regcache != NULL);

  return regcache;
}

static void
do_action_at_tracepoint (struct tracepoint_hit_ctx *ctx,
			 CORE_ADDR stop_pc,
			 struct tracepoint *tpoint,
			 struct traceframe *tframe,
			 struct tracepoint_action *taction)
{
  enum eval_result_type err;

  switch (taction->type)
    {
    case 'M':
      {
	struct collect_memory_action *maction;

	maction = (struct collect_memory_action *) taction;

	trace_debug ("Want to collect %s bytes at 0x%s (basereg %d)",
		     pulongest (maction->len),
		     paddress (maction->addr), maction->basereg);
	/* (should use basereg) */
	agent_mem_read (tframe, NULL,
			(CORE_ADDR) maction->addr, maction->len);
	break;
      }
    case 'R':
      {
	struct collect_registers_action *raction;

	unsigned char *regspace;
	struct regcache tregcache;
	struct regcache *context_regcache;

	raction = (struct collect_registers_action *) taction;

	trace_debug ("Want to collect registers");

	/* Collect all registers for now.  */
	regspace = add_traceframe_block (tframe,
					 1 + register_cache_size ());
	if (regspace == NULL)
	  {
	    trace_debug ("Trace buffer block allocation failed, skipping");
	    break;
	  }
	/* Identify a register block.  */
	*regspace = 'R';

	context_regcache = get_context_regcache (ctx);

	/* Wrap the regblock in a register cache (in the stack, we
	   don't want to malloc here).  */
	init_register_cache (&tregcache, regspace + 1);

	/* Copy the register data to the regblock.  */
	regcache_cpy (&tregcache, context_regcache);

	/* On some platforms, trap-based tracepoints will have the PC
	   pointing to the next instruction after the trap, but we
	   don't want the user or GDB trying to guess whether the
	   saved PC needs adjusting; so always record the adjusted
	   stop_pc.  Note that we can't use tpoint->address instead,
	   since it will be wrong for while-stepping actions.  */
	trace_debug ("Storing stop pc (0x%s) in regblock",
		     paddress (tpoint->address));

	/* This changes the regblock, not the thread's
	   regcache.  */
	regcache_write_pc (&tregcache, stop_pc);
      }
      break;
    case 'X':
      {
	struct eval_expr_action *eaction;

	eaction = (struct eval_expr_action *) taction;

	trace_debug ("Want to evaluate expression");

	err = eval_agent_expr (ctx, tframe, eaction->expr, NULL);

	if (err != expr_eval_no_error)
	  {
	    record_tracepoint_error (tpoint, "action expression", err);
	    return;
	  }
      }
      break;
    default:
      trace_debug ("unknown trace action '%c', ignoring", taction->type);
      break;
    }
}

static int
condition_true_at_tracepoint (struct tracepoint_hit_ctx *ctx,
			      struct tracepoint *tpoint)
{
  ULONGEST value = 0;
  enum eval_result_type err;

  err = eval_agent_expr (ctx, NULL, tpoint->cond, &value);

  if (err != expr_eval_no_error)
    {
      record_tracepoint_error (tpoint, "condition", err);
      /* The error case must return false.  */
      return 0;
    }

  trace_debug ("Tracepoint %d at 0x%s condition evals to %s",
	       tpoint->number, paddress (tpoint->address),
	       pulongest (value));
  return (value ? 1 : 0);
}

/* The packet form of an agent expression consists of an 'X', number
   of bytes in expression, a comma, and then the bytes.  */

static struct agent_expr *
parse_agent_expr (char **actparm)
{
  char *act = *actparm;
  ULONGEST xlen;
  struct agent_expr *aexpr;

  ++act;  /* skip the X */
  act = unpack_varlen_hex (act, &xlen);
  ++act;  /* skip a comma */
  aexpr = xmalloc (sizeof (struct agent_expr));
  aexpr->length = xlen;
  aexpr->bytes = xmalloc (xlen);
  convert_ascii_to_int (act, aexpr->bytes, xlen);
  *actparm = act + (xlen * 2);
  return aexpr;
}

/* Convert the bytes of an agent expression back into hex digits, so
   they can be printed or uploaded.  This allocates the buffer,
   callers should free when they are done with it.  */

static char *
unparse_agent_expr (struct agent_expr *aexpr)
{
  char *rslt;

  rslt = xmalloc (2 * aexpr->length + 1);
  convert_int_to_ascii (aexpr->bytes, rslt, aexpr->length);
  return rslt;
}

/* The agent expression evaluator, as specified by the GDB docs. It
   returns 0 if everything went OK, and a nonzero error code
   otherwise.  */

static enum eval_result_type
eval_agent_expr (struct tracepoint_hit_ctx *ctx,
		 struct traceframe *tframe,
		 struct agent_expr *aexpr,
		 ULONGEST *rslt)
{
  int pc = 0;
#define STACK_MAX 100
  ULONGEST stack[STACK_MAX], top;
  int sp = 0;
  unsigned char op;
  int arg;

  /* This union is a convenient way to convert representations.  For
     now, assume a standard architecture where the hardware integer
     types have 8, 16, 32, 64 bit types.  A more robust solution would
     be to import stdint.h from gnulib.  */
  union
  {
    union
    {
      unsigned char bytes[1];
      unsigned char val;
    } u8;
    union
    {
      unsigned char bytes[2];
      unsigned short val;
    } u16;
    union
    {
      unsigned char bytes[4];
      unsigned int val;
    } u32;
    union
    {
      unsigned char bytes[8];
      ULONGEST val;
    } u64;
  } cnv;

  if (aexpr->length == 0)
    {
      trace_debug ("empty agent expression");
      return expr_eval_empty_expression;
    }

  /* Cache the stack top in its own variable. Much of the time we can
     operate on this variable, rather than dinking with the stack. It
     needs to be copied to the stack when sp changes.  */
  top = 0;

  while (1)
    {
      op = aexpr->bytes[pc++];

      trace_debug ("About to interpret byte 0x%x", op);

      switch (op)
	{
	case gdb_agent_op_add:
	  top += stack[--sp];
	  break;

	case gdb_agent_op_sub:
	  top = stack[--sp] - top;
	  break;

	case gdb_agent_op_mul:
	  top *= stack[--sp];
	  break;

	case gdb_agent_op_div_signed:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) / ((LONGEST) top);
	  break;

	case gdb_agent_op_div_unsigned:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = stack[--sp] / top;
	  break;

	case gdb_agent_op_rem_signed:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) % ((LONGEST) top);
	  break;

	case gdb_agent_op_rem_unsigned:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = stack[--sp] % top;
	  break;

	case gdb_agent_op_lsh:
	  top = stack[--sp] << top;
	  break;

	case gdb_agent_op_rsh_signed:
	  top = ((LONGEST) stack[--sp]) >> top;
	  break;

	case gdb_agent_op_rsh_unsigned:
	  top = stack[--sp] >> top;
	  break;

	case gdb_agent_op_trace:
	  agent_mem_read (tframe,
			  NULL, (CORE_ADDR) stack[--sp], (ULONGEST) top);
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_trace_quick:
	  arg = aexpr->bytes[pc++];
	  agent_mem_read (tframe, NULL, (CORE_ADDR) top, (ULONGEST) arg);
	  break;

	case gdb_agent_op_log_not:
	  top = !top;
	  break;

	case gdb_agent_op_bit_and:
	  top &= stack[--sp];
	  break;

	case gdb_agent_op_bit_or:
	  top |= stack[--sp];
	  break;

	case gdb_agent_op_bit_xor:
	  top ^= stack[--sp];
	  break;

	case gdb_agent_op_bit_not:
	  top = ~top;
	  break;

	case gdb_agent_op_equal:
	  top = (stack[--sp] == top);
	  break;

	case gdb_agent_op_less_signed:
	  top = (((LONGEST) stack[--sp]) < ((LONGEST) top));
	  break;

	case gdb_agent_op_less_unsigned:
	  top = (stack[--sp] < top);
	  break;

	case gdb_agent_op_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    {
	      LONGEST mask = 1 << (arg - 1);
	      top &= ((LONGEST) 1 << arg) - 1;
	      top = (top ^ mask) - mask;
	    }
	  break;

	case gdb_agent_op_ref8:
	  agent_mem_read (tframe, cnv.u8.bytes, (CORE_ADDR) top, 1);
	  top = cnv.u8.val;
	  break;

	case gdb_agent_op_ref16:
	  agent_mem_read (tframe, cnv.u16.bytes, (CORE_ADDR) top, 2);
	  top = cnv.u16.val;
	  break;

	case gdb_agent_op_ref32:
	  agent_mem_read (tframe, cnv.u32.bytes, (CORE_ADDR) top, 4);
	  top = cnv.u32.val;
	  break;

	case gdb_agent_op_ref64:
	  agent_mem_read (tframe, cnv.u64.bytes, (CORE_ADDR) top, 8);
	  top = cnv.u64.val;
	  break;

	case gdb_agent_op_if_goto:
	  if (top)
	    pc = (aexpr->bytes[pc] << 8) + (aexpr->bytes[pc + 1]);
	  else
	    pc += 2;
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_goto:
	  pc = (aexpr->bytes[pc] << 8) + (aexpr->bytes[pc + 1]);
	  break;

	case gdb_agent_op_const8:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const16:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const32:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const64:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_reg:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  {
	    int regnum = arg;
	    struct regcache *regcache;

	    regcache = get_context_regcache (ctx);

	    switch (register_size (regnum))
	      {
	      case 8:
		collect_register (regcache, regnum, cnv.u64.bytes);
		top = cnv.u64.val;
		break;
	      case 4:
		collect_register (regcache, regnum, cnv.u32.bytes);
		top = cnv.u32.val;
		break;
	      case 2:
		collect_register (regcache, regnum, cnv.u16.bytes);
		top = cnv.u16.val;
		break;
	      case 1:
		collect_register (regcache, regnum, cnv.u8.bytes);
		top = cnv.u8.val;
		break;
	      default:
		internal_error (__FILE__, __LINE__,
				"unhandled register size");
	      }
	  }
	  break;

	case gdb_agent_op_end:
	  trace_debug ("At end of expression, sp=%d, stack top cache=0x%s",
		       sp, pulongest (top));
	  if (rslt)
	    {
	      if (sp <= 0)
		{
		  /* This should be an error */
		  trace_debug ("Stack is empty, nothing to return");
		  return expr_eval_empty_stack;
		}
	      *rslt = top;
	    }
	  return expr_eval_no_error;

	case gdb_agent_op_dup:
	  stack[sp++] = top;
	  break;

	case gdb_agent_op_pop:
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_zero_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    top &= ((LONGEST) 1 << arg) - 1;
	  break;

	case gdb_agent_op_swap:
	  /* Interchange top two stack elements, making sure top gets
	     copied back onto stack.  */
	  stack[sp] = top;
	  top = stack[sp - 1];
	  stack[sp - 1] = stack[sp];
	  break;

	case gdb_agent_op_getv:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  top = get_trace_state_variable_value (arg);
	  break;

	case gdb_agent_op_setv:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  set_trace_state_variable_value (arg, top);
	  /* Note that we leave the value on the stack, for the
	     benefit of later/enclosing expressions.  */
	  break;

	case gdb_agent_op_tracev:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  agent_tsv_read (tframe, arg);
	  break;

	  /* GDB never (currently) generates any of these ops.  */
	case gdb_agent_op_float:
	case gdb_agent_op_ref_float:
	case gdb_agent_op_ref_double:
	case gdb_agent_op_ref_long_double:
	case gdb_agent_op_l_to_d:
	case gdb_agent_op_d_to_l:
	case gdb_agent_op_trace16:
	  trace_debug ("Agent expression op 0x%x valid, but not handled",
		       op);
	  /* If ever GDB generates any of these, we don't have the
	     option of ignoring.  */
	  return 1;

	default:
	  trace_debug ("Agent expression op 0x%x not recognized", op);
	  /* Don't struggle on, things will just get worse.  */
	  return expr_eval_unrecognized_opcode;
	}

      /* Check for stack badness.  */
      if (sp >= (STACK_MAX - 1))
	{
	  trace_debug ("Expression stack overflow");
	  return expr_eval_stack_overflow;
	}

      if (sp < 0)
	{
	  trace_debug ("Expression stack underflow");
	  return expr_eval_stack_underflow;
	}

      trace_debug ("Op %s -> sp=%d, top=0x%s",
		   gdb_agent_op_names[op], sp, pulongest (top));
    }
}

/* Do memory copies for bytecodes.  */
/* Do the recording of memory blocks for actions and bytecodes.  */

static int
agent_mem_read (struct traceframe *tframe,
		unsigned char *to, CORE_ADDR from, ULONGEST len)
{
  unsigned char *mspace;
  ULONGEST remaining = len;
  unsigned short blocklen;

  /* If a 'to' buffer is specified, use it.  */
  if (to != NULL)
    {
      read_inferior_memory (from, to, len);
      return 0;
    }

  /* Otherwise, create a new memory block in the trace buffer.  */
  while (remaining > 0)
    {
      size_t sp;

      blocklen = (remaining > 65535 ? 65535 : remaining);
      sp = 1 + sizeof (from) + sizeof (blocklen) + blocklen;
      mspace = add_traceframe_block (tframe, sp);
      if (mspace == NULL)
	return 1;
      /* Identify block as a memory block.  */
      *mspace = 'M';
      ++mspace;
      /* Record address and size.  */
      memcpy (mspace, &from, sizeof (from));
      mspace += sizeof (from);
      memcpy (mspace, &blocklen, sizeof (blocklen));
      mspace += sizeof (blocklen);
      /* Record the memory block proper.  */
      read_inferior_memory (from, mspace, blocklen);
      trace_debug ("%d bytes recorded", blocklen);
      remaining -= blocklen;
      from += blocklen;
    }
  return 0;
}

/* Record the value of a trace state variable.  */

static int
agent_tsv_read (struct traceframe *tframe, int n)
{
  unsigned char *vspace;
  LONGEST val;

  vspace = add_traceframe_block (tframe,
				 1 + sizeof (n) + sizeof (LONGEST));
  if (vspace == NULL)
    return 1;
  /* Identify block as a variable.  */
  *vspace = 'V';
  /* Record variable's number and value.  */
  memcpy (vspace + 1, &n, sizeof (n));
  val = get_trace_state_variable_value (n);
  memcpy (vspace + 1 + sizeof (n), &val, sizeof (val));
  trace_debug ("Variable %d recorded", n);
  return 0;
}

static unsigned char *
traceframe_find_block_type (unsigned char *database, unsigned int datasize,
			    int tfnum, char type_wanted)
{
  unsigned char *dataptr;

  if (datasize == 0)
    {
      trace_debug ("traceframe %d has no data", tfnum);
      return NULL;
    }

  /* Iterate through a traceframe's blocks, looking for a block of the
     requested type.  */
  for (dataptr = database;
       dataptr < database + datasize;
       /* nothing */)
    {
      char blocktype;
      unsigned short mlen;

      if (dataptr == trace_buffer_wrap)
	{
	  /* Adjust to reflect wrapping part of the frame around to
	     the beginning.  */
	  datasize = dataptr - database;
	  dataptr = database = trace_buffer_lo;
	}
      blocktype = *dataptr++;

      if (type_wanted == blocktype)
	return dataptr;

      switch (blocktype)
	{
	case 'R':
	  /* Skip over the registers block.  */
	  dataptr += register_cache_size ();
	  break;
	case 'M':
	  /* Skip over the memory block.  */
	  dataptr += sizeof (CORE_ADDR);
	  memcpy (&mlen, dataptr, sizeof (mlen));
	  dataptr += (sizeof (mlen) + mlen);
	  break;
	case 'S':
	  /* Skip over the static trace data block.  */
	  memcpy (&mlen, dataptr, sizeof (mlen));
	  dataptr += (sizeof (mlen) + mlen);
	  break;
	case 'V':
	  /* Skip over the TSV block.  */
	  dataptr += (sizeof (int) + sizeof (LONGEST));
	  break;
	default:
	  trace_debug ("traceframe %d has unknown block type 0x%x",
		       tfnum, blocktype);
	  return NULL;
	}
    }

  return NULL;
}

static unsigned char *
traceframe_find_regblock (struct traceframe *tframe, int tfnum)
{
  unsigned char *regblock;

  regblock = traceframe_find_block_type (tframe->data,
					 tframe->data_size,
					 tfnum, 'R');

  if (regblock == NULL)
    trace_debug ("traceframe %d has no register data", tfnum);

  return regblock;
}

/* Get registers from a traceframe.  */

int
fetch_traceframe_registers (int tfnum, struct regcache *regcache, int regnum)
{
  unsigned char *dataptr;
  struct tracepoint *tpoint;
  struct traceframe *tframe;

  tframe = find_traceframe (tfnum);

  if (tframe == NULL)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  dataptr = traceframe_find_regblock (tframe, tfnum);
  if (dataptr == NULL)
    {
      /* We don't like making up numbers, but GDB has all manner of
	 troubles when the target says there are no registers.  */
      supply_regblock (regcache, NULL);

      /* We can generally guess at a PC, although this will be
	 misleading for while-stepping frames and multi-location
	 tracepoints.  */
      tpoint = find_next_tracepoint_by_number (NULL, tframe->tpnum);
      if (tpoint != NULL)
	regcache_write_pc (regcache, tpoint->address);
    }
  else
    supply_regblock (regcache, dataptr);

  return 0;
}

static CORE_ADDR
traceframe_get_pc (struct traceframe *tframe)
{
  struct regcache regcache;
  unsigned char *dataptr;

  dataptr = traceframe_find_regblock (tframe, -1);
  if (dataptr == NULL)
    return 0;

  init_register_cache (&regcache, dataptr);
  return regcache_read_pc (&regcache);
}

/* Read a requested block of memory from a trace frame.  */

int
traceframe_read_mem (int tfnum, CORE_ADDR addr,
		     unsigned char *buf, ULONGEST length,
		     ULONGEST *nbytes)
{
  struct traceframe *tframe;
  unsigned char *database, *dataptr;
  unsigned int datasize;
  CORE_ADDR maddr;
  unsigned short mlen;

  trace_debug ("traceframe_read_mem");

  tframe = find_traceframe (tfnum);

  if (!tframe)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  datasize = tframe->data_size;
  database = dataptr = &tframe->data[0];

  /* Iterate through a traceframe's blocks, looking for memory.  */
  while ((dataptr = traceframe_find_block_type (dataptr,
						datasize - (dataptr - database),
						tfnum, 'M')) != NULL)
    {
      memcpy (&maddr, dataptr, sizeof (maddr));
      dataptr += sizeof (maddr);
      memcpy (&mlen, dataptr, sizeof (mlen));
      dataptr += sizeof (mlen);
      trace_debug ("traceframe %d has %d bytes at %s",
		   tfnum, mlen, paddress (maddr));

      /* Check that requested data is in bounds.  */
      if (maddr <= addr && (addr + length) <= (maddr + mlen))
	{
	  /* Block includes the requested range, copy it out.  */
	  memcpy (buf, dataptr + (addr - maddr), length);
	  *nbytes = length;
	  return 0;
	}

      /* Skip over this block.  */
      dataptr += mlen;
    }

  trace_debug ("traceframe %d has no memory data for the desired region",
	       tfnum);

  *nbytes = 0;
  return 0;
}

static int
traceframe_read_tsv (int tsvnum, LONGEST *val)
{
  int tfnum;
  struct traceframe *tframe;
  unsigned char *database, *dataptr;
  unsigned int datasize;
  int vnum;

  trace_debug ("traceframe_read_tsv");

  tfnum = current_traceframe;

  if (tfnum < 0)
    {
      trace_debug ("no current traceframe");
      return 1;
    }

  tframe = find_traceframe (tfnum);

  if (tframe == NULL)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  datasize = tframe->data_size;
  database = dataptr = &tframe->data[0];

  /* Iterate through a traceframe's blocks, looking for the tsv.  */
  while ((dataptr = traceframe_find_block_type (dataptr,
						datasize - (dataptr - database),
						tfnum, 'V')) != NULL)
    {
      memcpy (&vnum, dataptr, sizeof (vnum));
      dataptr += sizeof (vnum);

      trace_debug ("traceframe %d has variable %d", tfnum, vnum);

      /* Check that this is the variable we want.  */
      if (tsvnum == vnum)
	{
	  memcpy (val, dataptr, sizeof (*val));
	  return 0;
	}

      /* Skip over this block.  */
      dataptr += sizeof (LONGEST);
    }

  trace_debug ("traceframe %d has no data for variable %d",
	       tfnum, tsvnum);
  return 1;
}

static LONGEST
tsv_get_timestamp (void)
{
   struct timeval tv;

   if (gettimeofday (&tv, 0) != 0)
     return -1;
   else
     return (LONGEST) tv.tv_sec * 1000000 + tv.tv_usec;
}

void
initialize_tracepoint (void)
{
  /* There currently no way to change the buffer size.  */
  const int sizeOfBuffer = 5 * 1024 * 1024;
  unsigned char *buf = xmalloc (sizeOfBuffer);
  init_trace_buffer (buf, sizeOfBuffer);

  /* Wire trace state variable 1 to be the timestamp.  This will be
     uploaded to GDB upon connection and become one of its trace state
     variables.  (In case you're wondering, if GDB already has a trace
     variable numbered 1, it will be renumbered.)  */
  create_trace_state_variable (1);
  set_trace_state_variable_name (1, "trace_timestamp");
  set_trace_state_variable_getter (1, tsv_get_timestamp);
}
