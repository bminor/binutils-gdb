/* ldmisc.c
   Copyright (C) 1991 Free Software Foundation, Inc.

   Written by Steve Chamberlain of Cygnus Support.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include <varargs.h>
#include <demangle.h>

#include "ld.h"
#include "ldmisc.h"
#include "ldlang.h"
#include "ldlex.h"
/* IMPORTS */

extern char *program_name;

extern FILE *ldlex_input_stack;
extern char *ldfile_input_filename;
extern ld_config_type config;


extern int errno;
extern   int  sys_nerr;
extern char *sys_errlist[];

/* VARARGS*/
static void finfo ();

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
 %v hex bfd_vma, no leading zeros
 %C Clever filename:linenumber 
 %R info about a relent
 %
*/
extern bfd *output_bfd;

static char *
demangle(string, remove_underscore)
char *string;
int remove_underscore;
{
  char *res;
  if (remove_underscore && output_bfd) 
  {
    if (bfd_get_symbol_leading_char(output_bfd) == string[0])
     string++;
  }
  /* Note that there's a memory leak here, we keep buying memory
     for demangled names, and never free.  But if you have so many
     errors that you run out of VM with the error messages, then
     there's something up */
  res = cplus_demangle(string, DMGL_ANSI|DMGL_PARAMS);
  return res ? res : string;
}

static void
vfinfo(fp, fmt, arg)
     FILE *fp;
     char *fmt;
     va_list arg;
{
  boolean fatal = false;

  while (*fmt) 
  {
    while (*fmt != '%' && *fmt != '\0') 
    {
      putc(*fmt, fp);
      fmt++;
    }

    if (*fmt == '%') 
    {
      fmt ++;
      switch (*fmt++) 
      {
       case 'X':
	config.make_executable = false;
	break;

       case 'V':
	{
	  bfd_vma value = va_arg(arg, bfd_vma);
	  fprintf_vma(fp, value);
	}
	break;

      case 'v':
	{
	  char buf[100];
	  char *p = buf;
	  bfd_vma value = va_arg (arg, bfd_vma);
	  sprintf_vma (p, value);
	  while (*p == '0')
	    p++;
	  if (!*p)
	    p--;
	  fputs (p, fp);
	}
	break;

       case 'T':
       {
	 asymbol *symbol = va_arg(arg, asymbol *);
	 if (symbol) 
	 {
	   asection *section = symbol->section;
	   char *cplusname =   demangle(symbol->name, 1);
	   CONST char *section_name =  section->name;
	   if (section != &bfd_und_section) 
	   {
	     fprintf(fp,"%s (%s)", cplusname, section_name);
	   }
	   else 
	   {
	     fprintf(fp,"%s", cplusname);
	   }
	 }
	 else 
	 {
	   fprintf(fp,"no symbol");
	 }
       }
	break;

       case 'B':
       { 
	 bfd *abfd = va_arg(arg, bfd *);
	 if (abfd->my_archive) {
	   fprintf(fp,"%s(%s)", abfd->my_archive->filename,
		   abfd->filename);
	 }
	 else {
	   fprintf(fp,"%s", abfd->filename);
	 }
       }
	break;

       case 'F':
	fatal = true;
	break;

       case 'P':
	fprintf(fp,"%s", program_name);
	break;

       case 'E':
	/* Replace with the most recent errno explanation */
	fprintf(fp, bfd_errmsg(bfd_error));
	break;

       case 'I':
       {
	 lang_input_statement_type *i =
	  va_arg(arg,lang_input_statement_type *);
	
	 fprintf(fp,"%s", i->local_sym_name);
       }
	break;

       case 'S':
	/* Print source script file and line number */
       {
	 extern unsigned int lineno;
	 if (ldfile_input_filename == (char *)NULL) {
	   fprintf(fp,"command line");
	 }
	 else {
	   fprintf(fp,"%s:%u", ldfile_input_filename, lineno );
	 }
       }
	break;

       case 'R':
	/* Print all that's interesting about a relent */
       {
	 arelent *relent = va_arg(arg, arelent *);
	
	 finfo (fp, "%s+0x%v (type %s)",
		(*(relent->sym_ptr_ptr))->name,
		relent->addend,
		relent->howto->name);
       }
	break;
	
       case 'C':
       {
	 CONST char *filename;
	 CONST char *functionname;
	 char *cplus_name;
	 
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
	   {
	     cplus_name = demangle(functionname, 1);
	     fprintf(fp,"%s:%u: (%s)", filename, linenumber, cplus_name);
	   }
		
	   else if (linenumber != 0) 
	    fprintf(fp,"%s:%u", filename, linenumber);
	   else
	     finfo (fp, "%s(%s+0x%v)", filename, section->name, offset);

	 }
	 else
	   finfo (fp, "%s(%s+0x%v)", abfd->filename, section->name, offset);
       }
	break;
		
       case 's':
	fprintf(fp,"%s", va_arg(arg, char *));
	break;

       case 'd':
	fprintf(fp,"%d", va_arg(arg, int));
	break;

       default:
	fprintf(fp,"%s", va_arg(arg, char *));
	break;
      }
    }
  }

  if (fatal == true) 
  {
    extern char *output_filename;
    if (output_filename) 
    {
      char *new = malloc(strlen(output_filename)+2);
      extern bfd *output_bfd;
      
      strcpy(new, output_filename);
      if (output_bfd && output_bfd->iostream)
       fclose((FILE *)(output_bfd->iostream));
      unlink(new);
    }
    exit(1);
  }
}

