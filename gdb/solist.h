/* Shared library declarations for GDB, the GNU Debugger.
   Copyright 1990, 91, 92, 93, 94, 95, 96, 98, 1999, 2000
   Free Software Foundation, Inc.

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

#define SO_NAME_MAX_PATH_SIZE 512	/* FIXME: Should be dynamic */

/* Forward declaration for target specific link map information.  This
   struct is opaque to all but the target specific file.  */
struct lm_info;

struct so_list
  {
    /* The following fields of the structure come directly from the
       dynamic linker's tables in the inferior, and are initialized by
       current_sos.  */

    struct so_list *next;	/* next structure in linked list */

    /* A pointer to target specific link map information.  Often this
       will be a copy of struct link_map from the user process, but
       it need not be; it can be any collection of data needed to
       traverse the dynamic linker's data structures. */
    struct lm_info *lm_info;

    /* Shared object file name, exactly as it appears in the
       inferior's link map.  This may be a relative path, or something
       which needs to be looked up in LD_LIBRARY_PATH, etc.  We use it
       to tell which entries in the inferior's dynamic linker's link
       map we've already loaded.  */
    char so_original_name[SO_NAME_MAX_PATH_SIZE];

    /* shared object file name, expanded to something GDB can open */
    char so_name[SO_NAME_MAX_PATH_SIZE];

    /* The following fields of the structure are built from
       information gathered from the shared object file itself, and
       are initialized when we actually add it to our symbol tables.  */

    bfd *abfd;
    CORE_ADDR lmend;		/* upper addr bound of mapped object */
    char symbols_loaded;	/* flag: symbols read in yet? */
    char from_tty;		/* flag: print msgs? */
    struct objfile *objfile;	/* objfile for loaded lib */
    struct section_table *sections;
    struct section_table *sections_end;
    struct section_table *textsection;
  };

struct target_so_ops
  {
    CORE_ADDR (*lm_addr) (struct so_list *so);
    void (*free_so) (struct so_list *so);
    void (*clear_solib) (void);
    void (*solib_create_inferior_hook) (void);
    void (*special_symbol_handling) (void);
    struct so_list *(*current_sos) (void);
    int (*open_symbol_file_object) (void *from_ttyp);
  };

void free_so (struct so_list *so);

/* FIXME: gdbarch needs to control this variable */
extern struct target_so_ops *current_target_so_ops;

#define TARGET_SO_LM_ADDR (current_target_so_ops->lm_addr)
#define TARGET_SO_FREE_SO (current_target_so_ops->free_so)
#define TARGET_SO_CLEAR_SOLIB (current_target_so_ops->clear_solib)
#define TARGET_SO_SOLIB_CREATE_INFERIOR_HOOK \
  (current_target_so_ops->solib_create_inferior_hook)
#define TARGET_SO_SPECIAL_SYMBOL_HANDLING \
  (current_target_so_ops->special_symbol_handling)
#define TARGET_SO_CURRENT_SOS (current_target_so_ops->current_sos)
#define TARGET_SO_OPEN_SYMBOL_FILE_OBJECT \
  (current_target_so_ops->open_symbol_file_object)
