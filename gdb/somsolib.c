/* Handle HP SOM shared libraries for GDB, the GNU Debugger.
   Copyright 1993 Free Software Foundation, Inc.
   
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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Written by the Center for Software Science at the Univerity of Utah
and by Cygnus Support.  */


#include "defs.h"

#include "frame.h"
#include "bfd.h"
#include "som.h"
#include "libhppa.h"
#include "gdbcore.h"
#include "symtab.h"
#include "breakpoint.h"
#include "symfile.h"
#include "objfiles.h"
#include "inferior.h"

/* We don't currently use this structure, but it isn't documented anywhere,
   and we'll likely need it in som_solib_add as more shared library support
   is added.  */

struct som_solib_mapped_entry
{
  /* The name of the library.  */
  char *name;

  /* Version of this structure (it is expected to change again in hpux10.  */
  unsigned char struct_version;

  /* Binding mode for this library.  */
  unsigned char bind_mode;

  /* Version of this library.  */
  short library_version;

  /* Start of text address, link-time text location, end of text address.  */
  CORE_ADDR text_addr;
  CORE_ADDR text_link_addr;
  CORE_ADDR text_end;

  /* Start of data, start of bss and end of data.  */
  CORE_ADDR data_start;
  CORE_ADDR bss_start;
  CORE_ADDR data_end;

  /* Value of linkage pointer (%r19).  */
  CORE_ADDR got_value;

  /* Next entry.  */
  struct som_solib_mapped_entry *next;

  /* There are other fields, but I don't have information as to what is
     contained in them.  */
};

/* Add symbols from shared libraries into the symtab list.  */

void
som_solib_add (arg_string, from_tty, target)
     char *arg_string;
     int from_tty;
     struct target_ops *target;
{	
  struct minimal_symbol *msymbol;
  CORE_ADDR addr;
  int status;
  char buf[4];

  msymbol = lookup_minimal_symbol ("__dld_list", (struct objfile *) NULL);
  if (!msymbol)
    {
      /* Older crt0.o files (hpux8) don't have __dld_list as a symbol,
	 but the data is still available if you know where to look.  */
      msymbol = lookup_minimal_symbol ("__dld_flags", (struct objfile *)NULL);
      if (!msymbol)
	{
	  error ("Unable to find dynamic library list.\n");
	  return;
	}
      addr = SYMBOL_VALUE_ADDRESS (msymbol) - 8;
    }
  else
    addr = SYMBOL_VALUE_ADDRESS (msymbol);

  status = target_read_memory (addr, buf, 4);
  if (status != 0)
    {
      error ("Unable to find dynamic library list.\n");
      return;
    }

  addr = extract_unsigned_integer (buf, 4);

  /* If addr is zero, then we're using an old dynamic loader which
     doesn't maintain __dld_list.  We'll have to use a completely
     different approach to get shared library information.  */
  if (addr == 0)
    goto old_dld;

  /* Using the information in __dld_list is the preferred method
     to get at shared library information.  It doesn't depend on
     any functions in /usr/lib/end.o and has a chance of working
     with hpux10 when it is released.  */
  status = target_read_memory (addr, buf, 4);
  if (status != 0)
    {
      error ("Unable to find dynamic library list.\n");
      return;
    }

  /* addr now holds the address of the first entry in the dynamic
     library list.  */
  addr = extract_unsigned_integer (buf, 4);

  /* Now that we have a pointer to the dynamic library list, walk
     through it and add the symbols for each library.

     Skip the first entry since it's our executable.  */
  status = target_read_memory (addr + 36, buf, 4);
  if (status != 0)
    {
      error ("Error while reading dynamic library list.\n");
      return;
    }
  addr = extract_unsigned_integer (buf, 4);

  while (1)
    {
      CORE_ADDR name_addr, text_addr;
      unsigned int name_len;
      char *name;
      if (addr == 0)
	break;

      /* Get a pointer to the name of this library.  */
      status = target_read_memory (addr, buf, 4);
      if (status != 0)
	{
	  error ("Error while reading dynamic library list.\n");
	  return;
	}
      name_addr = extract_unsigned_integer (buf, 4);
      name_len = 0;
      while (1)
	{
	  target_read_memory (name_addr + name_len, buf, 1);
	  if (status != 0)
	    {
	      error ("Error while reading dynamic library list.\n");
	      return;
	    }
	  name_len++;
	  if (*buf == '\0')
	    break;
	}
      name = alloca (name_len);
      status = target_read_memory (name_addr, name, name_len);
      if (status != 0)
	{
	  error ("Error while reading dynamic library list.\n");
	  return;
	}
      name = obsavestring (name, name_len - 1,
			   &symfile_objfile->symbol_obstack);

      status = target_read_memory (addr + 8, buf, 4);
      if (status != 0)
	{
	  error ("Error while reading dynamic library list.\n");
	  return;
	}
      text_addr = extract_unsigned_integer (buf, 4);

      /* OK, we've got everything we need.  Tell GDB about it.  */
      symbol_file_add (name, from_tty, text_addr, 0, 0, 0);

      /* Get address of the next record.  */
      status = target_read_memory (addr + 36, buf, 4);
      if (status != 0)
	{
	  error ("Error while reading dynamic library list.\n");
	  return;
	}
      addr = extract_unsigned_integer (buf, 4);
    }

