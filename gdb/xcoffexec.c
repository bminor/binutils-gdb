/* Execute AIXcoff files, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994 Free Software Foundation, Inc.
   Derived from exec.c.  Modified by IBM Corporation.
   Donated by IBM Corporation and Cygnus Support.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* xcoff-exec -	deal with executing XCOFF files.  */
  
#include "defs.h"

#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "frame.h"
#include "inferior.h"
#include "target.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "language.h"
#include "symfile.h"
#include "objfiles.h"

#include "bfd.h"
#include "xcoffsolib.h"

/* Prototypes for local functions */

static void
file_command PARAMS ((char *, int));

static void
exec_close PARAMS ((int));

struct vmap *
map_vmap PARAMS ((bfd *, bfd *));

struct section_table *exec_sections, *exec_sections_end;

/* Whether to open exec and core files read-only or read-write.  */

int write_files = 0;

extern int info_verbose;

bfd *exec_bfd;			/* needed by core.c	*/

extern char *getenv();
extern void add_syms_addr_command ();
extern void symbol_file_command ();
static void exec_files_info();

struct vmap *vmap;		/* current vmap */

extern struct target_ops exec_ops;

/* exec_close -	done with exec file, clean up all resources. */

static void
exec_close (quitting)
int quitting;
{
  register struct vmap *vp, *nxt;
  int need_symtab_cleanup = 0;
  
  for (nxt = vmap; nxt; )
    {
      vp = nxt;
      nxt = vp->nxt;

      /* if there is an objfile associated with this bfd,
	 free_objfile() will do proper cleanup of objfile *and* bfd. */
		   
      if (vp->objfile)
	{
	  free_objfile (vp->objfile);
	  need_symtab_cleanup = 1;
	}
      else
	bfd_close (vp->bfd);

      /* FIXME: This routine is #if 0'd in symfile.c.  What should we
	 be doing here?  Should we just free everything in
	 vp->objfile->symtabs?  Should free_objfile do that?  */
      free_named_symtabs (vp->name);
      free (vp);
    }
  
  vmap = 0;

  /* exec_bfd was already closed (the exec file has a vmap entry).  */
  exec_bfd = NULL;

  if (exec_ops.to_sections)
    {
      free (exec_ops.to_sections);
      exec_ops.to_sections = NULL;
      exec_ops.to_sections_end = NULL;
    }

  if (need_symtab_cleanup)
    clear_symtab_users ();
}

/*  Process the first arg in ARGS as the new exec file.

    Note that we have to explicitly ignore additional args, since we can
    be called from file_command(), which also calls symbol_file_command()
    which can take multiple args. */

void
exec_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  target_preopen (from_tty);

  /* Remove any previous exec file.  */
  unpush_target (&exec_ops);

  /* Now open and digest the file the user requested, if any. */

  if (filename)
    {
      char *scratch_pathname;
      int scratch_chan;
      
      filename = tilde_expand (filename);
      make_cleanup (free, filename);
      
      scratch_chan = openp (getenv ("PATH"), 1, filename,
			    write_files? O_RDWR: O_RDONLY, 0,
			    &scratch_pathname);
      if (scratch_chan < 0)
	perror_with_name (filename);

      exec_bfd = bfd_fdopenr (scratch_pathname, gnutarget, scratch_chan);
      if (!exec_bfd)
	error ("Could not open `%s' as an executable file: %s",
	       scratch_pathname, bfd_errmsg(bfd_get_error ()));

      /* make sure we have an object file */

      if (!bfd_check_format (exec_bfd, bfd_object))
	error ("\"%s\": not in executable format: %s.", scratch_pathname,
	       bfd_errmsg (bfd_get_error ()));

      /* setup initial vmap */

      map_vmap (exec_bfd, 0);
      if (!vmap)
	error ("Can't find the file sections in `%s': %s", exec_bfd->filename,
	       bfd_errmsg(bfd_get_error ()));

      if (build_section_table (exec_bfd, &exec_ops.to_sections,
			       &exec_ops.to_sections_end))
	error ("Can't find the file sections in `%s': %s", exec_bfd->filename,
	       bfd_errmsg (bfd_get_error ()));

      /* make sure core, if present, matches */
      validate_files ();

      push_target(&exec_ops);

      /* Tell display code (if any) about the changed file name. */
      if (exec_file_display_hook)
	(*exec_file_display_hook) (filename);
    } 
  else
    {
      exec_close (0);		/* just in case	*/
      if (from_tty)
	printf_unfiltered ("No exec file now.\n");
    }
}

