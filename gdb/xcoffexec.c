/* Execute AIXcoff files, for GDB.
   Copyright 1988, 1989, 1991, 1992 Free Software Foundation, Inc.
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
#include <sys/ldr.h>

#include "frame.h"
#include "inferior.h"
#include "target.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"

#include "libbfd.h"		/* BFD internals (sigh!)  FIXME */
#include "bfd.h"
#include "xcoffsolib.h"

/* Prototypes for local functions */

static void
file_command PARAMS ((char *, int));

static void
exec_close PARAMS ((int));

static struct vmap *
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
exec_close(quitting)
{
  register struct vmap *vp, *nxt;
  struct objfile *obj;
  int need_symtab_cleanup = 0;
  
  for (nxt = vmap; vp = nxt; )
    {
      nxt = vp->nxt;

      /* if there is an objfile associated with this bfd,
	 free_objfile() will do proper cleanup of objfile *and* bfd. */
		   
      if (vp->objfile)
	{
	  free_objfile (vp->objfile);
	  need_symtab_cleanup = 1;
	}
      else
	bfd_close(vp->bfd);

      /* FIXME: This routine is #if 0'd in symfile.c.  What should we
	 be doing here?  Should we just free everything in
	 vp->objfile->symtabs?  Should free_objfile do that?  */
      free_named_symtabs(vp->name);
      free(vp);
    }
  
  vmap = 0;

  /* exec_bfd was already closed (the exec file has a vmap entry).  */
  exec_bfd = NULL;

  if (exec_ops.to_sections) {
    free (exec_ops.to_sections);
    exec_ops.to_sections = NULL;
    exec_ops.to_sections_end = NULL;
  }

  if (need_symtab_cleanup)
    clear_symtab_users ();
}

/*
 * exec_file_command -	handle the "exec" command, &c.
 */
void
exec_file_command(filename, from_tty)
char *filename;
{
  target_preopen(from_tty);

  /* Remove any previous exec file.  */
  unpush_target(&exec_ops);

  /* Now open and digest the file the user requested, if any. */

  if (filename) {
  	char *scratch_pathname;
  	int scratch_chan;
      
  	filename = tilde_expand(filename);
  	make_cleanup (free, filename);
      
  	scratch_chan = openp(getenv("PATH"), 1, filename,
			     write_files? O_RDWR: O_RDONLY, 0,
  			     &scratch_pathname);
  	if (scratch_chan < 0)
	  perror_with_name(filename);

  	exec_bfd = bfd_fdopenr(scratch_pathname, gnutarget, scratch_chan);
  	if (!exec_bfd)
	  error("Could not open `%s' as an executable file: %s"
  		      , scratch_pathname, bfd_errmsg(bfd_error));

  	/* make sure we have an object file */

  	if (!bfd_check_format(exec_bfd, bfd_object))
  		error("\"%s\": not in executable format: %s.",
  		      scratch_pathname, bfd_errmsg(bfd_error));


  	/* setup initial vmap */

  	map_vmap (exec_bfd, 0);
  	if (!vmap)
  		error("Can't find the file sections in `%s': %s",
  		      exec_bfd->filename, bfd_errmsg(bfd_error));

	if (build_section_table (exec_bfd, &exec_ops.to_sections,
				&exec_ops.to_sections_end))
	  error ("Can't find the file sections in `%s': %s", 
  		exec_bfd->filename, bfd_errmsg (bfd_error));

  	/* make sure core, if present, matches */
  	validate_files();

  	push_target(&exec_ops);

  	/* Tell display code(if any) about the changed file name. */

  	if (exec_file_display_hook)
  		(*exec_file_display_hook)(filename);
  } 
  else {
  	exec_close(0);	/* just in case	*/
  	if (from_tty)
	  printf("No exec file now.\n");
  }
}

/* Set both the exec file and the symbol file, in one command.  What a
 * novelty.  Why did GDB go through four major releases before this
 * command was added?
 */
