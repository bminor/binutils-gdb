/* Copyright (C) 1990 Free Software Foundation, Inc.

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

/*
**	symbol definitions
*/
#include <sys/types.h>
#include <string.h>
#include <link.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "gdbcore.h"
#include "command.h"
#include "target.h"

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
    int	symbols_loaded;
};

static struct so_list *so_list_head = 0;

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
CORE_ADDR inferior_dynamic_ptr = 0;
struct link_map *inferior_lm = 0;
struct link_dynamic inferior_dynamic_cpy;
struct link_dynamic_2 inferior_ld_2_cpy;
struct so_list *new;
int i;

     if (!so_list_ptr) {
	 if (!(so_list_next = so_list_head)) {
	     for (i = 0; i < misc_function_count; i++) {
		 if (!strcmp (misc_function_vector[i].name, "_DYNAMIC")) {
		     inferior_dynamic_ptr = misc_function_vector[i].address;
		     break;
		 }	
	     }		
	     if (inferior_dynamic_ptr) {
		 read_memory(inferior_dynamic_ptr, &inferior_dynamic_cpy, sizeof(struct link_dynamic));
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
			&so_list_ptr->inferior_lm,
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

	 read_memory((CORE_ADDR)new->inferior_lm.lm_ld,
		     &inferior_dynamic_cpy,
		     sizeof(struct link_dynamic));
	 read_memory((CORE_ADDR)inferior_dynamic_cpy.ld_un.ld_2,
		     &inferior_ld_2_cpy,
		     sizeof(struct link_dynamic_2));
	 new->ld_text = inferior_ld_2_cpy.ld_text;
    
	 new->next = 0;
	 new->symbols_loaded = 0;
	 if (so_list_ptr)
	     so_list_ptr->next = new;
	 else
	     so_list_head = new;
	 so_list_next = new;
     }
     return(so_list_next);
}
/*=======================================================================*/

static void solib_add(arg_string, from_tty)
char *arg_string;
int from_tty;
{	
    register struct so_list *so = 0;   	/* link map state variable */
    char *val;
    int sz;

    if (arg_string == 0)
	re_comp (".");
    else if (val = (char *) re_comp (arg_string)) {
	error ("Invalid regexp: %s", val);
    }
 
    printf_filtered ("All shared libraries");
    if (arg_string)
      printf_filtered (" matching regular expresion \"%s\"", arg_string);
    printf_filtered (":\n");
    
    dont_repeat();

    while (so = find_solib(so)) {
	if (re_exec(so->inferior_so_name)) {
	    if (so->symbols_loaded) {
		printf("Symbols already loaded for %s\n", so->inferior_so_name);
	    } else {
		symbol_file_add (so->inferior_so_name, from_tty,
				 (unsigned int)so->inferior_lm.lm_addr, 0);
		so->symbols_loaded = 1;
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
	    printf("      Address Range      Symbols     Shared Object Library\n");
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
    next = so_list_head->next;
    free(so_list_head);
    so_list_head = next;
  }
    
}

void
_initialize_solib()
{

  add_com("sharedlibrary", class_files, solib_add,
	   "Load shared object library symbols for files matching REGEXP.");
  add_info("sharedlibrary", solib_info, 
	   "Status of loaded shared object libraries");

}
