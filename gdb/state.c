/* Support for dumping and reloading various pieces of GDB's internal state.
   Copyright 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support, using pieces from other GDB modules.

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

/* This file provides support for dumping and then later reloading various
   portions of gdb's internal state.  It was originally implemented to
   support a need for mapping in an image of gdb's symbol table from an
   external file, where this image was created by an external program, such
   as an incremental linker.  However, it was generalized to enable future
   support for dumping and reloading various other useful pieces of gdb's
   internal state.

   State files have a fairly simple form which is intended to be easily
   extensible.  The basic format is:

	<file-header> <state-data> <form-tree>

   Where:

        file-header	A simple file-header containing a magic number
			so that gdb (and other readers) can quickly
			determine what kind of file this is, and a file
			offset to the root of the form-tree.

	state-data	The "raw" state-data that is referenced by nodes
			in the form-tree.

	form-tree	A tree of arbitrarily sized nodes containing
			information about gdb's internal state, and
			possibly referencing data in the state-data section
			of the file.  Resembles DWARF in some respects.

   When writing a state file, a hole is left for the file-header at the
   beginning of the file, the state data is written immediately after the
   file header (while storing the file offsets and sizes back into the
   internal form-tree along the way), the form-tree itself is written
   at the end of the file, and then the file header is written by seeking
   back to the beginning of the file.  This order is required because
   the form tree contains file offsets and sizes in the state data portion
   of the file, and the file header contains the file offset to the start
   of the form tree.

   Readers simply open the file, validate the magic number, seek to the
   root of the form-tree, and walk the tree looking for the information that
   they are interested in (and ignoring things that they aren't, or don't
   understand).

   */


#include "defs.h"
#include "symtab.h"
#include "bfd.h"
#include "symfile.h"
#include "state.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

/* Inside the state file, the form-tree consists of a series of
   form-tree entries (FTE's).  The parent/child/sibling relationships
   are implied by the ordering and by an explicit sibling reference
   in FTE's that have siblings.

   Specifically, given two sequential FTE's, say A and B, if B immediately
   follows A, and A does not have a sibling reference to B, then B is
   the first child of A.  Otherwise B must be a sibling of A and A must
   have a sibling reference for it.

   Each FTE is simply an array of long integers, with at least three
   members.  This form was chosen over a packed data form for simplicity
   in access, not having to worry about the relative sizes of the different
   integers (short, int, long), and not having to worry about alignment
   constraints.  Also in the name of simplicity, every FTE has a sibling
   reference slot reserved for it, even if there are no siblings.

   The first value in an FTE is the size of the FTE in bytes, including
   the size value itself.  The second entry contains a tag which indicates
   the type of the FTE.  The third entry is a sibling reference, which either
   refers to a valid sibling node or is zero.  Following is zero or more
   attributes, each of which consists of one or more long values. */

/* Tag names and codes. */

#define TAG_padding	0x0000		/* Padding */
#define TAG_objfile	0x0001		/* Dumped objfile */

/* Form names, codes, and macros. */

#define FORM_ABSREF	0x01		/* Next long is absolute file offset */
#define FORM_RELREF	0x02		/* Next long is relative file offset */
#define FORM_IVAL	0x03		/* Next long is int value */
#define FORM_ADDR	0x04		/* Next long is mem addr */

#define FORM_MASK	0xFF
#define FORM_X(atr)	((atr) & FORM_MASK)

/* Attribute names and codes. */

#define AT_sibling	(0x0100 | FORM_RELREF)	/* Reference to sibling node */
#define AT_name		(0x0200 | FORM_ABSREF)	/* Reference to a string */
#define AT_offset	(0x0300 | FORM_ABSREF)	/* Reference to generic data */
#define AT_size		(0x0400 | FORM_IVAL)
#define AT_addr		(0x0500 | FORM_ADDR)
#define AT_aux_addr	(0x0600 | FORM_ADDR)

/* */

static void
load_symbols PARAMS ((FILE *));

static void
dump_state_command PARAMS ((char *, int));

static void
load_state_command PARAMS ((char *, int));

#ifdef HAVE_MMAP

static void
write_header PARAMS ((sfd *));

static void
write_formtree PARAMS ((sfd *));

static void
write_objfile_state PARAMS ((sfd *));