/* Set both the exec file and the symbol file, in one command.  What a
   novelty.  Why did GDB go through four major releases before this
   command was added?  */

static void
file_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  /* FIXME, if we lose on reading the symbol file, we should revert
     the exec file, but that's rough.  */
  exec_file_command (arg, from_tty);
  symbol_file_command (arg, from_tty);
}

/* Locate all mappable sections of a BFD file. 
   table_pp_char is a char * to get it through bfd_map_over_sections;
   we cast it back to its proper type.  */

static void
add_to_section_table (abfd, asect, table_pp_char)
     bfd *abfd;
     sec_ptr asect;
     char *table_pp_char;
{
  struct section_table **table_pp = (struct section_table **)table_pp_char;
  flagword aflag;

  aflag = bfd_get_section_flags (abfd, asect);
  /* FIXME, we need to handle BSS segment here...it alloc's but doesn't load */
  if (!(aflag & SEC_LOAD))
    return;
  if (0 == bfd_section_size (abfd, asect))
    return;
  (*table_pp)->bfd = abfd;
  (*table_pp)->the_bfd_section = asect;
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
    fatal ("aborting");	/* return 1? */
  if (*start)
    free (*start);
  *start = (struct section_table *) xmalloc (count * sizeof (**start));
  *end = *start;
  bfd_map_over_sections (some_bfd, add_to_section_table, (char *) end);
  if (*end > *start + count)
    fatal ("aborting");
  /* We could realloc the table, but it probably loses for most files.  */
  return 0;
}

static void
bfdsec_to_vmap(bf, sect, arg3) 
     bfd *bf;
     sec_ptr sect;
     PTR arg3;
{
  struct vmap_and_bfd *vmap_bfd = (struct vmap_and_bfd *) arg3;
  register struct vmap *vp;
  vp = vmap_bfd->pvmap;

  if ((bfd_get_section_flags (bf, sect) & SEC_LOAD) == 0)
    return;

  if (STREQ(bfd_section_name (bf, sect), ".text"))
    {
      vp->tstart = 0;
      vp->tend = vp->tstart + bfd_section_size (bf, sect);

      /* When it comes to this adjustment value, in contrast to our previous
	 belief shared objects should behave the same as the main load segment.
	 This is the offset from the beginning of text section to the first
	 real instruction. */

      vp->tadj = sect->filepos - bfd_section_vma (bf, sect);
    }
  else if (STREQ(bfd_section_name (bf, sect), ".data"))
    {
      vp->dstart = 0;
      vp->dend   = vp->dstart + bfd_section_size (bf, sect);
    }
  else if (STREQ(bfd_section_name(bf, sect), ".bss"))	/* FIXMEmgo */
    printf_unfiltered ("bss section in exec! Don't know what the heck to do!\n");
}

/* Make a vmap for the BFD "bf", which might be a member of the archive
   BFD "arch".  Return the new vmap.  */