static void
file_command(arg, from_tty)
char *arg; {

	exec_file_command(arg, from_tty);
	symbol_file_command(arg, from_tty);
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
    fatal ("aborting");	/* return 1? */
  if (*start)
    free (*start);
  *start = (struct section_table *) xmalloc (count * sizeof (**start));
  *end = *start;
  bfd_map_over_sections (some_bfd, add_to_section_table, (char *)end);
  if (*end > *start + count)
    fatal ("aborting");
  /* We could realloc the table, but it probably loses for most files.  */
  return 0;
}

void
sex_to_vmap(bfd *bf, sec_ptr sex, PTR arg3) 
{
  struct vmap_and_bfd *vmap_bfd = (struct vmap_and_bfd *)arg3;
  register struct vmap *vp, **vpp;
  register struct symtab *syms;
  bfd *arch = vmap_bfd->pbfd;
  vp = vmap_bfd->pvmap;

  if ((bfd_get_section_flags(bf, sex) & SEC_LOAD) == 0)
    return;

  if (STREQ(bfd_section_name(bf, sex), ".text")) {
    vp->tstart = 0;
    vp->tend   = vp->tstart + bfd_section_size(bf, sex);

    /* When it comes to this adjustment value, in contrast to our previous
       belief shared objects should behave the same as the main load segment.
       This is the offset from the beginning of text section to the first
       real instruction. */

    vp->tadj = sex->filepos - bfd_section_vma(bf, sex);
  }

  else if (STREQ(bfd_section_name(bf, sex), ".data")) {
    vp->dstart = 0;
    vp->dend   = vp->dstart + bfd_section_size(bf, sex);
  }

  else if (STREQ(bfd_section_name(bf, sex), ".bss"))	/* FIXMEmgo */
    printf ("bss section in exec! Don't know what the heck to do!\n");
}

/* Make a vmap for the BFD "bf", which might be a member of the archive
   BFD "arch".  Return the new vmap.  */
struct vmap *
map_vmap (bfd *bf, bfd *arch)
{
  struct vmap_and_bfd vmap_bfd;
  struct vmap *vp, **vpp;
  struct objfile *obj;

  vp = (void*) xmalloc (sizeof (*vp));
  memset (vp, '\0', sizeof (*vp));
  vp->nxt = 0;
  vp->bfd = bf;
  vp->name = bfd_get_filename(arch ? arch : bf);
  vp->member = arch ? bfd_get_filename(bf) : "";
  
  vmap_bfd.pbfd = arch;
  vmap_bfd.pvmap = vp;
  bfd_map_over_sections (bf, sex_to_vmap, &vmap_bfd);

  /* find the end of the list, and append. */
  for (vpp = &vmap; *vpp; vpp = &(*vpp)->nxt)
  ;
  *vpp = vp;

  return vp;
}


/*  vmap_symtab -	handle symbol translation on vmapping */