static void
free_subtree PARAMS ((struct formnode *));

static void
size_subtree PARAMS ((struct formnode *));

#endif

struct formnode *formtree = NULL;

/* ARGSUSED */
static void
load_symbols (statefile)
     FILE *statefile;
{

#if 0
  /* Discard old symbols.  FIXME: This is essentially symbol_file_command's
     body when there is no name.  Make it a common function that is
     called from each place. */

  if (symfile_objfile)
    {
      free_objfile (symfile_objfile);
    }
  symfile_objfile = NULL;
#endif

#if 0 && defined (HAVE_MMAP)
  if (mtop > mbase)
    {
      warning ("internal error: mbase (%08x) != mtop (%08x)",
	       mbase, mtop);
      munmap (mbase, mtop - mbase);
    }
#endif	/* HAVE_MMAP */

  /* Getting new symbols may change our opinion about what is frameless. */

  reinit_frame_cache ();

}

#ifdef HAVE_MMAP

/* Allocate a form node */

static struct formnode *
alloc_formnode ()
{
  struct formnode *fnp;
  fnp = (struct formnode *) xmalloc (sizeof (struct formnode));
  (void) memset (fnp, 0, sizeof (struct formnode));
  fnp -> sibling = formtree;
  formtree = fnp;
  return (fnp);
}

/* Recursively walk a form-tree from the specified node, freeing
   nodes from the bottom up.  The concept is pretty simple, just free
   all the child nodes, then all the sibling nodes, then the node
   itself. */

static void
free_subtree (fnp)
     struct formnode *fnp;
{
  if (fnp != NULL)
    {
      free_subtree (fnp -> child);
      free_subtree (fnp -> sibling);
      if (fnp -> nodedata != NULL)
	{
	  free (fnp -> nodedata);
	}
      free (fnp);
    }
}

/* Recursively walk a form-tree from the specified node, computing the
   size of each subtree from the bottom up.

   At each node, the file space that will be consumed by the subtree
   rooted in that node is the sum of all the subtrees rooted in each
   child node plus the size of the node itself.

   Thus for each node, we size the child subtrees, add to that our
   size, contribute this size towards the size of any parent node, and
   then ask any of our siblings to do the same.

   Also, once we know the size of any subtree rooted at this node, we
   can initialize the offset to the sibling node (if any).

   Since every form-tree node must have valid nodedata at this point,
   we detect and report a warning for any node that doesn't. */

static void
size_subtree (fnp)
     struct formnode *fnp;
{
  long *lp;

  if (fnp != NULL)
    {
      if (fnp -> nodedata == NULL)
	{
	  warning ("internal error -- empty form node");
	}
      else
	{
	  size_subtree (fnp -> child);
	  fnp -> treesize += *(long *) fnp -> nodedata;
	  if (fnp -> parent != NULL)
	    {
	      fnp -> parent -> treesize += fnp -> treesize;
	    }
	  if (fnp -> sibling)
	    {
	      size_subtree (fnp -> sibling);
	      lp = (long *) (fnp -> nodedata + 2 * sizeof (long));
	      *lp = fnp -> treesize;
	    }
	}
    }
}

/* Recursively walk a form-tree from the specified node, writing
   nodes from the top down. */

static void
write_subtree (fnp, asfd)
     struct formnode *fnp;
     sfd *asfd;
{
  if (fnp != NULL)
    {
      if (fnp -> nodedata != NULL)
	{
	  fwrite (fnp -> nodedata, *(long *) fnp -> nodedata, 1, asfd -> fp);
	}
      write_subtree (fnp -> child, asfd);
      write_subtree (fnp -> sibling, asfd);
    }
}

/* Free the entire current formtree.  Called via do_cleanups, regardless
   of whether there is an error or not. */

static void
free_formtree ()
{
  free_subtree (formtree);
  formtree = NULL;
}

/* Write out the file header.  Generally this is done last, even though
   it is located at the start of the file, since we need to have file
   offset to where the annotated form tree was written, and it's size. */

static void
write_header (asfd)
     sfd *asfd;
{
  fseek (asfd -> fp, 0L, SEEK_SET);
  fwrite ((char *) &asfd -> hdr, sizeof (asfd -> hdr), 1, asfd -> fp);
}