struct vmap *
map_vmap (bf, arch)
     bfd *bf;
     bfd *arch;
{
  struct vmap_and_bfd vmap_bfd;
  struct vmap *vp, **vpp;

  vp = (PTR) xmalloc (sizeof (*vp));
  memset (vp, '\0', sizeof (*vp));
  vp->nxt = 0;
  vp->bfd = bf;
  vp->name = bfd_get_filename (arch ? arch : bf);
  vp->member = arch ? bfd_get_filename (bf) : "";
  
  vmap_bfd.pbfd = arch;
  vmap_bfd.pvmap = vp;
  bfd_map_over_sections (bf, bfdsec_to_vmap, &vmap_bfd);

  /* find the end of the list, and append. */
  for (vpp = &vmap; *vpp; vpp = &(*vpp)->nxt)
  ;
  *vpp = vp;

  return vp;
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
xfer_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;
{
  boolean res;
  struct section_table *p;
  CORE_ADDR nextsectaddr, memend;
  boolean (*xfer_fn) PARAMS ((bfd *, sec_ptr, PTR, file_ptr, bfd_size_type));

  if (len <= 0)
    fatal ("aborting");

  memend = memaddr + len;
  xfer_fn = write? bfd_set_section_contents: bfd_get_section_contents;
  nextsectaddr = memend;

  for (p = target->to_sections; p < target->to_sections_end; p++)
    {
      if (p->addr <= memaddr)
	if (p->endaddr >= memend)
	  {
	    /* Entire transfer is within this section.  */
	    res = xfer_fn (p->bfd, p->the_bfd_section, myaddr, memaddr - p->addr, len);
	    return (res != 0) ? len : 0;
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
	    res = xfer_fn (p->bfd, p->the_bfd_section, myaddr, memaddr - p->addr, len);
	    return (res != 0) ? len : 0;
	  }
      else if (p->addr < nextsectaddr)
	nextsectaddr = p->addr;
    }

  if (nextsectaddr >= memend)
    return 0;				/* We can't help */
  else
    return - (nextsectaddr - memaddr);	/* Next boundary where we can help */
}

void
print_section_info (t, abfd)
  struct target_ops *t;
  bfd *abfd;
{
  struct section_table *p;

  /* FIXME-32x64: Need a version of print_address_numeric with field width.  */
  printf_filtered ("\t`%s', ", bfd_get_filename(abfd));
  wrap_here ("        ");
  printf_filtered ("file type %s.\n", bfd_get_target(abfd));

  for (p = t->to_sections; p < t->to_sections_end; p++) {
    printf_filtered ("\t%s",
		     local_hex_string_custom ((unsigned long) p->addr, "08l"));
    printf_filtered (" - %s",
		     local_hex_string_custom ((unsigned long) p->endaddr, "08l"));
    if (info_verbose)
      printf_filtered (" @ %s",
		       local_hex_string_custom ((unsigned long) p->the_bfd_section->filepos, "08l"));
    printf_filtered (" is %s", bfd_section_name (p->bfd, p->the_bfd_section));
    if (p->bfd != abfd) {
      printf_filtered (" in %s", bfd_get_filename (p->bfd));
    }
    printf_filtered ("\n");
  }
}


static void
exec_files_info (t)
     struct target_ops *t;
{
  register struct vmap *vp = vmap;

  print_section_info (t, exec_bfd);

  if (!vp)
    return;

  printf_unfiltered ("\tMapping info for file `%s'.\n", vp->name);

  printf_unfiltered ("\t  %8.8s   %8.8s   %8.8s   %8.8s %8.8s %s\n",
    "tstart", "tend", "dstart", "dend", "section", "file(member)");

  for (; vp; vp = vp->nxt)
    printf_unfiltered ("\t0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x %s%s%s%s\n",
		       vp->tstart,
		       vp->tend,
		       vp->dstart,
		       vp->dend,
		       vp->name,
		       *vp->member ? "(" : "",
		       vp->member,
		       *vp->member ? ")" : "");
}

#ifdef DAMON
/*  Damon's implementation of set_section_command! It is based on the sex member
  (which is a section pointer from vmap) of vmap.
  We will not have multiple vmap entries (one for each section), rather transmit
  text and data base offsets and fix them at the same time. Elimination of sex
  entry in vmap make this function obsolute, use the one from exec.c. 
  Need further testing!!	FIXMEmgo.  */

static void
set_section_command(args, from_tty)
char *args; 
{
	register struct vmap *vp = vmap;
	char *secname;
	unsigned seclen;
	unsigned long secaddr;
	char secprint[100];
	long offset;

	if (args == 0)
		error("Must specify section name and its virtual address");

	/* Parse out section name */
	for (secname = args; !isspace(*args); args++)
		;
	seclen = args - secname;

	/* Parse out new virtual address */
	secaddr = parse_and_eval_address(args);

	for (vp = vmap; vp; vp = vp->nxt) {
		if (!strncmp(secname
			     , bfd_section_name(vp->bfd, vp->sex), seclen)
		    && bfd_section_name(vp->bfd, vp->sex)[seclen] == '\0') {
			offset = secaddr - vp->tstart;
			vp->tstart += offset;
			vp->tend   += offset;
			exec_files_info();
			return;
		}
	} 

	if (seclen >= sizeof(secprint))
		seclen = sizeof(secprint) - 1;
	strncpy(secprint, secname, seclen);
	secprint[seclen] = '\0';
	error("Section %s not found", secprint);
}
#else
static void
set_section_command (args, from_tty)
     char *args;
     int from_tty;
{
  struct section_table *p;
  char *secname;
  unsigned seclen;
  unsigned long secaddr;
  char secprint[100];
  long offset;

  if (args == 0)
    error ("Must specify section name and its virtual address");

  /* Parse out section name */
  for (secname = args; !isspace(*args); args++) ;
  seclen = args - secname;

  /* Parse out new virtual address */
  secaddr = parse_and_eval_address (args);

  for (p = exec_ops.to_sections; p < exec_ops.to_sections_end; p++)
    if (!strncmp (secname, bfd_section_name (exec_bfd, p->the_bfd_section), seclen)
	&& bfd_section_name (exec_bfd, p->the_bfd_section)[seclen] == '\0')
      {
	offset = secaddr - p->addr;
	p->addr += offset;
	p->endaddr += offset;
	if (from_tty)
	  exec_files_info (&exec_ops);
	return;
      }

  if (seclen >= sizeof (secprint))
    seclen = sizeof (secprint) - 1;
  strncpy (secprint, secname, seclen);
  secprint[seclen] = '\0';
  error ("Section %s not found", secprint);
}

#endif /* !DAMON */

struct target_ops exec_ops = {
	"exec", "Local exec file",
	"Use an executable file as a target.\n\
Specify the filename of the executable file.",
	exec_file_command, exec_close, /* open, close */
	find_default_attach, 0, 0, 0, /* attach, detach, resume, wait, */
	0, 0, /* fetch_registers, store_registers, */
	0, /* prepare_to_store */
	xfer_memory, exec_files_info,
	0, 0, /* insert_breakpoint, remove_breakpoint, */
	0, 0, 0, 0, 0, /* terminal stuff */
	0, 0, /* kill, load */
	0, /* lookup sym */
	find_default_create_inferior,
	0, /* mourn_inferior */
	0, /* can_run */
	0, /* notice_signals */
	file_stratum, 0, /* next */
	0, 1, 0, 0, 0,	/* all mem, mem, stack, regs, exec */
	0, 0,			/* section pointers */
	OPS_MAGIC,		/* Always the last thing */
};

void
_initialize_exec()
{

  add_com("file", class_files, file_command,
	   "Use FILE as program to be debugged.\n\
It is read for its symbols, for getting the contents of pure memory,\n\
and it is the program executed when you use the `run' command.\n\
If FILE cannot be found as specified, your execution directory path\n\
($PATH) is searched for a command of that name.\n\
No arg means to have no executable file and no symbols.");

  add_com("exec-file", class_files, exec_file_command,
	   "Use FILE as program for getting contents of pure memory.\n\
If FILE cannot be found as specified, your execution directory path\n\
is searched for a command of that name.\n\
No arg means have no executable file.");

  add_com("section", class_files, set_section_command,
   "Change the base address of section SECTION of the exec file to ADDR.\n\
This can be used if the exec file does not contain section addresses,\n\
(such as in the a.out format), or when the addresses specified in the\n\
file itself are wrong.  Each section must be changed separately.  The\n\
``info files'' command lists all the sections and their addresses.");

  add_target(&exec_ops);
}
