/* GDB routines for manipulating objfiles.
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

/* This file contains support routines for creating, manipulating, and
   destroying objfile structures. */

#include "defs.h"
#include "bfd.h"		/* Binary File Description */
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <obstack.h>

/* Prototypes for local functions */

static int
open_mapped_file PARAMS ((char *filename, long mtime, int mapped));

static CORE_ADDR
map_to_address PARAMS ((void));

/* Externally visible variables that are owned by this module.
   See declarations in objfile.h for more info. */

struct objfile *object_files;		/* Linked list of all objfiles */
struct objfile *current_objfile;	/* For symbol file being read in */
struct objfile *symfile_objfile;	/* Main symbol table loaded from */

int mapped_symbol_files;		/* Try to use mapped symbol files */

/* Given a pointer to an initialized bfd (ABFD) and a flag that indicates
   whether or not an objfile is to be mapped (MAPPED), allocate a new objfile
   struct, fill it in as best we can, link it into the list of all known
   objfiles, and return a pointer to the new objfile struct. */

struct objfile *
allocate_objfile (abfd, mapped)
     bfd *abfd;
     int mapped;
{
  struct objfile *objfile = NULL;
  int fd;
  void *md;
  CORE_ADDR mapto;

  mapped |= mapped_symbol_files;

#if !defined(NO_MMALLOC) && defined(HAVE_MMAP)

  /* If we can support mapped symbol files, try to open/reopen the mapped file
     that corresponds to the file from which we wish to read symbols.  If the
     objfile is to be mapped, we must malloc the structure itself using the
     mmap version, and arrange that all memory allocation for the objfile uses
     the mmap routines.  If we are reusing an existing mapped file, from which
     we get our objfile pointer, we have to make sure that we update the
     pointers to the alloc/free functions in the obstack, in case these
     functions have moved within the current gdb. */

  fd = open_mapped_file (bfd_get_filename (abfd), bfd_get_mtime (abfd),
			 mapped);
  if (fd >= 0)
    {
      if (((mapto = map_to_address ()) == 0) ||
	  ((md = mmalloc_attach (fd, (void *) mapto)) == NULL))
	{
	  close (fd);
	}
      else if ((objfile = (struct objfile *) mmalloc_getkey (md, 0)) != NULL)
	{
	  /* Update memory corruption handler function addresses. */
	  init_malloc (md);
	  objfile -> md = md;
	  /* Update pointers to functions to *our* copies */
	  obstack_chunkfun (&objfile -> psymbol_obstack, xmmalloc);
	  obstack_freefun (&objfile -> psymbol_obstack, mfree);
	  obstack_chunkfun (&objfile -> symbol_obstack, xmmalloc);
	  obstack_freefun (&objfile -> symbol_obstack, mfree);
	  obstack_chunkfun (&objfile -> type_obstack, xmmalloc);
	  obstack_freefun (&objfile -> type_obstack, mfree);
	}
      else
	{
	  /* Set up to detect internal memory corruption.  MUST be done before
	     the first malloc.  See comments in init_malloc() and mmcheck(). */
	  init_malloc (md);
	  objfile = (struct objfile *) xmmalloc (md, sizeof (struct objfile));
	  (void) memset (objfile, 0, sizeof (struct objfile));
	  objfile -> md = md;
	  objfile -> flags |= OBJF_MAPPED;
	  mmalloc_setkey (objfile -> md, 0, objfile);
	  obstack_full_begin (&objfile -> psymbol_obstack, 0, 0,
			      xmmalloc, mfree, objfile -> md,
			      OBSTACK_MMALLOC_LIKE);
	  obstack_full_begin (&objfile -> symbol_obstack, 0, 0,
			      xmmalloc, mfree, objfile -> md,
			      OBSTACK_MMALLOC_LIKE);
	  obstack_full_begin (&objfile -> type_obstack, 0, 0,
			      xmmalloc, mfree, objfile -> md,
			      OBSTACK_MMALLOC_LIKE);
	}
    }

  if (mapped && (objfile == NULL))
    {
      warning ("symbol table for '%s' will not be mapped",
	       bfd_get_filename (abfd));
    }

#else	/* defined(NO_MMALLOC) || !defined(HAVE_MMAP) */

  if (mapped)
    {
      warning ("this version of gdb does not support mapped symbol tables.");

      /* Turn off the global flag so we don't try to do mapped symbol tables
	 any more, which shuts up gdb unless the user specifically gives the
	 "mapped" keyword again. */

      mapped_symbol_files = 0;
    }

#endif	/* !defined(NO_MMALLOC) && defined(HAVE_MMAP) */

  /* If we don't support mapped symbol files, didn't ask for the file to be
     mapped, or failed to open the mapped file for some reason, then revert
     back to an unmapped objfile. */

  if (objfile == NULL)
    {
      objfile = (struct objfile *) xmalloc (sizeof (struct objfile));
      (void) memset (objfile, 0, sizeof (struct objfile));
      objfile -> md = NULL;
      obstack_full_begin (&objfile -> psymbol_obstack, 0, 0, xmalloc, free,
			  (void *) 0, 0);
      obstack_full_begin (&objfile -> symbol_obstack, 0, 0, xmalloc, free,
			  (void *) 0, 0);
      obstack_full_begin (&objfile -> type_obstack, 0, 0, xmalloc, free,
			  (void *) 0, 0);

    }

  /* Update the per-objfile information that comes from the bfd, ensuring
     that any data that is reference is saved in the per-objfile data
     region. */

  objfile -> obfd = abfd;
  objfile -> name = mstrsave (objfile -> md, bfd_get_filename (abfd));
  objfile -> mtime = bfd_get_mtime (abfd);

  /* Push this file onto the head of the linked list of other such files. */

  objfile -> next = object_files;
  object_files = objfile;

  return (objfile);
}