/* Write out the annotated form tree.  We should already have written out
   the state data, and noted the file offsets and sizes in each node of
   the form tree that references part of the state data.

   The form tree can be written anywhere in the file where there is room
   for it.  Since there is always room at the end of the file, we write
   it there.  We also need to record the file offset to the start of the
   form tree, and it's size, for future use when writing the file header.

   In order to compute the sibling references, we need to know, at
   each node, how much space will be consumed when all of that node's
   children nodes have been written.  Thus we walk the tree, computing
   the sizes of the subtrees from the bottom up.  At any node, the
   offset from the start of that node to the start of the sibling node
   is simply the size of the node plus the size of the subtree rooted
   in that node. */

static void
write_formtree (asfd)
     sfd *asfd;
{
  size_subtree (formtree);
  fseek (asfd -> fp, 0L, SEEK_END);
  asfd -> hdr.sf_ftoff = ftell (asfd -> fp);
  write_subtree (formtree, asfd);
  asfd -> hdr.sf_ftsize = ftell (asfd -> fp) - asfd -> hdr.sf_ftoff;
}

/* Note that we currently only support having one objfile with dumpable
   state. */

static void
write_objfile_state (asfd)
     sfd *asfd;
{
  struct objfile *objfile;
  struct formnode *fnp;
  PTR base;
  PTR breakval;
  long *lp;
  unsigned int ftesize;
  long ftebuf[64];
  long foffset;

  /* First walk through the objfile list looking for the first objfile
     that is dumpable. */

  for (objfile = object_files; objfile != NULL; objfile = objfile -> next)
    {
      if (objfile -> flags & OBJF_DUMPABLE)
	{
	  break;
	}
    }

  if (objfile == NULL)
    {
      warning ("no dumpable objfile was found");
    }
  else
    {
      fnp = alloc_formnode ();
      lp = ftebuf;

      lp++;			/* Skip FTE size slot, filled in at the end. */
      *lp++ = TAG_objfile;	/* This is an objfile FTE */
      *lp++ = 0;		/* Zero the sibling reference slot. */

      /* Build an AT_name attribute for the objfile's name, and write
	 the name into the state data. */

      *lp++ = AT_name;
      *lp++ = (long) ftell (asfd -> fp);
      fwrite (objfile -> name, strlen (objfile -> name) + 1, 1, asfd -> fp);

      /* Build an AT_addr attribute for the virtual address to which the
	 objfile data is mapped (and needs to be remapped when read in). */

      base = mmap_base ();
      *lp++ = AT_addr;
      *lp++ = (long) base;

      /* Build an AT_aux_addr attribute for the address of the objfile
	 structure itself, within the dumpable data.  When we read the objfile
	 back in, we use this address as the pointer the "struct objfile". */

      *lp++ = AT_aux_addr;
      *lp++ = (long) objfile;

      /* Reposition in state file to next paging boundry so we can mmap the
	 dumpable objfile data when we reload it. */

      foffset = (long) mmap_page_align ((PTR) ftell (asfd -> fp));
      fseek (asfd -> fp, foffset, SEEK_SET);

      /* Build an AT_offset attribute for the offset in the state file to
	 the start of the dumped objfile data. */

      *lp++ = AT_offset;
      *lp++ = (long) ftell (asfd -> fp);

      /* Build an AT_size attribute for the size of the dumped objfile data. */

      breakval = mmap_sbrk (0);
      *lp++ = AT_size;
      *lp++ = breakval - base;

      /* Write the dumpable data. */ 

      fwrite ((char *) base, breakval - base, 1, asfd -> fp);

      /* Now finish up the FTE by filling in the size slot based on
	 how much of the ftebuf we have used, allocate some memory for
	 it hung off the form tree node, and copy it there. */

      ftebuf[0] = (lp - ftebuf) * sizeof (ftebuf[0]);
      fnp -> nodedata = (char *) xmalloc (ftebuf[0]);
      memcpy (fnp -> nodedata, ftebuf, ftebuf[0]);
    }
}

