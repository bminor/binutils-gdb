/* Shared library support for RS/6000 (xcoff) object files, for GDB.
   Copyright 1991, 1992 Free Software Foundation.
   Contributed by IBM Corporation.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "bfd.h"
#include "xcoffsolib.h"
#include "inferior.h"
#include "gdbcmd.h"
#include "symfile.h"
#include "frame.h"
#include "gdb_regex.h"


/* Return the module name of a given text address. Note that returned buffer
   is not persistent. */

char *
pc_load_segment_name (CORE_ADDR addr)
{
  static char buffer[BUFSIZ];
  struct vmap *vp = vmap;

  buffer[0] = buffer[1] = '\0';
  for (; vp; vp = vp->nxt)
    if (vp->tstart <= addr && addr < vp->tend)
      {
	if (*vp->member)
	  {
	    buffer[0] = '(';
	    strcat (&buffer[1], vp->member);
	    strcat (buffer, ")");
	  }
	strcat (buffer, vp->name);
	return buffer;
      }
  return "(unknown load module)";
}

static void solib_info (char *, int);
static void sharedlibrary_command (char *pattern, int from_tty);

static void
solib_info (char *args, int from_tty)
{
  struct vmap *vp = vmap;

  /* Check for new shared libraries loaded with load ().  */
  if (inferior_pid)
    xcoff_relocate_symtab (inferior_pid);

  if (vp == NULL || vp->nxt == NULL)
    {
      printf_unfiltered ("No shared libraries loaded at this time.\n");
      return;
    }

  /* Skip over the first vmap, it is the main program, always loaded.  */
  vp = vp->nxt;

  printf_unfiltered ("\
Text Range		Data Range		Syms	Shared Object Library\n");

  for (; vp != NULL; vp = vp->nxt)
    {
      printf_unfiltered ("0x%s-0x%s	0x%s-0x%s	%s	%s%s%s%s\n",
			 paddr (vp->tstart),paddr (vp->tend),
			 paddr (vp->dstart), paddr (vp->dend),
			 vp->loaded ? "Yes" : "No ",
			 *vp->member ? "(" : "",
			 vp->member,
			 *vp->member ? ") " : "",
			 vp->name);
    }
}

static void
sharedlibrary_command (char *pattern, int from_tty)
{
  dont_repeat ();

  /* Check for new shared libraries loaded with load ().  */
  if (inferior_pid)
    xcoff_relocate_symtab (inferior_pid);

  if (pattern)
    {
      char *re_err = re_comp (pattern);

      if (re_err)
	error ("Invalid regexp: %s", re_err);
    }

  /* Walk the list of currently loaded shared libraries, and read
     symbols for any that match the pattern --- or any whose symbols
     aren't already loaded, if no pattern was given.  */
  {
    int any_matches = 0;
    int loaded_any_symbols = 0;
    struct vmap *vp = vmap;

    if (!vp)
      return;

    /* skip over the first vmap, it is the main program, always loaded. */
    for (vp = vp->nxt; vp; vp = vp->nxt)
      if (! pattern
	    || re_exec (vp->name)
	    || (*vp->member && re_exec (vp->member)))
	{
	  any_matches = 1;

	  if (vp->loaded)
	    {
	      if (from_tty)
		printf_unfiltered ("Symbols already loaded for %s\n",
				   vp->name);
	    }
	  else
	    {
	      if (vmap_add_symbols (vp))
		loaded_any_symbols = 1;
	    }
	}

    if (from_tty && pattern && ! any_matches)
      printf_unfiltered
	("No loaded shared libraries match the pattern `%s'.\n", pattern);

    if (loaded_any_symbols)
      {
	/* Getting new symbols may change our opinion about what is
	   frameless.  */
	reinit_frame_cache ();
      }
  }
}

void
_initialize_solib (void)
{
  add_com ("sharedlibrary", class_files, sharedlibrary_command,
	   "Load shared object library symbols for files matching REGEXP.");
  add_info ("sharedlibrary", solib_info,
	    "Status of loaded shared object libraries");

  add_show_from_set
    (add_set_cmd ("auto-solib-add", class_support, var_zinteger,
		  (char *) &auto_solib_add,
		  "Set autoloading of shared library symbols.\n\
If nonzero, symbols from all shared object libraries will be loaded\n\
automatically when the inferior begins execution or when the dynamic linker\n\
informs gdb that a new library has been loaded.  Otherwise, symbols\n\
must be loaded manually, using `sharedlibrary'.",
		  &setlist),
     &showlist);
}