  /* Getting new symbols may change our opinion about what is
     frameless.  */
  reinit_frame_cache ();
  return;

old_dld:
  error ("Debugging dynamic executables loaded via the hpux8 dld.sl is not supported.\n");
  return;
}

/* This hook gets called just before the first instruction in the
   inferior process is executed.

   This is our opportunity to set magic flags in the inferior so
   that GDB can be notified when a shared library is mapped in and
   to tell the dynamic linker that a private copy of the library is
   needed (so GDB can set breakpoints in the library).

   __dld_flags is the location of the magic flags; as of this implementation
   there are 3 flags of interest:

   bit 0 when set indicates that private copies of the libraries are needed
   bit 1 when set indicates that the callback hook routine is valid
   bit 2 when set indicates that the dynamic linker should maintain the
         __dld_list structure when loading/unloading libraries.

   Note that shared libraries are not mapped in at this time, so we have
   run the inferior until the libraries are mapped in.  Typically this
   means running until the "_start" is called.  */
   
void 
som_solib_create_inferior_hook()
{
  struct minimal_symbol *msymbol;
  asection *shlib_info;
  unsigned int dld_flags, status;
  char shadow_contents[BREAKPOINT_MAX], buf[4];
  CORE_ADDR anaddr;

  /* First see if the objfile was dynamically linked.  */
  shlib_info = bfd_get_section_by_name (symfile_objfile->obfd, "$SHLIB_INFO$");
  if (!shlib_info)
    return;

  /* It's got a $SHLIB_INFO$ section, make sure it's not empty.  */
  if (bfd_section_size (symfile_objfile->obfd, shlib_info) == 0)
    return;

  /* Get the address of __dld_flags, if no such symbol exists, then we can
     not debug the shared code.  */
  msymbol = lookup_minimal_symbol ("__dld_flags", (struct objfile *) NULL);
  if (msymbol == NULL)
    {
      error ("Unable to find __dld_flags symbol in object file.\n");
      return;
    }

  anaddr = SYMBOL_VALUE_ADDRESS (msymbol);
  /* Read the current contents.  */
  status = target_read_memory (anaddr, buf, 4);
  if (status != 0)
    {
      error ("Unable to read __dld_flags\n");
      return;
    }
  dld_flags = extract_unsigned_integer (buf, 4);

  /* Turn on the flags we care about.  */
  dld_flags |= 0x5;
  store_unsigned_integer (buf, 4, dld_flags);
  status = target_write_memory (anaddr, buf, 4);
  if (status != 0)
    {
      error ("Unable to write __dld_flags\n");
      return;
    }

  /* Now find the address of _start and set a breakpoint there.  */
  msymbol = lookup_minimal_symbol ("_start", symfile_objfile);
  if (msymbol == NULL)
    {
      error ("Unable to find _start symbol in object file.\n");
      return;
    }

  anaddr = SYMBOL_VALUE_ADDRESS (msymbol);
  if (target_insert_breakpoint (anaddr, shadow_contents))
    {
      error ("Unable to set breakpoint at _start.\n");
      return;
    }

  /* Start the process again and wait for it to hit our breakpoint.  */
  clear_proceed_status ();
  stop_soon_quietly = 1;
  stop_signal = TARGET_SIGNAL_0;
  do
    {
      target_resume (-1, 0, stop_signal);
      wait_for_inferior ();
    }
  while (stop_signal != TARGET_SIGNAL_TRAP);
  stop_soon_quietly = 0;

  /* All the libraries should be mapped in now.  Remove our breakpoint and
     read in the symbol tables from the shared libraries.  */
  if (target_remove_breakpoint (anaddr, shadow_contents))
    {
      error ("Unable to remove breakpoint at _start.\n");
      return;
    }

  som_solib_add ((char *) 0, 0, (struct target_ops *) 0);
}