static void
load_state_command (arg_string, from_tty)
     char *arg_string;
     int from_tty;
{
  char *filename;
  char **argv;
  FILE *fp;
  struct cleanup *cleanups;
  
  dont_repeat ();

  if (arg_string == NULL)
    {
      error ("load-state takes a file name and optional state specifiers");
    }
  else if ((argv = buildargv (arg_string)) == NULL)
    {
      fatal ("virtual memory exhausted.", 0);
    }
  cleanups = make_cleanup (freeargv, argv);

  filename = tilde_expand (*argv);
  make_cleanup (free, filename);

  if ((fp = fopen (filename, FOPEN_RB)) == NULL)
    {
      perror_with_name (filename);
    }
  make_cleanup (fclose, fp);
  immediate_quit++;

  while (*++argv != NULL)
    {
      if (STREQ (*argv, "symbols"))
	{
	  if (from_tty
	      && !query ("load symbol table state from file \"%s\"? ",
			 filename))
	    {
	      error ("Not confirmed.");
	    }
	  load_symbols (fp);
	}
      else
	{
	  error ("unknown state specifier '%s'", *argv);
	}
    }
  immediate_quit--;
  do_cleanups (cleanups);
}

/* ARGSUSED */
static void
dump_state_command (arg_string, from_tty)
     char *arg_string;
     int from_tty;
{
  char *filename;
  char **argv;
  sfd *asfd;
  struct cleanup *cleanups;
  
  dont_repeat ();

  if (arg_string == NULL)
    {
      error ("dump-state takes a file name and state specifiers");
    }
  else if ((argv = buildargv (arg_string)) == NULL)
    {
      fatal ("virtual memory exhausted.", 0);
    }
  cleanups = make_cleanup (freeargv, argv);

  filename = tilde_expand (*argv);
  make_cleanup (free, filename);

  /* Now attempt to create a fresh state file. */

  if ((asfd = sfd_fopen (filename, FOPEN_WB)) == NULL)
    {
      perror_with_name (filename);
    }
  make_cleanup (sfd_fclose, asfd);
  make_cleanup (free_formtree, NULL);
  immediate_quit++;

  /* Now that we have an open and initialized state file, seek to the
     proper offset to start writing state data and the process the
     arguments.  For each argument, write the state data and initialize
     a form-tree node for each piece of state data. */

  fseek (asfd -> fp, sizeof (sf_hdr), SEEK_SET);
  while (*++argv != NULL)
    {
      if (STREQ (*argv, "objfile"))
	{
	  write_objfile_state (asfd);
	}
      else
	{
	  error ("unknown state specifier '%s'", *argv);
	}

    }

  /* We have written any state data.  All that is left to do now is
     write the form-tree and the file header. */

  write_formtree (asfd);
  write_header (asfd);

  immediate_quit--;
  do_cleanups (cleanups);
}

static char *
find_fte_by_walk (thisfte, endfte, tag)
     char *thisfte;
     char *endfte;
     long tag;
{
  char *found = NULL;
  char *nextfte;
  long thistag;
  long thissize;
  long siboffset;

  while (thisfte < endfte)
    {
      if ((thistag = *(long *)(thisfte + sizeof (long))) == tag)
	{
	  found = thisfte;
	  break;
	}
      else
	{
	  thissize =  *(long *)(thisfte);
	  siboffset = *(long *)(thisfte + (2 * sizeof (long)));
	  nextfte = thisfte + (siboffset != 0 ? siboffset : thissize);
	  found = find_fte_by_walk (thisfte + thissize, nextfte, tag);
	  thisfte = nextfte;
	}
    }
  return (found);
}

/* Walk the form-tree looking for a specific FTE type.  Returns the first
   one found that matches the specified tag. */

static char *
find_fte (asfd, tag)
     sfd *asfd;
     long tag;
{
  char *ftbase;
  char *ftend;
  char *ftep;
  char *found = NULL;

  if (fseek (asfd -> fp, asfd -> hdr.sf_ftoff, SEEK_SET) == 0)
    {
      ftbase = xmalloc (asfd -> hdr.sf_ftsize);
      ftend = ftbase + asfd -> hdr.sf_ftsize;
      if (fread (ftbase, asfd -> hdr.sf_ftsize, 1, asfd -> fp) == 1)
	{
	  ftep = find_fte_by_walk (ftbase, ftend, tag);
	  if (ftep != NULL)
	    {
	      found = xmalloc (*(long *)ftep);
	      memcpy (found, ftep, (int) *(long *)ftep);
	    }
	}
      free (ftbase);
    }
  return (found);
}