static void
vmap_symtab (vp)
     register struct vmap *vp;
{
  register struct objfile *objfile;
  asection *textsec;
  asection *datasec;
  asection *bsssec;
  CORE_ADDR text_delta;
  CORE_ADDR data_delta;
  CORE_ADDR bss_delta;
  struct section_offsets *new_offsets;
  int i;
  
  objfile = vp->objfile;
  if (objfile == NULL)
    {
      /* OK, it's not an objfile we opened ourselves.
	 Currently, that can only happen with the exec file, so
	 relocate the symbols for the symfile.  */
      if (symfile_objfile == NULL)
	return;
      objfile = symfile_objfile;
    }

  new_offsets = alloca
    (sizeof (struct section_offsets)
     + sizeof (new_offsets->offsets) * objfile->num_sections);

  for (i = 0; i < objfile->num_sections; ++i)
    ANOFFSET (new_offsets, i) = ANOFFSET (objfile->section_offsets, i);
  
  textsec = bfd_get_section_by_name (vp->bfd, ".text");
  text_delta =
    vp->tstart - ANOFFSET (objfile->section_offsets, textsec->target_index);
  ANOFFSET (new_offsets, textsec->target_index) = vp->tstart;

  datasec = bfd_get_section_by_name (vp->bfd, ".data");
  data_delta =
    vp->dstart - ANOFFSET (objfile->section_offsets, datasec->target_index);
  ANOFFSET (new_offsets, datasec->target_index) = vp->dstart;
  
  bsssec = bfd_get_section_by_name (vp->bfd, ".bss");
  bss_delta =
    vp->dstart - ANOFFSET (objfile->section_offsets, bsssec->target_index);
  ANOFFSET (new_offsets, bsssec->target_index) = vp->dstart;

  objfile_relocate (objfile, new_offsets);

  {
    struct obj_section *s;
    for (s = objfile->sections; s < objfile->sections_end; ++s)
      {
	if (s->sec_ptr->target_index == textsec->target_index)
	  {
	    s->addr += text_delta;
	    s->endaddr += text_delta;
	  }
	else if (s->sec_ptr->target_index == datasec->target_index)
	  {
	    s->addr += data_delta;
	    s->endaddr += data_delta;
	  }
	else if (s->sec_ptr->target_index == bsssec->target_index)
	  {
	    s->addr += bss_delta;
	    s->endaddr += bss_delta;
	  }
      }
  }
  
  if (text_delta != 0)
    /* breakpoints need to be relocated as well. */
    fixup_breakpoints (0, TEXT_SEGMENT_BASE, text_delta);
}

/* Add symbols for an objfile.  */
static int
objfile_symbol_add (arg)
     char *arg;
{
  struct objfile *obj = (struct objfile *) arg;
  syms_from_objfile (obj, 0, 0, 0);
  new_symfile_objfile (obj, 0, 0);
  return 1;
}

static struct vmap *add_vmap PARAMS ((struct ld_info *));

/* Add a new vmap entry based on ldinfo() information.

   If ldi->ldinfo_fd is not valid (e.g. this struct ld_info is from a
   core file), the caller should set it to -1, and we will open the file.

   Return the vmap new entry.  */
static struct vmap *
add_vmap(ldi)
     register struct ld_info *ldi; 
{
	bfd *abfd, *last;
	register char *mem, *objname;
	struct objfile *obj;
	struct vmap *vp;

	/* This ldi structure was allocated using alloca() in 
	   xcoff_relocate_symtab(). Now we need to have persistent object 
	   and member names, so we should save them. */

	mem = ldi->ldinfo_filename + strlen(ldi->ldinfo_filename) + 1;
	mem = savestring (mem, strlen (mem));
	objname = savestring (ldi->ldinfo_filename, strlen (ldi->ldinfo_filename));

	if (ldi->ldinfo_fd < 0)
	  /* Note that this opens it once for every member; a possible
	     enhancement would be to only open it once for every object.  */
	  abfd = bfd_openr (objname, gnutarget);
	else
	  abfd = bfd_fdopenr(objname, gnutarget, ldi->ldinfo_fd);
	if (!abfd)
	  error("Could not open `%s' as an executable file: %s",
					objname, bfd_errmsg(bfd_error));


	/* make sure we have an object file */

	if (bfd_check_format(abfd, bfd_object))
	  vp = map_vmap (abfd, 0);

	else if (bfd_check_format(abfd, bfd_archive)) {
		last = 0;
		/*
		 * FIXME??? am I tossing BFDs?  bfd?
		 */
		while (last = bfd_openr_next_archived_file(abfd, last))
			if (STREQ(mem, last->filename))
				break;

		if (!last) {
		  bfd_close(abfd);
		  /* FIXME -- should be error */
		  warning("\"%s\": member \"%s\" missing.", abfd->filename, mem);
		  return;
		}

		if (!bfd_check_format(last, bfd_object)) {
			bfd_close(last);	/* XXX???	*/
			goto obj_err;
		}

		vp = map_vmap (last, abfd);
	}
	else {
	    obj_err:
		bfd_close(abfd);
		error ("\"%s\": not in executable format: %s.",
		       objname, bfd_errmsg(bfd_error));
		/*NOTREACHED*/
	}
	obj = allocate_objfile (vp->bfd, 0);
	vp->objfile = obj;

#ifndef SOLIB_SYMBOLS_MANUAL
	if (catch_errors (objfile_symbol_add, (char *)obj,
			  "Error while reading shared library symbols:\n",
			  RETURN_MASK_ALL))
	  {
	    /* Note this is only done if symbol reading was successful.  */
	    vmap_symtab (vp);
	    vp->loaded = 1;
	  }
#endif
	return vp;
}


