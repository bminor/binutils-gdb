/* Work with executable files, for GDB. 
   Copyright (C) 1988, 1989 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "target.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <fcntl.h>

#include "gdbcore.h"

#ifdef STILL_NEEDED_FOR_DECSTATION
#include <sys/dir.h>		/* For DECstations */
#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#endif

#include <sys/stat.h>

extern char *getenv();
extern void child_create_inferior (), child_attach ();
extern void symbol_file_command ();

/* The Binary File Descriptor handle for the executable file.  */

bfd *exec_bfd = NULL;

/* The base and bounds of the table of the exec file's sections.  */

struct section_table *exec_sections, *exec_sections_end;

/* Forward decl */

extern struct target_ops exec_ops;

void
exec_close (quitting)
     int quitting;
{
  if (exec_bfd) {
    bfd_close (exec_bfd);
    exec_bfd = NULL;
  }
}

void
exec_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{

  /* Remove any previous exec file.  */
  unpush_target (&exec_ops);

  /* Now open and digest the file the user requested, if any.  */

  if (filename)
    {
      char *scratch_pathname;
      int scratch_chan;
      
      filename = tilde_expand (filename);
      make_cleanup (free, filename);
      
/* FIXME, if writeable is set, open for read/write. */
      scratch_chan = openp (getenv ("PATH"), 1, filename, O_RDONLY, 0,
			    &scratch_pathname);
      if (scratch_chan < 0)
	perror_with_name (filename);

      exec_bfd = bfd_fdopenr (scratch_pathname, NULL, scratch_chan);
      if (!exec_bfd)
	error ("Could not open `%s' as an executable file: %s",
	       scratch_pathname, bfd_errmsg (bfd_error));
      if (!bfd_check_format (exec_bfd, bfd_object))
	error ("\"%s\": not in executable format: %s.",
	       scratch_pathname, bfd_errmsg (bfd_error));

#if FIXME
/* This code needs to be incorporated into BFD */
#ifdef COFF_ENCAPSULATE
	/* If we have a coff header, it can give us better values for
	   text_start and exec_data_start.  This is particularly useful
	   for remote debugging of embedded systems.  */
	if (N_FLAGS(exec_aouthdr) & N_FLAGS_COFF_ENCAPSULATE)
	{
		struct coffheader ch;
		int val;
		val = lseek (execchan, -(sizeof (AOUTHDR) + sizeof (ch)), 1);
		if (val == -1)
			perror_with_name (filename);
		val = myread (execchan, &ch, sizeof (ch));
		if (val < 0)
			perror_with_name (filename);
		text_start = ch.text_start;
		exec_data_start = ch.data_start;
	} else
#endif
	       {
		text_start =
		  IS_OBJECT_FILE (exec_aouthdr) ? 0 : N_TXTADDR (exec_aouthdr);
		exec_data_start = IS_OBJECT_FILE (exec_aouthdr)
		  ? exec_aouthdr.a_text : N_DATADDR (exec_aouthdr);
	}
#endif FIXME

      if (build_section_table (exec_bfd, &exec_sections, &exec_sections_end))
	error ("Can't find the file sections in `%s': %s", 
		exec_bfd->filename, bfd_errmsg (bfd_error));

      validate_files ();

      push_target (&exec_ops);

      /* Tell display code (if any) about the changed file name.  */
      if (exec_file_display_hook)
	(*exec_file_display_hook) (filename);
    }
  else if (from_tty)
    printf ("No exec file now.\n");
}

/* Set both the exec file and the symbol file, in one command.  
   What a novelty.  Why did GDB go through four major releases before this
   command was added?  */

void
file_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  /* FIXME, if we lose on reading the symbol file, we should revert
     the exec file, but that's rough.  */
  exec_file_command (arg, from_tty);
  symbol_file_command (arg, from_tty);
}


/* Locate all mappable sections of a BFD file.  */

void
add_to_section_table (abfd, asect, table_pp)
     bfd *abfd;
     sec_ptr asect;
     struct section_table **table_pp;
{
  flagword aflag;

  aflag = bfd_get_section_flags (abfd, asect);
  /* FIXME, we need to handle BSS segment here...it alloc's but doesn't load */
  if (!(aflag & SEC_LOAD))
    return;
  (*table_pp)->sec_ptr = asect;
  (*table_pp)->addr = bfd_section_vma (abfd, asect);
  (*table_pp)->endaddr = (*table_pp)->addr + bfd_section_size (abfd, asect);
  (*table_pp)++;
}

int
build_section_table (some_bfd, start, end)
     bfd *some_bfd;
     struct section_table **start, **end;
{
  unsigned count;

  count = bfd_count_sections (some_bfd);
  if (count == 0)
    abort();	/* return 1? */
  *start = (struct section_table *) xmalloc (count * sizeof (**start));
  *end = *start;
  bfd_map_over_sections (some_bfd, add_to_section_table, end);
  if (*end > *start + count)
    abort();
  /* We could realloc the table, but it probably loses for most files.  */
  return 0;
}

