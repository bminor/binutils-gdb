/* Copyright (C) 1990 Free Software Foundation, Inc.

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

/*
**	symbol definitions
*/
#include <sys/types.h>
#include <string.h>
#include <link.h>
#include <sys/param.h>
#include <fcntl.h>
#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "gdbcore.h"
#include "command.h"
#include "target.h"
#include "frame.h"
#include "regex.h"
#include "inferior.h"

extern char *getenv();

/*
**	local data declarations
*/
#define MAX_PATH_SIZE 256
struct so_list {
    struct link_map inferior_lm;		/* inferior link map */
    struct link_map *inferior_lm_add;
    long   ld_text;
    char inferior_so_name[MAX_PATH_SIZE];	/* Shared Object Library Name */
    struct so_list *next;			/* Next Structure */
    char	symbols_loaded;			/* Flag: loaded? */
    char	from_tty;			/* Flag: print msgs? */
    bfd *so_bfd;
    struct section_table *sections;
    struct section_table *sections_end;
};

static struct so_list *so_list_head = 0;

/*
** Build a section map for a shared library, record its text size in
** the so_list structure and set up the text section of the shared lib.
*/
static void
solib_map_sections(so)
struct so_list *so;
{
  char *filename;
  char *scratch_pathname;
  int scratch_chan;
  struct section_table *p;
  
  filename = tilde_expand (so->inferior_so_name);
  make_cleanup (free, filename);
  
  scratch_chan = openp (getenv ("PATH"), 1, filename, O_RDONLY, 0,
			    &scratch_pathname);
  if (scratch_chan < 0)
    scratch_chan = openp (getenv ("LD_LIBRARY_PATH"), 1, filename, O_RDONLY, 0,
			    &scratch_pathname);
  if (scratch_chan < 0)
	perror_with_name (filename);

  so->so_bfd = bfd_fdopenr (scratch_pathname, NULL, scratch_chan);
  if (!so->so_bfd)
    error ("Could not open `%s' as an executable file: %s",
	   scratch_pathname, bfd_errmsg (bfd_error));
  if (!bfd_check_format (so->so_bfd, bfd_object))
    error ("\"%s\": not in executable format: %s.",
	   scratch_pathname, bfd_errmsg (bfd_error));
  if (build_section_table (so->so_bfd, &so->sections, &so->sections_end))
    error ("Can't find the file sections in `%s': %s", 
	   exec_bfd->filename, bfd_errmsg (bfd_error));

  for (p = so->sections; p < so->sections_end; p++)
    {
      if (strcmp (bfd_section_name (so->so_bfd, p->sec_ptr), ".text") == 0)
	{
	  /* Determine length of text section and relocate it. */
	  so->ld_text = p->endaddr - p->addr;
	  p->addr += (CORE_ADDR)so->inferior_lm.lm_addr;
	  p->endaddr += (CORE_ADDR)so->inferior_lm.lm_addr;
	}
      else
	/* All other sections are ignored for now. */
	p->addr = p->endaddr = 0;
    }
}

/*=======================================================================*/

/*	find_solib
**
**Description:
**
**	This module contains the routine which finds the names of any loaded
**	"images" in the current process. The argument in must be NULL on the
**	first call, and then the returned value must be passed in on
**	subsequent calls. This provides the capability to "step" down the
**	list of loaded objects. On the last object, a NULL value is returned.
**	The arg and return value are "struct link_map" pointers, as defined
**	in <link.h>.
**
**	NOTE: This only works under SunOS4.0.
*/

struct so_list *find_solib(so_list_ptr)
struct so_list *so_list_ptr;			/* so_list_head position ptr */
{
struct so_list *so_list_next = 0;
struct link_map *inferior_lm = 0;
struct link_dynamic inferior_dynamic_cpy;
struct link_dynamic_2 inferior_ld_2_cpy;
struct so_list *new;
int i;