/* As well as symbol tables, exec_sections need relocation. After
   the inferior process' termination, there will be a relocated symbol
   table exist with no corresponding inferior process. At that time, we
   need to use `exec' bfd, rather than the inferior process's memory space
   to look up symbols.

   `exec_sections' need to be relocated only once, as long as the exec
   file remains unchanged.
*/
vmap_exec ()
{
  static bfd *execbfd;
  int i;

  if (execbfd == exec_bfd)
    return;

  execbfd = exec_bfd;

  if (!vmap || !exec_ops.to_sections)
    error ("vmap_exec: vmap or exec_ops.to_sections == 0\n");

  for (i=0; &exec_ops.to_sections[i] < exec_ops.to_sections_end; i++)
    {
      if (STREQ(".text", exec_ops.to_sections[i].sec_ptr->name))
	{
	  exec_ops.to_sections[i].addr += vmap->tstart;
	  exec_ops.to_sections[i].endaddr += vmap->tstart;
	}
      else if (STREQ(".data", exec_ops.to_sections[i].sec_ptr->name))
	{
	  exec_ops.to_sections[i].addr += vmap->dstart;
	  exec_ops.to_sections[i].endaddr += vmap->dstart;
	}
    }
}

#if 0
/* This was for the old, half-assed, core file support.  */
int
text_adjustment (abfd)
bfd *abfd;
{
  static bfd *execbfd;
  static int adjustment;
  sec_ptr  sect;

  if (exec_bfd == execbfd)
    return adjustment;

  sect = bfd_get_section_by_name (abfd, ".text");
  if (sect)
    adjustment = sect->filepos - sect->vma;
  else
    adjustment = 0x200;				/* just a wild assumption */

  return adjustment;
}
#endif

/*
 * vmap_ldinfo -	update VMAP info with ldinfo() information
 *
 * Input:
 *	ldi	-	^ to ldinfo() results.
 */