/* Destroy an objfile and all the symtabs and psymtabs under it.  Note
   that as much as possible is allocated on the symbol_obstack and
   psymbol_obstack, so that the memory can be efficiently freed.

   Things which we do NOT free because they are not in malloc'd memory
   or not in memory specific to the objfile include:

   	objfile -> sf

   */

void
free_objfile (objfile)
     struct objfile *objfile;
{
  struct objfile *ofp;

  if (objfile -> sf != NULL)
    {
      (*objfile -> sf -> sym_finish) (objfile);
    }
  if (objfile -> name != NULL)
    {
      mfree (objfile -> md, objfile -> name);
    }
  if (objfile -> obfd != NULL)
    {
      bfd_close (objfile -> obfd);
    }

  /* Remove it from the chain of all objfiles.  */

  if (object_files == objfile)
    {
      object_files = objfile -> next;
    }
  else
    {
      for (ofp = object_files; ofp; ofp = ofp -> next)
	{
	  if (ofp -> next == objfile)
	    {
	      ofp -> next = objfile -> next;
	    }
	}
    }

  obstack_free (&objfile -> psymbol_obstack, 0);
  obstack_free (&objfile -> symbol_obstack, 0);
  obstack_free (&objfile -> type_obstack, 0);

#if 0	/* FIXME!! */

  /* Before the symbol table code was redone to make it easier to
     selectively load and remove information particular to a specific
     linkage unit, gdb used to do these things whenever the monolithic
     symbol table was blown away.  How much still needs to be done
     is unknown, but we play it safe for now and keep each action until
     it is shown to be no longer needed. */
     
  clear_symtab_users_once ();
#if defined (CLEAR_SOLIB)
  CLEAR_SOLIB ();
#endif
  clear_pc_function_cache ();

#endif

  /* The last thing we do is free the objfile struct itself */

  mfree (objfile -> md, objfile);
}


/* Free all the object files at once.  */

