/* Objective C language support definitions for GDB, the GNU debugger.
   Copyright 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef __STDC__		/* Forward decls for prototypes */
struct value;
#endif

extern int
objc_parse PARAMS ((void));	/* Defined in c-exp.y */

extern void
objc_error PARAMS ((char *));	/* Defined in c-exp.y */

extern void			/* Defined in c-typeprint.c */
c_print_type PARAMS ((struct type *, char *, struct ui_file *, int, int));

extern int
c_val_print PARAMS ((struct type *, char *, int, CORE_ADDR, struct ui_file *, int, int,
		     int, enum val_prettyprint));

extern int
c_value_print PARAMS ((struct value *, struct ui_file *, int, 
		       enum val_prettyprint));

extern CORE_ADDR lookup_objc_class     PARAMS ((char *classname));
extern int       lookup_child_selector PARAMS ((char *methodname));

char *objc_demangle PARAMS ((const char *mangled));

int find_objc_msgcall (CORE_ADDR pc, CORE_ADDR *new_pc);

char *parse_selector
PARAMS ((char *method, char **selector));

char *parse_method
PARAMS ((char *method, char *type, char **class, char **category, char **selector));

void find_methods
PARAMS ((struct symtab *symtab, 
	 char type, const char *class, const char *category, const char *selector,
	 struct symbol **syms, unsigned int *nsym, unsigned int *ndebug));

char *find_imps
PARAMS ((struct symtab *symtab, struct block *block,
 char *method, struct symbol **syms, unsigned int *nsym, unsigned int *ndebug));