struct objfile *
objfile_from_statefile (asfd)
     sfd *asfd;
{
  struct objfile *objfile = NULL;
  char *ftep;
  long *thisattr;
  long *endattr;
  PTR base;
  long foffset;
  long mapsize;

  ftep = find_fte (asfd, TAG_objfile);
  thisattr = (long *) (ftep + 3 * sizeof (long));
  endattr = (long *) (ftep + *(long *)ftep);
  while (thisattr < endattr)
    {
      switch (*thisattr++)
	{
	  case AT_name:
	    /* Ignore for now */
	    thisattr++;
	    break;
	  case AT_addr:
	    base = (PTR) *thisattr++;
	    break;
	  case AT_aux_addr:
	    objfile = (struct objfile *) *thisattr++;
	    break;
	  case AT_offset:
	    foffset = *thisattr++;
	    break;
	  case AT_size:
	    mapsize = *thisattr++;
	    break;
	}
    }
  if (mmap_remap (base, mapsize, (int) fileno (asfd -> fp), foffset) != base)
    {
      print_sys_errmsg (asfd -> filename, errno);
      error ("mapping failed");
    }

  return (objfile);
}

#else

struct objfile *
objfile_from_statefile (asfd)
     sfd *asfd;
{
  error ("this version of gdb doesn't support reloading symtabs from state files");
}

#endif	/* HAVE_MMAP */

/* Close a state file, freeing all memory that was used by the state
   file descriptor, closing the raw file pointer, etc. */

void
sfd_fclose (asfd)
     sfd *asfd;
{
  if (asfd != NULL)
    {
      if (asfd -> fp != NULL)
	{
	  fclose (asfd -> fp);
	}
      if (asfd -> filename != NULL)
	{
	  free (asfd -> filename);
	}
      free (asfd);
    }
}

/* Given the name of a possible statefile, and flags to use to open it,
   try to open the file and prepare it for use.

   If the flags contain 'r', then we want to read an existing state
   file, so attempt to read in the state file header and determine if this
   is a valid state file.  If not, return NULL.

   Returns a pointer to a properly initialized state file descriptor if
   successful. */

sfd *
sfd_fopen (name, flags)
     char *name;
     char *flags;
{
  int success = 0;
  sfd *asfd;

  asfd = (sfd *) xmalloc (sizeof (sfd));
  (void) memset (asfd, 0, sizeof (sfd));
  asfd -> filename = xmalloc (strlen (name) + 1);
  (void) strcpy (asfd -> filename, name);

  if ((asfd -> fp = fopen (asfd -> filename, flags)) != NULL)
    {
      /* We have the file, now see if we are reading an existing file
	 or writing to a new file.  We don't currently support "rw". */
      if (strchr (flags, 'r') != NULL)
	{
	  if (fread ((char *) &asfd -> hdr, sizeof (asfd -> hdr), 1,
		     asfd -> fp) == 1)
	    {
	      if (SF_GOOD_MAGIC (asfd))
		{
		  success = 1;
		}
	    }
	}
      else
	{
	  /* This is a new state file.  Initialize various things. */
	  asfd -> hdr.sf_mag0 = SF_MAG0;
	  asfd -> hdr.sf_mag1 = SF_MAG1;
	  asfd -> hdr.sf_mag2 = SF_MAG2;
	  asfd -> hdr.sf_mag3 = SF_MAG3;
	  success = 1;
	}
    }

  if (!success)
    {
      sfd_fclose (asfd);
      asfd = NULL;
    }
  return (asfd);
  
}


void
_initialize_state ()
{

#ifdef HAVE_MMAP

  add_com ("load-state", class_support, load_state_command,
   "Load some saved gdb state from FILE.\n\
Select and load some portion of gdb's saved state from the specified file.\n\
The dump-state command may be used to save various portions of gdb's\n\
internal state.");

  add_com ("dump-state", class_support, dump_state_command,
   "Dump some of gdb's state to FILE.\n\
Select and dump some portion of gdb's internal state to the specified file.\n\
The load-state command may be used to reload various portions of gdb's\n\
internal state from the file.");

#endif	/* HAVE_MMAP */

}