/* Format info message and print on stdout. */

void info(va_alist)
va_dcl
{
  char *fmt;
  va_list arg;
  va_start(arg);
  fmt = va_arg(arg, char *);
  vfinfo(stdout, fmt, arg);
  va_end(arg);
}

/* ('e' for error.) Format info message and print on stderr. */

void einfo(va_alist)
va_dcl
{
  char *fmt;
  va_list arg;
  va_start(arg);
  fmt = va_arg(arg, char *);
  vfinfo(stderr, fmt, arg);
  va_end(arg);
}

/* Warn about a symbol NEWSYM being multiply defined with another symbol OLDSYM.
   MESSAGE1 and MESSAGE2 should look something like:
   "%C: warning: multiple commons of `%s'\n"
   "%C: warning: previous common here\n"  */

void
multiple_warn (message1, newsym, message2, oldsym)
     char *message1;
     asymbol *newsym;
     char *message2;
     asymbol *oldsym;
{
  lang_input_statement_type *stat;
  asymbol **stat_symbols;

  stat = (lang_input_statement_type *) bfd_asymbol_bfd (newsym)->usrdata;
  stat_symbols = stat ? stat->asymbols : 0;

  einfo (message1,
	 bfd_asymbol_bfd (newsym), newsym->section, stat_symbols, newsym->value,
	 demangle (newsym->name, 1));

  stat = (lang_input_statement_type *) bfd_asymbol_bfd (oldsym)->usrdata;
  stat_symbols = stat ? stat->asymbols : 0;

  einfo (message2,
	 bfd_asymbol_bfd (oldsym), oldsym->section, stat_symbols, oldsym->value);
}

void 
info_assert(file, line)
     char *file;
     unsigned int line;
{
  einfo("%F%P internal error %s %d\n", file,line);
}

/* Return a newly-allocated string
   whose contents concatenate those of S1, S2, S3.  */

char *
concat (s1, s2, s3)
     CONST char *s1;
     CONST char *s2;
     CONST char *s3;
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


PTR
ldmalloc (size)
     size_t size;
{
  PTR result =  malloc ((int)size);

  if (result == (char *)NULL && size != 0)
    einfo("%F%P virtual memory exhausted\n");

  return result;
} 

PTR
xmalloc (size)
     int size;
{
return ldmalloc(size);
}


PTR
ldrealloc (ptr, size)
     PTR ptr;
     size_t size;
{
  PTR result =  realloc (ptr, (int)size);

  if (result == (char *)NULL && size != 0)
    einfo("%F%P virtual memory exhausted\n");

  return result;
} 

PTR
xrealloc (ptr, size)
     PTR ptr;
     size_t size;
{
return ldrealloc(ptr, size);
}


char *
buystring (x)
     CONST char *CONST x;
{
  size_t l = strlen(x)+1;
  char *r = ldmalloc(l);
  memcpy(r, x,l);
  return r;
}


/* ('m' for map) Format info message and print on map. */

void minfo(va_alist)
va_dcl
{
  char *fmt;
  va_list arg;
  va_start(arg);
  fmt = va_arg(arg, char *);
  vfinfo(config.map_file, fmt, arg);
  va_end(arg);
}


static void
finfo (va_alist)
     va_dcl
{
  char *fmt;
  FILE *file;
  va_list arg;
  va_start (arg);
  file = va_arg (arg, FILE *);
  fmt = va_arg (arg, char *);
  vfinfo (file, fmt, arg);
  va_end (arg);
}



/*----------------------------------------------------------------------
  Functions to print the link map 
 */

void 
print_space ()
{
  fprintf(config.map_file, " ");
}
void 
print_nl ()
{
  fprintf(config.map_file, "\n");
}
void 
print_address (value)
     bfd_vma value;
{
  fprintf_vma(config.map_file, value);
}