     if (!so_list_ptr) {
	 if (!(so_list_next = so_list_head)) {
	     i = lookup_misc_func ("_DYNAMIC");
	     if (i >= 0) {
		 read_memory(misc_function_vector[i].address,
			     &inferior_dynamic_cpy,
			     sizeof(struct link_dynamic));
		 if (inferior_dynamic_cpy.ld_version == 3) {
		     read_memory((CORE_ADDR)inferior_dynamic_cpy.ld_un.ld_2,
				 &inferior_ld_2_cpy,
				 sizeof(struct link_dynamic_2));
		     inferior_lm = inferior_ld_2_cpy.ld_loaded;
		 }
	     }
	 }
     } else {
	 /*
	 ** Advance to next local abbreviated load_map structure
	 */
	 if (!(inferior_lm = so_list_ptr->inferior_lm.lm_next)) {
	     /* See if any were added, but be quiet if we can't read
		from the target any more.  */
	     int status;

	     status = target_read_memory (
			(CORE_ADDR)so_list_ptr->inferior_lm_add,
			(char *)&so_list_ptr->inferior_lm,
			sizeof(struct link_map));
	     if (status == 0)
	       inferior_lm = so_list_ptr->inferior_lm.lm_next;
	     else
	       inferior_lm = 0;
	 }
	 so_list_next = so_list_ptr->next;
     }
     if ((!so_list_next) && inferior_lm) {
	 /* 
	 ** Get Next LM Structure from inferior image and build
	 ** an local abbreviated load_map structure
	 */
	 new = (struct so_list *) xmalloc(sizeof(struct so_list));
         new->inferior_lm_add = inferior_lm;
	 read_memory((CORE_ADDR)inferior_lm,
		     &new->inferior_lm,
		     sizeof(struct link_map));

	 read_memory((CORE_ADDR)new->inferior_lm.lm_name,
		     new->inferior_so_name,
		     MAX_PATH_SIZE - 1);
	 new->inferior_so_name[MAX_PATH_SIZE - 1] = 0;
	 /* Zero everything after the first terminating null */
	 strncpy(new->inferior_so_name, new->inferior_so_name, MAX_PATH_SIZE);

#if 0
	 /* This doesn't work for core files, so instead get ld_text
	    using solib_map_sections (below).  */
	 read_memory((CORE_ADDR)new->inferior_lm.lm_ld,
		     &inferior_dynamic_cpy,
		     sizeof(struct link_dynamic));
	 read_memory((CORE_ADDR)inferior_dynamic_cpy.ld_un.ld_2,
		     &inferior_ld_2_cpy,
		     sizeof(struct link_dynamic_2));
	 new->ld_text = inferior_ld_2_cpy.ld_text;
#endif

	 new->next = 0;
	 new->symbols_loaded = 0;
	 new->so_bfd = NULL;
	 new->sections = NULL;
	 if (so_list_ptr)
	     so_list_ptr->next = new;
	 else
	     so_list_head = new;

	 solib_map_sections (new);

	 so_list_next = new;
     }
     return(so_list_next);
}

/* A small stub to get us past the arg-passing pinhole of catch_errors.  */

static int
symbol_add_stub (arg)
     char *arg;
{
  register struct so_list *so = (struct so_list *)arg;	/* catch_errs bogon */

  symbol_file_add (so->inferior_so_name, so->from_tty,
		   (unsigned int)so->inferior_lm.lm_addr, 0);
  return 1;
}

/* The real work of adding a shared library file to the symtab and
   the section list.  */

void
solib_add (arg_string, from_tty, target)
     char *arg_string;
     int from_tty;
     struct target_ops *target;
{	
  register struct so_list *so = 0;   	/* link map state variable */
  char *val;
  int count, old;
  struct section_table *sec;

  if (arg_string == 0)
      re_comp (".");
  else if (val = (char *) re_comp (arg_string)) {
      error ("Invalid regexp: %s", val);
  }

  /* Getting new symbols may change our opinion about what is
     frameless.  */
  reinit_frame_cache ();

  if (from_tty) {
    printf_filtered ("Shared libraries");
    if (arg_string)
      printf_filtered (" matching regular expresion \"%s\"", arg_string);
    printf_filtered (":\n");
  }
  
  dont_repeat();

  while (so = find_solib(so)) {
      if (re_exec(so->inferior_so_name)) {
	  if (so->symbols_loaded) {
	      if (from_tty)
	        printf("Symbols already loaded for %s\n", so->inferior_so_name);
	  } else {
	      so->symbols_loaded = 1;
	      so->from_tty = from_tty;
       	      catch_errors (symbol_add_stub, (char *)so,
		    "Error while reading shared library symbols:\n");
	  }
      }
  }

  /* Now add the shared library sections to the section table of the
     specified target, if any.  */
  if (target) {
    /* Count how many new section_table entries there are.  */
    so = 0;
    count = 0;
    while (0 != (so = find_solib (so))) {
      count += so->sections_end - so->sections;
    }

    if (count) {
      /* Reallocate the target's section table including the new size.  */
      if (target->sections) {
	old = target->sections_end - target->sections;
	target->sections = (struct section_table *)
	    realloc ((char *)target->sections,
	      (sizeof (struct section_table)) * (count + old));
      } else {
	old = 0;
	target->sections = (struct section_table *)
	    malloc ((sizeof (struct section_table)) * count);
      }
      target->sections_end = target->sections + (count + old);

      /* Add these section table entries to the target's table.  */
      while (0 != (so = find_solib (so))) {
	count = so->sections_end - so->sections;
	bcopy (so->sections, (char *)(target->sections + old), 
	       (sizeof (struct section_table)) * count);
	old += count;
      }
    }
  }
}

/*=======================================================================*/

