/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * $Id$ 
 */

/*
  ldmisc.c

*/

#include "sysdep.h"
#include <varargs.h>
#include "bfd.h"

#include "ld.h"
#include "ldmisc.h"
#include "ldlang.h"

/* IMPORTS */

extern char *program_name;

extern FILE *ldlex_input_stack;
extern char *ldfile_input_filename;
extern ld_config_type config;

void
yyerror(arg) 
char *arg;
{ 
  info("%P%F: %S %s\n",arg);
}

extern int errno;
extern   int  sys_nerr;
extern char *sys_errlist[];

/*
 %F error is fatal
 %P print progam name
 %S print script file and linenumber
 %E current bfd error or errno
 %I filename from a lang_input_statement_type
 %B filename from a bfd
 %T symbol table entry
 %X no object output, fail return
 %V hex bfd_vma
 %C Clever filename:linenumber 
 %
*/
void info(va_alist)
va_dcl
{
  char *fmt;
  boolean fatal = false;
  va_list arg;
  va_start(arg);
  fmt = va_arg(arg, char *);
  while (*fmt) {
    while (*fmt != '%' && *fmt != '\0') {
      fputc(*fmt, stderr);
      fmt++;
    }
    if (*fmt == '%') {
      fmt ++;
      switch (*fmt++) {
      case 'X':
	config.make_executable = false;
	break;
      case 'V':
	fprintf(stderr,"%08lx", va_arg(arg, bfd_vma));
	break;
      case 'T':
	{
	  asymbol *symbol = va_arg(arg, asymbol *);
	  if (symbol) {
	    asection *section = symbol->section;
	    if ((symbol->flags & BSF_UNDEFINED) == 0) {
	      char *section_name = section == (asection *)NULL ?
		"absolute" : section->name;
	      fprintf(stderr,"%s (%s)", symbol->name, section_name);
	    }
	    else {
	      fprintf(stderr,"%s", symbol->name);
	    }
	  }
	  else {
	    fprintf(stderr,"no symbol");
	  }
	}
	break;
      case 'B':
	{ 
	  bfd *abfd = va_arg(arg, bfd *);
	  if (abfd->my_archive) {
	    fprintf(stderr,"%s(%s)", abfd->my_archive->filename,
		    abfd->filename);
	  }
	  else {
	    fprintf(stderr,"%s", abfd->filename);

	  }
	}
	break;
      case 'F':
	fatal = true;
	break;
      case 'P':
	fprintf(stderr,"%s", program_name);
	break;
      case 'E':
	/* Replace with the most recent errno explanation */


	fprintf(stderr, bfd_errmsg(bfd_error));


	break;
      case 'I':
	{
	  lang_input_statement_type *i =
	    va_arg(arg,lang_input_statement_type *);
	
	  fprintf(stderr,"%s", i->local_sym_name);
	}
	break;
      case 'S':
	/* Print source script file and line number */

	if (ldlex_input_stack) {
	  extern unsigned int lineno;
	  if (ldfile_input_filename == (char *)NULL) {
	    fprintf(stderr,"command line");
	  }
	  else {
	    fprintf(stderr,"%s:%u", ldfile_input_filename, lineno );
	  }
	}
	else {
	  int ch;
	  int n = 0;
	  fprintf(stderr,"command (before <");
	  ch = lex_input();
	  while (ch != 0 && n < 10) {
	    fprintf(stderr, "%c", ch);
	    ch = lex_input();
	    n++;
	  }
	  fprintf(stderr,")");
	    
	}
	break;
      case 'C':
	{
	  char *filename;
	  char *functionname;
	  unsigned int linenumber;
	  bfd *abfd = va_arg(arg, bfd *);
	  asection *section = va_arg(arg, asection *);
	  asymbol **symbols = va_arg(arg, asymbol **);
	  bfd_vma offset = va_arg(arg, bfd_vma);
	 
	  if (bfd_find_nearest_line(abfd,
				    section,
				    symbols,
				    offset,
				    &filename,
				    &functionname,
				    &linenumber))
	    {
		if (filename == (char *)NULL) 	
		    filename = abfd->filename;
		if (functionname != (char *)NULL)
		    fprintf(stderr,"%s:%u: (%s)", filename, linenumber,  functionname);
		else if (linenumber != 0) 
		    fprintf(stderr,"%s:%u", filename, linenumber);
		else
		    fprintf(stderr,"%s", filename);

	    }
	  else {
	    fprintf(stderr,"%s", abfd->filename);
	  }
	}
	break;
		
      case 's':
	fprintf(stderr,"%s", va_arg(arg, char *));
	break;
      case 'd':
	fprintf(stderr,"%d", va_arg(arg, int));
	break;
      default:
	fprintf(stderr,"%s", va_arg(arg, char *));
	break;
      }
    }
  }
  if (fatal == true) {
    extern char *output_filename;
    if (output_filename)
      unlink(output_filename);
    exit(1);
  }
  va_end(arg);
}


void 
info_assert(file, line)
char *file;
unsigned int line;
{
  info("%F%P internal error %s %d\n", file,line);
}

/* Return a newly-allocated string
   whose contents concatenate those of S1, S2, S3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  size_t len1 = strlen (s1);
  size_t len2 = strlen (s2);
  size_t len3 = strlen (s3);
  char *result = ldmalloc (len1 + len2 + len3 + 1);

  if (len1 != 0)
    memcpy(result, s1, len1);
  if (len2 != 0)
    memcpy(result+len1, s2, len2);
  if (len3 != 0)
    memcpy(result+len1+len2, s2, len3);
  *(result + len1 + len2 + len3) = 0;

  return result;
}



char  *ldmalloc (size)
size_t size;
{
  char * result =  malloc (size);

  if (result == (char *)NULL && size != 0)
    info("%F%P virtual memory exhausted\n");

  return result;
} 



char *buystring(x)
char *x;
{
  size_t  l = strlen(x)+1;
  char *r = ldmalloc(l);
  memcpy(r, x,l);
  return r;
}
