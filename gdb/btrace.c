/* Branch trace support for GDB, the GNU debugger.

   Copyright (C) 2013-2014 Free Software Foundation, Inc.

   Contributed by Intel Corp. <markus.t.metzger@intel.com>

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

#include "btrace.h"
#include "gdbthread.h"
#include "exceptions.h"
#include "inferior.h"
#include "target.h"
#include "record.h"
#include "symtab.h"
#include "disasm.h"
#include "source.h"
#include "filenames.h"
#include "xml-support.h"

/* Print a record debug message.  Use do ... while (0) to avoid ambiguities
   when used in if statements.  */

#define DEBUG(msg, args...)						\
  do									\
    {									\
      if (record_debug != 0)						\
        fprintf_unfiltered (gdb_stdlog,					\
			    "[btrace] " msg "\n", ##args);		\
    }									\
  while (0)

#define DEBUG_FTRACE(msg, args...) DEBUG ("[ftrace] " msg, ##args)

/* Return the function name of a recorded function segment for printing.
   This function never returns NULL.  */

static const char *
ftrace_print_function_name (const struct btrace_function *bfun)
{
  struct minimal_symbol *msym;
  struct symbol *sym;

  msym = bfun->msym;
  sym = bfun->sym;

  if (sym != NULL)
    return SYMBOL_PRINT_NAME (sym);

  if (msym != NULL)
    return SYMBOL_PRINT_NAME (msym);

  return "<unknown>";
}

/* Return the file name of a recorded function segment for printing.
   This function never returns NULL.  */

static const char *
ftrace_print_filename (const struct btrace_function *bfun)
{
  struct symbol *sym;
  const char *filename;

  sym = bfun->sym;

  if (sym != NULL)
    filename = symtab_to_filename_for_display (sym->symtab);
  else
    filename = "<unknown>";

  return filename;
}

/* Return a string representation of the address of an instruction.
   This function never returns NULL.  */

static const char *
ftrace_print_insn_addr (const struct btrace_insn *insn)
{
  if (insn == NULL)
    return "<nil>";

  return core_addr_to_string_nz (insn->pc);
}

/* Print an ftrace debug status message.  */

static void
ftrace_debug (const struct btrace_function *bfun, const char *prefix)
{
  const char *fun, *file;
  unsigned int ibegin, iend;
  int lbegin, lend, level;

  fun = ftrace_print_function_name (bfun);
  file = ftrace_print_filename (bfun);
  level = bfun->level;

  lbegin = bfun->lbegin;
  lend = bfun->lend;

  ibegin = bfun->insn_offset;
  iend = ibegin + VEC_length (btrace_insn_s, bfun->insn);

  DEBUG_FTRACE ("%s: fun = %s, file = %s, level = %d, lines = [%d; %d], "
		"insn = [%u; %u)", prefix, fun, file, level, lbegin, lend,
		ibegin, iend);
}

/* Return non-zero if BFUN does not match MFUN and FUN,
   return zero otherwise.  */

static int
ftrace_function_switched (const struct btrace_function *bfun,
			  const struct minimal_symbol *mfun,
			  const struct symbol *fun)
{
  struct minimal_symbol *msym;
  struct symbol *sym;

  msym = bfun->msym;
  sym = bfun->sym;

  /* If the minimal symbol changed, we certainly switched functions.  */
  if (mfun != NULL && msym != NULL
      && strcmp (SYMBOL_LINKAGE_NAME (mfun), SYMBOL_LINKAGE_NAME (msym)) != 0)
    return 1;

  /* If the symbol changed, we certainly switched functions.  */
  if (fun != NULL && sym != NULL)
    {
      const char *bfname, *fname;

      /* Check the function name.  */
      if (strcmp (SYMBOL_LINKAGE_NAME (fun), SYMBOL_LINKAGE_NAME (sym)) != 0)
	return 1;

      /* Check the location of those functions, as well.  */
      bfname = symtab_to_fullname (sym->symtab);
      fname = symtab_to_fullname (fun->symtab);
      if (filename_cmp (fname, bfname) != 0)
	return 1;
    }

  /* If we lost symbol information, we switched functions.  */
  if (!(msym == NULL && sym == NULL) && mfun == NULL && fun == NULL)
    return 1;

  /* If we gained symbol information, we switched functions.  */
  if (msym == NULL && sym == NULL && !(mfun == NULL && fun == NULL))
    return 1;

  return 0;
}

/* Return non-zero if we should skip this file when generating the function
   call history, zero otherwise.
   We would want to do that if, say, a macro that is defined in another file
   is expanded in this function.  */

static int
ftrace_skip_file (const struct btrace_function *bfun, const char *fullname)
{
  struct symbol *sym;
  const char *bfile;

  sym = bfun->sym;
  if (sym == NULL)
    return 1;

  bfile = symtab_to_fullname (sym->symtab);

  return (filename_cmp (bfile, fullname) != 0);
}

/* Allocate and initialize a new branch trace function segment.
   PREV is the chronologically preceding function segment.
   MFUN and FUN are the symbol information we have for this function.  */

static struct btrace_function *
ftrace_new_function (struct btrace_function *prev,
		     struct minimal_symbol *mfun,
		     struct symbol *fun)
{
  struct btrace_function *bfun;

  bfun = xzalloc (sizeof (*bfun));

  bfun->msym = mfun;
  bfun->sym = fun;
  bfun->flow.prev = prev;

  /* We start with the identities of min and max, respectively.  */
  bfun->lbegin = INT_MAX;
  bfun->lend = INT_MIN;

  if (prev == NULL)
    {
      /* Start counting at one.  */
      bfun->number = 1;
      bfun->insn_offset = 1;
    }
  else
    {
      gdb_assert (prev->flow.next == NULL);
      prev->flow.next = bfun;

      bfun->number = prev->number + 1;
      bfun->insn_offset = (prev->insn_offset
			   + VEC_length (btrace_insn_s, prev->insn));
    }

  return bfun;
}

/* Update the UP field of a function segment.  */

static void
ftrace_update_caller (struct btrace_function *bfun,
		      struct btrace_function *caller,
		      enum btrace_function_flag flags)
{
  if (bfun->up != NULL)
    ftrace_debug (bfun, "updating caller");

  bfun->up = caller;
  bfun->flags = flags;

  ftrace_debug (bfun, "set caller");
}

/* Fix up the caller for all segments of a function.  */

static void
ftrace_fixup_caller (struct btrace_function *bfun,
		     struct btrace_function *caller,
		     enum btrace_function_flag flags)
{
  struct btrace_function *prev, *next;

  ftrace_update_caller (bfun, caller, flags);

  /* Update all function segments belonging to the same function.  */
  for (prev = bfun->segment.prev; prev != NULL; prev = prev->segment.prev)
    ftrace_update_caller (prev, caller, flags);

  for (next = bfun->segment.next; next != NULL; next = next->segment.next)
    ftrace_update_caller (next, caller, flags);
}

/* Add a new function segment for a call.
   CALLER is the chronologically preceding function segment.
   MFUN and FUN are the symbol information we have for this function.  */

static struct btrace_function *
ftrace_new_call (struct btrace_function *caller,
		 struct minimal_symbol *mfun,
		 struct symbol *fun)
{
  struct btrace_function *bfun;

  bfun = ftrace_new_function (caller, mfun, fun);
  bfun->up = caller;
  bfun->level = caller->level + 1;

  ftrace_debug (bfun, "new call");

  return bfun;
}

/* Add a new function segment for a tail call.
   CALLER is the chronologically preceding function segment.
   MFUN and FUN are the symbol information we have for this function.  */

static struct btrace_function *
ftrace_new_tailcall (struct btrace_function *caller,
		     struct minimal_symbol *mfun,
		     struct symbol *fun)
{
  struct btrace_function *bfun;

  bfun = ftrace_new_function (caller, mfun, fun);
  bfun->up = caller;
  bfun->level = caller->level + 1;
  bfun->flags |= BFUN_UP_LINKS_TO_TAILCALL;

  ftrace_debug (bfun, "new tail call");

  return bfun;
}

/* Find the innermost caller in the back trace of BFUN with MFUN/FUN
   symbol information.  */

static struct btrace_function *
ftrace_find_caller (struct btrace_function *bfun,
		    struct minimal_symbol *mfun,
		    struct symbol *fun)
{
  for (; bfun != NULL; bfun = bfun->up)
    {
      /* Skip functions with incompatible symbol information.  */
      if (ftrace_function_switched (bfun, mfun, fun))
	continue;

      /* This is the function segment we're looking for.  */
      break;
    }

  return bfun;
}

/* Find the innermost caller in the back trace of BFUN, skipping all
   function segments that do not end with a call instruction (e.g.
   tail calls ending with a jump).  */

static struct btrace_function *
ftrace_find_call (struct gdbarch *gdbarch, struct btrace_function *bfun)
{
  for (; bfun != NULL; bfun = bfun->up)
    {
      struct btrace_insn *last;
      CORE_ADDR pc;

      /* We do not allow empty function segments.  */
      gdb_assert (!VEC_empty (btrace_insn_s, bfun->insn));

      last = VEC_last (btrace_insn_s, bfun->insn);
      pc = last->pc;

      if (gdbarch_insn_is_call (gdbarch, pc))
	break;
    }

  return bfun;
}

/* Add a continuation segment for a function into which we return.
   PREV is the chronologically preceding function segment.
   MFUN and FUN are the symbol information we have for this function.  */

static struct btrace_function *
ftrace_new_return (struct gdbarch *gdbarch,
		   struct btrace_function *prev,
		   struct minimal_symbol *mfun,
		   struct symbol *fun)
{
  struct btrace_function *bfun, *caller;

  bfun = ftrace_new_function (prev, mfun, fun);

  /* It is important to start at PREV's caller.  Otherwise, we might find
     PREV itself, if PREV is a recursive function.  */
  caller = ftrace_find_caller (prev->up, mfun, fun);
  if (caller != NULL)
    {
      /* The caller of PREV is the preceding btrace function segment in this
	 function instance.  */
      gdb_assert (caller->segment.next == NULL);

      caller->segment.next = bfun;
      bfun->segment.prev = caller;

      /* Maintain the function level.  */
      bfun->level = caller->level;

      /* Maintain the call stack.  */
      bfun->up = caller->up;
      bfun->flags = caller->flags;

      ftrace_debug (bfun, "new return");
    }
  else
    {
      /* We did not find a caller.  This could mean that something went
	 wrong or that the call is simply not included in the trace.  */

      /* Let's search for some actual call.  */
      caller = ftrace_find_call (gdbarch, prev->up);
      if (caller == NULL)
	{
	  /* There is no call in PREV's back trace.  We assume that the
	     branch trace did not include it.  */

	  /* Let's find the topmost call function - this skips tail calls.  */
	  while (prev->up != NULL)
	    prev = prev->up;

	  /* We maintain levels for a series of returns for which we have
	     not seen the calls.
	     We start at the preceding function's level in case this has
	     already been a return for which we have not seen the call.
	     We start at level 0 otherwise, to handle tail calls correctly.  */
	  bfun->level = min (0, prev->level) - 1;

	  /* Fix up the call stack for PREV.  */
	  ftrace_fixup_caller (prev, bfun, BFUN_UP_LINKS_TO_RET);

	  ftrace_debug (bfun, "new return - no caller");
	}
      else
	{
	  /* There is a call in PREV's back trace to which we should have
	     returned.  Let's remain at this level.  */
	  bfun->level = prev->level;

	  ftrace_debug (bfun, "new return - unknown caller");
	}
    }

  return bfun;
}

/* Add a new function segment for a function switch.
   PREV is the chronologically preceding function segment.
   MFUN and FUN are the symbol information we have for this function.  */

static struct btrace_function *
ftrace_new_switch (struct btrace_function *prev,
		   struct minimal_symbol *mfun,
		   struct symbol *fun)
{
  struct btrace_function *bfun;

  /* This is an unexplained function switch.  The call stack will likely
     be wrong at this point.  */
  bfun = ftrace_new_function (prev, mfun, fun);

  /* We keep the function level.  */
  bfun->level = prev->level;

  ftrace_debug (bfun, "new switch");

  return bfun;
}

/* Update BFUN with respect to the instruction at PC.  This may create new
   function segments.
   Return the chronologically latest function segment, never NULL.  */

static struct btrace_function *
ftrace_update_function (struct gdbarch *gdbarch,
			struct btrace_function *bfun, CORE_ADDR pc)
{
  struct bound_minimal_symbol bmfun;
  struct minimal_symbol *mfun;
  struct symbol *fun;
  struct btrace_insn *last;

  /* Try to determine the function we're in.  We use both types of symbols
     to avoid surprises when we sometimes get a full symbol and sometimes
     only a minimal symbol.  */
  fun = find_pc_function (pc);
  bmfun = lookup_minimal_symbol_by_pc (pc);
  mfun = bmfun.minsym;

  if (fun == NULL && mfun == NULL)
    DEBUG_FTRACE ("no symbol at %s", core_addr_to_string_nz (pc));

  /* If we didn't have a function before, we create one.  */
  if (bfun == NULL)
    return ftrace_new_function (bfun, mfun, fun);

  /* Check the last instruction, if we have one.
     We do this check first, since it allows us to fill in the call stack
     links in addition to the normal flow links.  */
  last = NULL;
  if (!VEC_empty (btrace_insn_s, bfun->insn))
    last = VEC_last (btrace_insn_s, bfun->insn);

  if (last != NULL)
    {
      CORE_ADDR lpc;

      lpc = last->pc;

      /* Check for returns.  */
      if (gdbarch_insn_is_ret (gdbarch, lpc))
	return ftrace_new_return (gdbarch, bfun, mfun, fun);

      /* Check for calls.  */
      if (gdbarch_insn_is_call (gdbarch, lpc))
	{
	  int size;

	  size = gdb_insn_length (gdbarch, lpc);

	  /* Ignore calls to the next instruction.  They are used for PIC.  */
	  if (lpc + size != pc)
	    return ftrace_new_call (bfun, mfun, fun);
	}
    }

  /* Check if we're switching functions for some other reason.  */
  if (ftrace_function_switched (bfun, mfun, fun))
    {
      DEBUG_FTRACE ("switching from %s in %s at %s",
		    ftrace_print_insn_addr (last),
		    ftrace_print_function_name (bfun),
		    ftrace_print_filename (bfun));

      if (last != NULL)
	{
	  CORE_ADDR start, lpc;

	  start = get_pc_function_start (pc);

	  /* If we can't determine the function for PC, we treat a jump at
	     the end of the block as tail call.  */
	  if (start == 0)
	    start = pc;

	  lpc = last->pc;

	  /* Jumps indicate optimized tail calls.  */
	  if (start == pc && gdbarch_insn_is_jump (gdbarch, lpc))
	    return ftrace_new_tailcall (bfun, mfun, fun);
	}

      return ftrace_new_switch (bfun, mfun, fun);
    }

  return bfun;
}

/* Update BFUN's source range with respect to the instruction at PC.  */

static void
ftrace_update_lines (struct btrace_function *bfun, CORE_ADDR pc)
{
  struct symtab_and_line sal;
  const char *fullname;

  sal = find_pc_line (pc, 0);
  if (sal.symtab == NULL || sal.line == 0)
    {
      DEBUG_FTRACE ("no lines at %s", core_addr_to_string_nz (pc));
      return;
    }

  /* Check if we switched files.  This could happen if, say, a macro that
     is defined in another file is expanded here.  */
  fullname = symtab_to_fullname (sal.symtab);
  if (ftrace_skip_file (bfun, fullname))
    {
      DEBUG_FTRACE ("ignoring file at %s, file=%s",
		    core_addr_to_string_nz (pc), fullname);
      return;
    }

  /* Update the line range.  */
  bfun->lbegin = min (bfun->lbegin, sal.line);
  bfun->lend = max (bfun->lend, sal.line);

  if (record_debug > 1)
    ftrace_debug (bfun, "update lines");
}

/* Add the instruction at PC to BFUN's instructions.  */

static void
ftrace_update_insns (struct btrace_function *bfun, CORE_ADDR pc)
{
  struct btrace_insn *insn;

  insn = VEC_safe_push (btrace_insn_s, bfun->insn, NULL);
  insn->pc = pc;

  if (record_debug > 1)
    ftrace_debug (bfun, "update insn");
}

/* Compute the function branch trace from a block branch trace BTRACE for
   a thread given by BTINFO.  */

static void
btrace_compute_ftrace (struct btrace_thread_info *btinfo,
		       VEC (btrace_block_s) *btrace)
{
  struct btrace_function *begin, *end;
  struct gdbarch *gdbarch;
  unsigned int blk;
  int level;

  DEBUG ("compute ftrace");

  gdbarch = target_gdbarch ();
  begin = NULL;
  end = NULL;
  level = INT_MAX;
  blk = VEC_length (btrace_block_s, btrace);

  while (blk != 0)
    {
      btrace_block_s *block;
      CORE_ADDR pc;

      blk -= 1;

      block = VEC_index (btrace_block_s, btrace, blk);
      pc = block->begin;

      for (;;)
	{
	  int size;

	  /* We should hit the end of the block.  Warn if we went too far.  */
	  if (block->end < pc)
	    {
	      warning (_("Recorded trace may be corrupted around %s."),
		       core_addr_to_string_nz (pc));
	      break;
	    }

	  end = ftrace_update_function (gdbarch, end, pc);
	  if (begin == NULL)
	    begin = end;

	  /* Maintain the function level offset.
	     For all but the last block, we do it here.  */
	  if (blk != 0)
	    level = min (level, end->level);

	  ftrace_update_insns (end, pc);
	  ftrace_update_lines (end, pc);

	  /* We're done once we pushed the instruction at the end.  */
	  if (block->end == pc)
	    break;

	  size = gdb_insn_length (gdbarch, pc);

	  /* Make sure we terminate if we fail to compute the size.  */
	  if (size <= 0)
	    {
	      warning (_("Recorded trace may be incomplete around %s."),
		       core_addr_to_string_nz (pc));
	      break;
	    }

	  pc += size;

	  /* Maintain the function level offset.
	     For the last block, we do it here to not consider the last
	     instruction.
	     Since the last instruction corresponds to the current instruction
	     and is not really part of the execution history, it shouldn't
	     affect the level.  */
	  if (blk == 0)
	    level = min (level, end->level);
	}
    }

  btinfo->begin = begin;
  btinfo->end = end;

  /* LEVEL is the minimal function level of all btrace function segments.
     Define the global level offset to -LEVEL so all function levels are
     normalized to start at zero.  */
  btinfo->level = -level;
}

/* See btrace.h.  */

void
btrace_enable (struct thread_info *tp)
{
  if (tp->btrace.target != NULL)
    return;

  if (!target_supports_btrace ())
    error (_("Target does not support branch tracing."));

  DEBUG ("enable thread %d (%s)", tp->num, target_pid_to_str (tp->ptid));

  tp->btrace.target = target_enable_btrace (tp->ptid);
}

/* See btrace.h.  */

void
btrace_disable (struct thread_info *tp)
{
  struct btrace_thread_info *btp = &tp->btrace;
  int errcode = 0;

  if (btp->target == NULL)
    return;

  DEBUG ("disable thread %d (%s)", tp->num, target_pid_to_str (tp->ptid));

  target_disable_btrace (btp->target);
  btp->target = NULL;

  btrace_clear (tp);
}

/* See btrace.h.  */

void
btrace_teardown (struct thread_info *tp)
{
  struct btrace_thread_info *btp = &tp->btrace;
  int errcode = 0;

  if (btp->target == NULL)
    return;

  DEBUG ("teardown thread %d (%s)", tp->num, target_pid_to_str (tp->ptid));

  target_teardown_btrace (btp->target);
  btp->target = NULL;

  btrace_clear (tp);
}

/* See btrace.h.  */

void
btrace_fetch (struct thread_info *tp)
{
  struct btrace_thread_info *btinfo;
  VEC (btrace_block_s) *btrace;
  struct cleanup *cleanup;

  DEBUG ("fetch thread %d (%s)", tp->num, target_pid_to_str (tp->ptid));

  btinfo = &tp->btrace;
  if (btinfo->target == NULL)
    return;

  btrace = target_read_btrace (btinfo->target, BTRACE_READ_NEW);
  cleanup = make_cleanup (VEC_cleanup (btrace_block_s), &btrace);

  if (!VEC_empty (btrace_block_s, btrace))
    {
      btrace_clear (tp);
      btrace_compute_ftrace (btinfo, btrace);
    }

  do_cleanups (cleanup);
}

/* See btrace.h.  */

void
btrace_clear (struct thread_info *tp)
{
  struct btrace_thread_info *btinfo;
  struct btrace_function *it, *trash;

  DEBUG ("clear thread %d (%s)", tp->num, target_pid_to_str (tp->ptid));

  btinfo = &tp->btrace;

  it = btinfo->begin;
  while (it != NULL)
    {
      trash = it;
      it = it->flow.next;

      xfree (trash);
    }

  btinfo->begin = NULL;
  btinfo->end = NULL;

  xfree (btinfo->insn_history);
  xfree (btinfo->call_history);

  btinfo->insn_history = NULL;
  btinfo->call_history = NULL;
}

/* See btrace.h.  */

void
btrace_free_objfile (struct objfile *objfile)
{
  struct thread_info *tp;

  DEBUG ("free objfile");

  ALL_THREADS (tp)
    btrace_clear (tp);
}

#if defined (HAVE_LIBEXPAT)

/* Check the btrace document version.  */

static void
check_xml_btrace_version (struct gdb_xml_parser *parser,
			  const struct gdb_xml_element *element,
			  void *user_data, VEC (gdb_xml_value_s) *attributes)
{
  const char *version = xml_find_attribute (attributes, "version")->value;

  if (strcmp (version, "1.0") != 0)
    gdb_xml_error (parser, _("Unsupported btrace version: \"%s\""), version);
}

/* Parse a btrace "block" xml record.  */

static void
parse_xml_btrace_block (struct gdb_xml_parser *parser,
			const struct gdb_xml_element *element,
			void *user_data, VEC (gdb_xml_value_s) *attributes)
{
  VEC (btrace_block_s) **btrace;
  struct btrace_block *block;
  ULONGEST *begin, *end;

  btrace = user_data;
  block = VEC_safe_push (btrace_block_s, *btrace, NULL);

  begin = xml_find_attribute (attributes, "begin")->value;
  end = xml_find_attribute (attributes, "end")->value;

  block->begin = *begin;
  block->end = *end;
}

static const struct gdb_xml_attribute block_attributes[] = {
  { "begin", GDB_XML_AF_NONE, gdb_xml_parse_attr_ulongest, NULL },
  { "end", GDB_XML_AF_NONE, gdb_xml_parse_attr_ulongest, NULL },
  { NULL, GDB_XML_AF_NONE, NULL, NULL }
};

static const struct gdb_xml_attribute btrace_attributes[] = {
  { "version", GDB_XML_AF_NONE, NULL, NULL },
  { NULL, GDB_XML_AF_NONE, NULL, NULL }
};

static const struct gdb_xml_element btrace_children[] = {
  { "block", block_attributes, NULL,
    GDB_XML_EF_REPEATABLE | GDB_XML_EF_OPTIONAL, parse_xml_btrace_block, NULL },
  { NULL, NULL, NULL, GDB_XML_EF_NONE, NULL, NULL }
};

static const struct gdb_xml_element btrace_elements[] = {
  { "btrace", btrace_attributes, btrace_children, GDB_XML_EF_NONE,
    check_xml_btrace_version, NULL },
  { NULL, NULL, NULL, GDB_XML_EF_NONE, NULL, NULL }
};

#endif /* defined (HAVE_LIBEXPAT) */

/* See btrace.h.  */

VEC (btrace_block_s) *
parse_xml_btrace (const char *buffer)
{
  VEC (btrace_block_s) *btrace = NULL;
  struct cleanup *cleanup;
  int errcode;

#if defined (HAVE_LIBEXPAT)

  cleanup = make_cleanup (VEC_cleanup (btrace_block_s), &btrace);
  errcode = gdb_xml_parse_quick (_("btrace"), "btrace.dtd", btrace_elements,
				 buffer, &btrace);
  if (errcode != 0)
    {
      do_cleanups (cleanup);
      return NULL;
    }

  /* Keep parse results.  */
  discard_cleanups (cleanup);

#else  /* !defined (HAVE_LIBEXPAT) */

  error (_("Cannot process branch trace.  XML parsing is not supported."));

#endif  /* !defined (HAVE_LIBEXPAT) */

  return btrace;
}

/* See btrace.h.  */

const struct btrace_insn *
btrace_insn_get (const struct btrace_insn_iterator *it)
{
  const struct btrace_function *bfun;
  unsigned int index, end;

  index = it->index;
  bfun = it->function;

  /* The index is within the bounds of this function's instruction vector.  */
  end = VEC_length (btrace_insn_s, bfun->insn);
  gdb_assert (0 < end);
  gdb_assert (index < end);

  return VEC_index (btrace_insn_s, bfun->insn, index);
}

/* See btrace.h.  */

unsigned int
btrace_insn_number (const struct btrace_insn_iterator *it)
{
  const struct btrace_function *bfun;

  bfun = it->function;
  return bfun->insn_offset + it->index;
}

/* See btrace.h.  */

void
btrace_insn_begin (struct btrace_insn_iterator *it,
		   const struct btrace_thread_info *btinfo)
{
  const struct btrace_function *bfun;

  bfun = btinfo->begin;
  if (bfun == NULL)
    error (_("No trace."));

  it->function = bfun;
  it->index = 0;
}

/* See btrace.h.  */

void
btrace_insn_end (struct btrace_insn_iterator *it,
		 const struct btrace_thread_info *btinfo)
{
  const struct btrace_function *bfun;
  unsigned int length;

  bfun = btinfo->end;
  if (bfun == NULL)
    error (_("No trace."));

  /* The last instruction in the last function is the current instruction.
     We point to it - it is one past the end of the execution trace.  */
  length = VEC_length (btrace_insn_s, bfun->insn);

  it->function = bfun;
  it->index = length - 1;
}

/* See btrace.h.  */

unsigned int
btrace_insn_next (struct btrace_insn_iterator *it, unsigned int stride)
{
  const struct btrace_function *bfun;
  unsigned int index, steps;

  bfun = it->function;
  steps = 0;
  index = it->index;

  while (stride != 0)
    {
      unsigned int end, space, adv;

      end = VEC_length (btrace_insn_s, bfun->insn);

      gdb_assert (0 < end);
      gdb_assert (index < end);

      /* Compute the number of instructions remaining in this segment.  */
      space = end - index;

      /* Advance the iterator as far as possible within this segment.  */
      adv = min (space, stride);
      stride -= adv;
      index += adv;
      steps += adv;

      /* Move to the next function if we're at the end of this one.  */
      if (index == end)
	{
	  const struct btrace_function *next;

	  next = bfun->flow.next;
	  if (next == NULL)
	    {
	      /* We stepped past the last function.

		 Let's adjust the index to point to the last instruction in
		 the previous function.  */
	      index -= 1;
	      steps -= 1;
	      break;
	    }

	  /* We now point to the first instruction in the new function.  */
	  bfun = next;
	  index = 0;
	}

      /* We did make progress.  */
      gdb_assert (adv > 0);
    }

  /* Update the iterator.  */
  it->function = bfun;
  it->index = index;

  return steps;
}

/* See btrace.h.  */

unsigned int
btrace_insn_prev (struct btrace_insn_iterator *it, unsigned int stride)
{
  const struct btrace_function *bfun;
  unsigned int index, steps;

  bfun = it->function;
  steps = 0;
  index = it->index;

  while (stride != 0)
    {
      unsigned int adv;

      /* Move to the previous function if we're at the start of this one.  */
      if (index == 0)
	{
	  const struct btrace_function *prev;

	  prev = bfun->flow.prev;
	  if (prev == NULL)
	    break;

	  /* We point to one after the last instruction in the new function.  */
	  bfun = prev;
	  index = VEC_length (btrace_insn_s, bfun->insn);

	  /* There is at least one instruction in this function segment.  */
	  gdb_assert (index > 0);
	}

      /* Advance the iterator as far as possible within this segment.  */
      adv = min (index, stride);
      stride -= adv;
      index -= adv;
      steps += adv;

      /* We did make progress.  */
      gdb_assert (adv > 0);
    }

  /* Update the iterator.  */
  it->function = bfun;
  it->index = index;

  return steps;
}

/* See btrace.h.  */

int
btrace_insn_cmp (const struct btrace_insn_iterator *lhs,
		 const struct btrace_insn_iterator *rhs)
{
  unsigned int lnum, rnum;

  lnum = btrace_insn_number (lhs);
  rnum = btrace_insn_number (rhs);

  return (int) (lnum - rnum);
}

/* See btrace.h.  */

int
btrace_find_insn_by_number (struct btrace_insn_iterator *it,
			    const struct btrace_thread_info *btinfo,
			    unsigned int number)
{
  const struct btrace_function *bfun;
  unsigned int end;

  for (bfun = btinfo->end; bfun != NULL; bfun = bfun->flow.prev)
    if (bfun->insn_offset <= number)
      break;

  if (bfun == NULL)
    return 0;

  end = bfun->insn_offset + VEC_length (btrace_insn_s, bfun->insn);
  if (end <= number)
    return 0;

  it->function = bfun;
  it->index = number - bfun->insn_offset;

  return 1;
}

/* See btrace.h.  */

const struct btrace_function *
btrace_call_get (const struct btrace_call_iterator *it)
{
  return it->function;
}

/* See btrace.h.  */

unsigned int
btrace_call_number (const struct btrace_call_iterator *it)
{
  const struct btrace_thread_info *btinfo;
  const struct btrace_function *bfun;
  unsigned int insns;

  btinfo = it->btinfo;
  bfun = it->function;
  if (bfun != NULL)
    return bfun->number;

  /* For the end iterator, i.e. bfun == NULL, we return one more than the
     number of the last function.  */
  bfun = btinfo->end;
  insns = VEC_length (btrace_insn_s, bfun->insn);

  /* If the function contains only a single instruction (i.e. the current
     instruction), it will be skipped and its number is already the number
     we seek.  */
  if (insns == 1)
    return bfun->number;

  /* Otherwise, return one more than the number of the last function.  */
  return bfun->number + 1;
}

/* See btrace.h.  */

void
btrace_call_begin (struct btrace_call_iterator *it,
		   const struct btrace_thread_info *btinfo)
{
  const struct btrace_function *bfun;

  bfun = btinfo->begin;
  if (bfun == NULL)
    error (_("No trace."));

  it->btinfo = btinfo;
  it->function = bfun;
}

/* See btrace.h.  */

void
btrace_call_end (struct btrace_call_iterator *it,
		 const struct btrace_thread_info *btinfo)
{
  const struct btrace_function *bfun;

  bfun = btinfo->end;
  if (bfun == NULL)
    error (_("No trace."));

  it->btinfo = btinfo;
  it->function = NULL;
}

/* See btrace.h.  */

unsigned int
btrace_call_next (struct btrace_call_iterator *it, unsigned int stride)
{
  const struct btrace_function *bfun;
  unsigned int steps;

  bfun = it->function;
  steps = 0;
  while (bfun != NULL)
    {
      const struct btrace_function *next;
      unsigned int insns;

      next = bfun->flow.next;
      if (next == NULL)
	{
	  /* Ignore the last function if it only contains a single
	     (i.e. the current) instruction.  */
	  insns = VEC_length (btrace_insn_s, bfun->insn);
	  if (insns == 1)
	    steps -= 1;
	}

      if (stride == steps)
	break;

      bfun = next;
      steps += 1;
    }

  it->function = bfun;
  return steps;
}

/* See btrace.h.  */

unsigned int
btrace_call_prev (struct btrace_call_iterator *it, unsigned int stride)
{
  const struct btrace_thread_info *btinfo;
  const struct btrace_function *bfun;
  unsigned int steps;

  bfun = it->function;
  steps = 0;

  if (bfun == NULL)
    {
      unsigned int insns;

      btinfo = it->btinfo;
      bfun = btinfo->end;
      if (bfun == NULL)
	return 0;

      /* Ignore the last function if it only contains a single
	 (i.e. the current) instruction.  */
      insns = VEC_length (btrace_insn_s, bfun->insn);
      if (insns == 1)
	bfun = bfun->flow.prev;

      if (bfun == NULL)
	return 0;

      steps += 1;
    }

  while (steps < stride)
    {
      const struct btrace_function *prev;

      prev = bfun->flow.prev;
      if (prev == NULL)
	break;

      bfun = prev;
      steps += 1;
    }

  it->function = bfun;
  return steps;
}

/* See btrace.h.  */

int
btrace_call_cmp (const struct btrace_call_iterator *lhs,
		 const struct btrace_call_iterator *rhs)
{
  unsigned int lnum, rnum;

  lnum = btrace_call_number (lhs);
  rnum = btrace_call_number (rhs);

  return (int) (lnum - rnum);
}

/* See btrace.h.  */

int
btrace_find_call_by_number (struct btrace_call_iterator *it,
			    const struct btrace_thread_info *btinfo,
			    unsigned int number)
{
  const struct btrace_function *bfun;

  for (bfun = btinfo->end; bfun != NULL; bfun = bfun->flow.prev)
    {
      unsigned int bnum;

      bnum = bfun->number;
      if (number == bnum)
	{
	  it->btinfo = btinfo;
	  it->function = bfun;
	  return 1;
	}

      /* Functions are ordered and numbered consecutively.  We could bail out
	 earlier.  On the other hand, it is very unlikely that we search for
	 a nonexistent function.  */
  }

  return 0;
}

/* See btrace.h.  */

void
btrace_set_insn_history (struct btrace_thread_info *btinfo,
			 const struct btrace_insn_iterator *begin,
			 const struct btrace_insn_iterator *end)
{
  if (btinfo->insn_history == NULL)
    btinfo->insn_history = xzalloc (sizeof (*btinfo->insn_history));

  btinfo->insn_history->begin = *begin;
  btinfo->insn_history->end = *end;
}

/* See btrace.h.  */

void
btrace_set_call_history (struct btrace_thread_info *btinfo,
			 const struct btrace_call_iterator *begin,
			 const struct btrace_call_iterator *end)
{
  gdb_assert (begin->btinfo == end->btinfo);

  if (btinfo->call_history == NULL)
    btinfo->call_history = xzalloc (sizeof (*btinfo->call_history));

  btinfo->call_history->begin = *begin;
  btinfo->call_history->end = *end;
}