static void solib_info()
{
register struct so_list *so = 0;  	/* link map state variable */

    while (so = find_solib(so)) {
	if (so == so_list_head) {
	    printf("      Address Range     Syms Read    Shared Object Library\n");
	}
	printf(" 0x%08x - 0x%08x   %s   %s\n", 
	    so->inferior_lm.lm_addr, 
	    so->inferior_lm.lm_addr + so->ld_text - 1,
	    (so->symbols_loaded ? "Yes" : "No "),
	    so->inferior_so_name);
    }
    if (!so_list_head) {
	printf("No shared libraries loaded at this time.\n");	
    }
}

/*
** Called by Insert Breakpoint to see if Address is Shared Library Address 
*/
int
solib_address(address)
     CORE_ADDR address;
{
register struct so_list *so = 0;   	/* link map state variable */

    while (so = find_solib(so)) {
	if ((address >= (CORE_ADDR) so->inferior_lm.lm_addr) &&
	    (address < (CORE_ADDR) so->inferior_lm.lm_addr + so->ld_text))
	  return 1;
    }
    return 0;
}

/*
** Called by free_all_symtabs
*/
void 
clear_solib()
{
struct so_list *next;

  while (so_list_head) {
    if (so_list_head->sections)
      free (so_list_head->sections);
    if (so_list_head->so_bfd)
      bfd_close (so_list_head->so_bfd);
    next = so_list_head->next;
    free(so_list_head);
    so_list_head = next;
  }
}

/* Called by child_create_inferior when the inferior is stopped at its
   first instruction.  */

void 
solib_create_inferior_hook()
{
  struct link_dynamic inferior_dynamic_cpy;
  CORE_ADDR inferior_debug_addr;
  struct ld_debug inferior_debug_cpy;
  int in_debugger;
  CORE_ADDR in_debugger_addr;
  CORE_ADDR breakpoint_addr;
  int i, j;

  /* FIXME:  We should look around in the executable code to find _DYNAMIC,
     if it isn't in the symbol table.  It's not that hard to find... 
     Then we can debug stripped executables using shared library symbols.  */
  i = lookup_misc_func ("_DYNAMIC");
  if (i < 0)			/* Can't find shared lib ptr. */
    return;
  if (misc_function_vector[i].address == 0)	/* statically linked program */
    return;

  /* Get link_dynamic structure */
  j = target_read_memory(misc_function_vector[i].address,
	      &inferior_dynamic_cpy,
	      sizeof(struct link_dynamic));
  if (j)					/* unreadable */
    return;

  /* Calc address of debugger interface structure */
  inferior_debug_addr = (CORE_ADDR)inferior_dynamic_cpy.ldd;
  /* Calc address of `in_debugger' member of debugger interface structure */
  in_debugger_addr = inferior_debug_addr + (CORE_ADDR)((char *)&inferior_debug_cpy.ldd_in_debugger - (char *)&inferior_debug_cpy);
  /* Write a value of 1 to this member.  */
  in_debugger = 1;
  write_memory(in_debugger_addr, &in_debugger, sizeof(in_debugger));

  /* Now run the target.  Seeing `in_debugger' set, it will set a
     breakpoint at some convenient place, remember the original contents
     of that place, and eventually take a SIGTRAP when it runs into the
     breakpoint.  We handle this by restoring the contents of the
     breakpointed location (which is only known after it stops), 
     chasing around to locate the shared libraries that have been
     loaded, then resuming.  */

  clear_proceed_status ();
  stop_soon_quietly = 1;
  target_resume (0, 0);
  wait_for_inferior ();
  while (stop_signal != SIGTRAP)
    {
      /* FIXME, what if child has exit()ed?  Must exit loop somehow */
      target_resume (0, stop_signal);
      wait_for_inferior ();
    }
  stop_soon_quietly = 0;

  /* Set `in_debugger' to zero now.  WHY, is this needed?  */
  in_debugger = 0;
  write_memory(in_debugger_addr, &in_debugger, sizeof(in_debugger));
  read_memory(inferior_debug_addr, &inferior_debug_cpy, sizeof(inferior_debug_cpy));
  /* FIXME: maybe we should add the common symbols from the ldd_cp chain
   * to the misc_function_vector ?
   */
  breakpoint_addr = (CORE_ADDR)inferior_debug_cpy.ldd_bp_addr;
  if (stop_pc - DECR_PC_AFTER_BREAK == breakpoint_addr)
    {
      write_memory(breakpoint_addr, &inferior_debug_cpy.ldd_bp_inst, sizeof(inferior_debug_cpy.ldd_bp_inst));
      if (DECR_PC_AFTER_BREAK)
        {
          stop_pc -= DECR_PC_AFTER_BREAK;
          write_register (PC_REGNUM, stop_pc);
        }
    }
  solib_add ((char *)0, 0, (struct target_ops *)0);
}

void
sharedlibrary_command (args, from_tty)
{
  solib_add (args, from_tty, (struct target_ops *)0);
}

void
_initialize_solib()
{

  add_com("sharedlibrary", class_files, sharedlibrary_command,
	   "Load shared object library symbols for files matching REGEXP.");
  add_info("sharedlibrary", solib_info, 
	   "Status of loaded shared object libraries");
}