/* Read or write the exec file.

   Args are address within exec file, address within gdb address-space,
   length, and a flag indicating whether to read or write.

   Result is a length:

	0:    We cannot handle this address and length.
	> 0:  We have handled N bytes starting at this address.
	      (If N == length, we did it all.)  We might be able
	      to handle more bytes beyond this length, but no
	      promises.
	< 0:  We cannot handle this address, but if somebody
	      else handles (-N) bytes, we can start from there.

    The same routine is used to handle both core and exec files;
    we just tail-call it with more arguments to select between them.  */

int
xfer_memory (memaddr, myaddr, len, write, abfd, sections, sections_end)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     bfd *abfd;
     struct section_table *sections, *sections_end;
{
  boolean res;
  struct section_table *p;
  CORE_ADDR nextsectaddr, memend;
  boolean (*xfer_fn) ();

  if (len <= 0)
    abort();

  memend = memaddr + len;
  xfer_fn = write? bfd_set_section_contents: bfd_get_section_contents;
  nextsectaddr = memend;

  for (p = sections; p < sections_end; p++)
    {
      if (p->addr <= memaddr)
	if (p->endaddr >= memend)
	  {
	    /* Entire transfer is within this section.  */
	    res = xfer_fn (abfd, p->sec_ptr, myaddr, memaddr - p->addr, len);
	    return (res != false)? len: 0;
	  }
	else if (p->endaddr <= memaddr)
	  {
	    /* This section ends before the transfer starts.  */
	    continue;
	  }
	else 
	  {
	    /* This section overlaps the transfer.  Just do half.  */
	    len = p->endaddr - memaddr;
	    res = xfer_fn (abfd, p->sec_ptr, myaddr, memaddr - p->addr, len);
	    return (res != false)? len: 0;
	  }
      else if (p->addr < nextsectaddr)
	nextsectaddr = p->addr;
    }

  if (nextsectaddr >= memend)
    return 0;				/* We can't help */
  else
    return - (nextsectaddr - memaddr);	/* Next boundary where we can help */
}

/* The function called by target_xfer_memory via our target_ops */

int
exec_xfer_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{
  return xfer_memory (memaddr, myaddr, len, write,
		      exec_bfd, exec_sections, exec_sections_end);
}


#ifdef FIXME
#ifdef REG_STACK_SEGMENT
/* MOVE TO BFD... */
    /* Pyramids and AM29000s have an extra segment in the virtual address space
       for the (control) stack of register-window frames.  The AM29000 folk
       call it the "register stack" rather than the "memory stack".  */
    else if (memaddr >= reg_stack_start && memaddr < reg_stack_end)
      {
	i = min (len, reg_stack_end - memaddr);
	fileptr = memaddr - reg_stack_start + reg_stack_offset;
	wanna_xfer = coredata;
      }
#endif				/* REG_STACK_SEGMENT */
#endif FIXME

static void
exec_files_info ()
{
  struct section_table *p;

  printf ("\tExecutable file `%s'.\n", bfd_get_filename(exec_bfd));

  for (p = exec_sections; p < exec_sections_end; p++)
    printf("\texecutable from 0x%08x to 0x%08x is %s\n",
	p->addr, p->endaddr,
	bfd_section_name (exec_bfd, p->sec_ptr));
}

struct target_ops exec_ops = {
	"exec", "Local exec file",
	exec_file_command, exec_close, /* open, close */
	child_attach, 0, 0, 0, /* attach, detach, resume, wait, */
	0, 0, /* fetch_registers, store_registers, */
	0, 0, 0, /* prepare_to_store, conv_to, conv_from, */
	exec_xfer_memory, exec_files_info,
	0, 0, /* insert_breakpoint, remove_breakpoint, */
	0, 0, 0, 0, 0, /* terminal stuff */
	0, 0, /* kill, load */
	add_syms_addr_command,
	0, 0, /* call fn, lookup sym */
	child_create_inferior,
	0, /* mourn_inferior */
	file_stratum, 0, /* next */
	0, 1, 0, 0, 0,	/* all mem, mem, stack, regs, exec */
	OPS_MAGIC,		/* Always the last thing */
};

void
_initialize_exec()
{

  add_com ("file", class_files, file_command,
	   "Use FILE as program to be debugged.\n\
It is read for its symbols, for getting the contents of pure memory,\n\
and it is the program executed when you use the `run' command.\n\
If FILE cannot be found as specified, your execution directory path\n\
($PATH) is searched for a command of that name.\n\
No arg means to have no executable file and no symbols.");

  add_com ("exec-file", class_files, exec_file_command,
	   "Use FILE as program for getting contents of pure memory.\n\
If FILE cannot be found as specified, your execution directory path\n\
is searched for a command of that name.\n\
No arg means have no executable file.");

  add_target (&exec_ops);
}