vmap_ldinfo(ldi)
register struct ld_info *ldi;
{
  struct stat ii, vi;
  register struct vmap *vp;
  register got_one, retried;
  CORE_ADDR ostart;

  /*
   * for each *ldi, see if we have a corresponding *vp
   *	if so, update the mapping, and symbol table.
   *	if not, add an entry and symbol table.
   */
  do {
	char *name = ldi->ldinfo_filename;
	char *memb = name + strlen(name) + 1;

	retried = 0;

	if (fstat(ldi->ldinfo_fd, &ii) < 0)
		fatal("cannot fstat(%d) on %s"
		      , ldi->ldinfo_fd
		      , name);
retry:
	for (got_one = 0, vp = vmap; vp; vp = vp->nxt) {
	  FILE *io;

	  /* First try to find a `vp', which is the same as in ldinfo.
	     If not the same, just continue and grep the next `vp'. If same,
	     relocate its tstart, tend, dstart, dend values. If no such `vp'
	     found, get out of this for loop, add this ldi entry as a new vmap
	     (add_vmap) and come back, fins its `vp' and so on... */

	  /* The filenames are not always sufficient to match on. */

	  if ((name[0] == '/' && !STREQ(name, vp->name))
	      	|| (memb[0] && !STREQ(memb, vp->member)))
	    continue;

	  io = bfd_cache_lookup(vp->bfd);		/* totally opaque! */
	  if (!io)
	    fatal("cannot find BFD's iostream for %s", vp->name);

	  /* see if we are referring to the same file */

	  if (fstat(fileno(io), &vi) < 0)
	    fatal("cannot fstat BFD for %s", vp->name);

	  if (ii.st_dev != vi.st_dev || ii.st_ino != vi.st_ino)
	    continue;

	  if (!retried)
	    close(ldi->ldinfo_fd);

	  ++got_one;

	  /* found a corresponding VMAP. remap! */
	  ostart = vp->tstart;

	  /* We can assume pointer == CORE_ADDR, this code is native only.  */
	  vp->tstart = (CORE_ADDR) ldi->ldinfo_textorg;
	  vp->tend   = vp->tstart + ldi->ldinfo_textsize;
	  vp->dstart = (CORE_ADDR) ldi->ldinfo_dataorg;
	  vp->dend   = vp->dstart + ldi->ldinfo_datasize;

	  if (vp->tadj) {
	    vp->tstart += vp->tadj;
	    vp->tend   += vp->tadj;
	  }

	  /* relocate symbol table(s). */
	  vmap_symtab (vp);

	  /* there may be more, so we don't break out of the loop. */
	}

	/* if there was no matching *vp, we must perforce create the sucker(s) */
  	if (!got_one && !retried) {
	  add_vmap(ldi);
	  ++retried;
	  goto retry;
	}
  } while (ldi->ldinfo_next
	 && (ldi = (void *) (ldi->ldinfo_next + (char *) ldi)));

}

/*
 * vmap_inferior -	print VMAP info for inferior
 */
