/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <demangle.h>
#include "gprof.h"
#include "cg_arcs.h"
#include "symtab.h"


/*
 * Print name of symbol.  Return number of characters printed.
 */
int
DEFUN(print_name_only, (self), Sym *self)
{
    const char *name = self->name;
    const char *filename;
    char *demangled = 0;
    char buf[PATH_MAX];
    int size = 0;

    if (name) {
	if (!bsd_style_output) {
	    if (name[0] == '_' && name[1] && discard_underscores) {
		name++;
	    } /* if */
	    demangled = cplus_demangle(name, DMGL_ANSI|DMGL_PARAMS);
	    if (demangled) {
		name = demangled;
	    } /* if */
	} /* if */
	printf("%s", name);
	size = strlen(name);
	if (line_granularity && self->file) {
	    filename = self->file->name;
	    if (!print_path) {
		filename = strrchr(filename, '/');
		if (filename) {
		    ++filename;
		} else {
		    filename = self->file->name;
		} /* if */
	    } /* if */
	    sprintf(buf, " (%s:%d)", filename, self->line_num);
	    printf(buf);
	    size += strlen(buf);
	} /* if */
	if (demangled) {
	    free(demangled);
	} /* if */
	DBG(DFNDEBUG, printf("{%d} ", self->cg.top_order));
	DBG(PROPDEBUG, printf("%4.0f%% ", 100.0 * self->cg.prop.fract));
    } /* if */
    return size;
} /* print_name_only */


void
DEFUN(print_name, (self), Sym *self)
{
    print_name_only(self);

    if (self->cg.cyc.num != 0) {
	printf(" <cycle %d>", self->cg.cyc.num);
    } /* if */
    if (self->cg.index != 0) {
	if (self->cg.print_flag) {
	    printf(" [%d]", self->cg.index);
	} else {
	    printf(" (%d)", self->cg.index);
	} /* if */
    } /* if */
} /* print_name */

			/*** end of utils.c ***/