void
free_all_objfiles ()
{
  struct objfile *objfile, *temp;

  ALL_OBJFILES_SAFE (objfile, temp)
    {
      free_objfile (objfile);
    }
}

/* Many places in gdb want to test just to see if we have any partial
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_partial_symbols ()
{
  struct objfile *ofp;

  ALL_OBJFILES (ofp)
    {
      if (ofp -> psymtabs != NULL)
	{
	  return 1;
	}
    }
  return 0;
}

/* Many places in gdb want to test just to see if we have any full
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_full_symbols ()
{
  struct objfile *ofp;

  ALL_OBJFILES (ofp)
    {
      if (ofp -> symtabs != NULL)
	{
	  return 1;
	}
    }
  return 0;
}

/* Many places in gdb want to test just to see if we have any minimal
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_minimal_symbols ()
{
  struct objfile *ofp;

  ALL_OBJFILES (ofp)
    {
      if (ofp -> msymbols != NULL)
	{
	  return 1;
	}
    }
  return 0;
}

/* Look for a mapped symbol file that corresponds to FILENAME and is more
   recent than MTIME.  If MAPPED is nonzero, the user has asked that gdb
   use a mapped symbol file for this file, so create a new one if one does
   not currently exist.

   If found, then return an open file descriptor for the file, otherwise
   return -1.

   This routine is responsible for implementing the policy that generates
   the name of the mapped symbol file from the name of a file containing
   symbols that gdb would like to read. */

static int
open_mapped_file (filename, mtime, mapped)
     char *filename;
     long mtime;
     int mapped;
{
  int fd;
  char *symfilename;
  struct stat sbuf;

  /* For now, all we do is look in the local directory for a file with
     the name of the base file and an extension of ".syms" */

  symfilename = concat ("./", basename (filename), ".syms", (char *) NULL);

  /* Check to see if the desired file already exists and is more recent than
     the corresponding base file (specified by the passed MTIME parameter).
     The open will fail if the file does not already exist. */

  if ((fd = open (symfilename, O_RDWR)) >= 0)
    {
      if (fstat (fd, &sbuf) != 0)
	{
	  close (fd);
	  perror_with_name (symfilename);
	}
      else if (sbuf.st_mtime > mtime)
	{
	  return (fd);
	}
      else
	{
	  close (fd);
	  fd = -1;
	}
    }

  /* Either the file does not already exist, or the base file has changed
     since it was created.  In either case, if the user has specified use of
     a mapped file, then create a new mapped file, truncating any existing
     one.

     In the case where there is an existing file, but it is out of date, and
     the user did not specify mapped, the existing file is just silently
     ignored.  Perhaps we should warn about this case (FIXME?).

     By default the file is rw for everyone, with the user's umask taking
     care of turning off the permissions the user wants off. */

  if (mapped)
    {
      fd = open (symfilename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    }

  return (fd);
}

/* Return the base address at which we would like the next objfile's
   mapped data to start.

   For now, we use the kludge that the configuration specifies a base
   address to which it is safe to map the first mmalloc heap, and an
   increment to add to this address for each successive heap.  There are
   a lot of issues to deal with here to make this work reasonably, including:

     Avoid memory collisions with existing mapped address spaces

     Reclaim address spaces when their mmalloc heaps are unmapped

     When mmalloc heaps are shared between processes they have to be
     mapped at the same addresses in each

     Once created, a mmalloc heap that is to be mapped back in must be
     mapped at the original address.  I.E. each objfile will expect to
     be remapped at it's original address.  This becomes a problem if
     the desired address is already in use.

     etc, etc, etc.

 */


static CORE_ADDR
map_to_address ()
{

#if defined(MMAP_BASE_ADDRESS) && defined (MMAP_INCREMENT)

  static CORE_ADDR next = MMAP_BASE_ADDRESS;
  CORE_ADDR mapto = next;

  next += MMAP_INCREMENT;
  return (mapto);

#else

  return (0);

#endif

}