vmap_inferior() {

	if (inferior_pid == 0)
	  return 0;				/* normal processing	*/

	exec_files_info();
	return 1;
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
	    res = xfer_fn (p->bfd, p->sec_ptr, myaddr, memaddr - p->addr, len);
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
	    res = xfer_fn (p->bfd, p->sec_ptr, myaddr, memaddr - p->addr, len);
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

void
print_section_info (t, abfd)
  struct target_ops *t;
  bfd *abfd;
{
  struct section_table *p;

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
		       local_hex_string_custom ((unsigned long) p->sec_ptr->filepos, "08l"));
    printf_filtered (" is %s", bfd_section_name (p->bfd, p->sec_ptr));
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

  printf("\tMapping info for file `%s'.\n", vp->name);

  printf("\t  %8.8s   %8.8s   %8.8s   %8.8s %8.8s %s\n",
    "tstart", "tend", "dstart", "dend", "section", "file(member)");

  for (; vp; vp = vp->nxt)
     printf("\t0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x %s%s%s%s\n",
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

  for (p = exec_ops.to_sections; p < exec_ops.to_sections_end; p++) {
    if (!strncmp (secname, bfd_section_name (exec_bfd, p->sec_ptr), seclen)
	&& bfd_section_name (exec_bfd, p->sec_ptr)[seclen] == '\0') {
      offset = secaddr - p->addr;
      p->addr += offset;
      p->endaddr += offset;
      if (from_tty)
        exec_files_info(&exec_ops);
      return;
    }
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

/* Core file stuff.  */

/* Relocate symtabs and read in shared library info, based on symbols
   from the core file.  */
void
xcoff_relocate_core ()
{
/* Offset of member MEMBER in a struct of type TYPE.  */
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)
#endif

/* Size of a struct ld_info except for the variable-length filename.  */
#define LDINFO_SIZE (offsetof (struct ld_info, ldinfo_filename))

  sec_ptr ldinfo_sec;
  int offset = 0;
  struct ld_info *ldip;
  struct vmap *vp;

  /* Allocated size of buffer.  */
  int buffer_size = LDINFO_SIZE;
  char *buffer = xmalloc (buffer_size);
  struct cleanup *old = make_cleanup (free_current_contents, &buffer);
    
  /* FIXME, this restriction should not exist.  For now, though I'll
     avoid coredumps with error() pending a real fix.  */
  if (vmap == NULL)
    error
      ("Can't debug a core file without an executable file (on the RS/6000)");
  
  ldinfo_sec = bfd_get_section_by_name (core_bfd, ".ldinfo");
  if (ldinfo_sec == NULL)
    {
bfd_err:
      fprintf_filtered (stderr, "Couldn't get ldinfo from core file: %s\n",
			bfd_errmsg (bfd_error));
      do_cleanups (old);
      return;
    }
  do
    {
      int i;
      int names_found = 0;

      /* Read in everything but the name.  */
      if (bfd_get_section_contents (core_bfd, ldinfo_sec, buffer,
				    offset, LDINFO_SIZE) == 0)
	goto bfd_err;

      /* Now the name.  */
      i = LDINFO_SIZE;
      do
	{
	  if (i == buffer_size)
	    {
	      buffer_size *= 2;
	      buffer = xrealloc (buffer, buffer_size);
	    }
	  if (bfd_get_section_contents (core_bfd, ldinfo_sec, &buffer[i],
					offset + i, 1) == 0)
	    goto bfd_err;
	  if (buffer[i++] == '\0')
	    ++names_found;
	} while (names_found < 2);

      ldip = (struct ld_info *)buffer;

      /* Can't use a file descriptor from the core file; need to open it.  */
      ldip->ldinfo_fd = -1;
      
      /* The first ldinfo is for the exec file, allocated elsewhere.  */
      if (offset == 0)
	vp = vmap;
      else
	vp = add_vmap (ldip);

      offset += ldip->ldinfo_next;

      /* We can assume pointer == CORE_ADDR, this code is native only.  */
      vp->tstart = (CORE_ADDR) ldip->ldinfo_textorg;
      vp->tend = vp->tstart + ldip->ldinfo_textsize;
      vp->dstart = (CORE_ADDR) ldip->ldinfo_dataorg;
      vp->dend = vp->dstart + ldip->ldinfo_datasize;

      if (vp->tadj != 0) {
	vp->tstart += vp->tadj;
	vp->tend += vp->tadj;
      }

      /* Unless this is the exec file,
	 add our sections to the section table for the core target.  */
      if (vp != vmap)
	{
	  int count;
	  struct section_table *stp;
	  
	  count = core_ops.to_sections_end - core_ops.to_sections;
	  count += 2;
	  core_ops.to_sections = (struct section_table *)
	    xrealloc (core_ops.to_sections,
		      sizeof (struct section_table) * count);
	  core_ops.to_sections_end = core_ops.to_sections + count;
	  stp = core_ops.to_sections_end - 2;

	  /* "Why do we add bfd_section_vma?", I hear you cry.
	     Well, the start of the section in the file is actually
	     that far into the section as the struct vmap understands it.
	     So for text sections, bfd_section_vma tends to be 0x200,
	     and if vp->tstart is 0xd0002000, then the first byte of
	     the text section on disk corresponds to address 0xd0002200.  */
	  stp->bfd = vp->bfd;
	  stp->sec_ptr = bfd_get_section_by_name (stp->bfd, ".text");
	  stp->addr = bfd_section_vma (stp->bfd, stp->sec_ptr) + vp->tstart;
	  stp->endaddr = bfd_section_vma (stp->bfd, stp->sec_ptr) + vp->tend;
	  stp++;
	  
	  stp->bfd = vp->bfd;
	  stp->sec_ptr = bfd_get_section_by_name (stp->bfd, ".data");
	  stp->addr = bfd_section_vma (stp->bfd, stp->sec_ptr) + vp->dstart;
	  stp->endaddr = bfd_section_vma (stp->bfd, stp->sec_ptr) + vp->dend;
	}

      vmap_symtab (vp);

      add_text_to_loadinfo ((CORE_ADDR)ldip->ldinfo_textorg,
			    (CORE_ADDR)ldip->ldinfo_dataorg);
    } while (ldip->ldinfo_next != 0);
  vmap_exec ();
  do_cleanups (old);
}

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
