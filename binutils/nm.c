/* nm.c -- Describe symbol table of a rel file.
   Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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

#include "bfd.h"
#include "sysdep.h"
#include "bucomm.h"
#include "getopt.h"
#include "aout/stab_gnu.h"
#include "aout/ranlib.h"



PROTO(static boolean, display_file, (char *filename));
PROTO(static void, do_one_rel_file, (bfd *file));
PROTO(static unsigned int, filter_symbols, (bfd *file, asymbol **syms,
					 unsigned long symcount));

PROTO(static void, print_symbols, (bfd *file, asymbol **syms,
				     unsigned long symcount));
extern PROTO(int, (*sorters[2][2]), (char *x, char *y));
PROTO(static void, print_symdef_entry, (bfd * abfd));

/* Command options.  */

int external_only = 0;	/* print external symbols only */
int file_on_each_line = 0; /* print file name on each line */
int no_sort = 0;	/* don't sort; print syms in order found */
int print_debug_syms = 0; /* print debugger-only symbols too */
int print_armap = 0;	/* describe __.SYMDEF data in archive files.  */
int reverse_sort = 0;	/* sort in downward(alpha or numeric) order */
int sort_numerically = 0; /* sort in numeric rather than alpha order */
int undefined_only = 0;	/* print undefined symbols only */

boolean print_each_filename = false; /* Ick.  Used in archives. */

/* IMPORT */
extern char *program_name;
extern char *program_version;
extern char *target;

struct option long_options[] = {
	{"debug-syms",	    0, &print_debug_syms,  1},
	{"extern-only",	    0, &external_only,     1},
	{"no-sort",	    0, &no_sort,	   1},
	{"numeric-sort",    0, &sort_numerically,  1},
	{"print-armap",	    0, &print_armap,       1},
	{"print-file-name", 0, &file_on_each_line, 1},
	{"reverse-sort",    0, &reverse_sort,      1},
	{"target", 	    2, (int *)NULL,	   0},
	{"undefined-only",  0, &undefined_only,    1},
	{0, 0, 0, 0}
};

int show_names = 0;

/* Some error-reporting functions */

void
usage ()
{
  fprintf(stderr, "nm %s\nUsage: %s [-agnoprsu] filename...\n",
	  program_version, program_name);
  exit(0);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  int c;			/* sez which option char */
  int ind = 0;			/* used by getopt and ignored by us */
  extern int optind;		/* steps thru options */
  int retval;	
  program_name = *argv;

  bfd_init();

  while ((c = getopt_long(argc, argv, "agnoprsu", long_options, &ind)) != EOF) {
    switch (c) {
    case 'a': print_debug_syms = 1; break;
    case 'g': external_only = 1; break;
    case 'n': sort_numerically = 1; break;
    case 'o': file_on_each_line = 1; break;
    case 'p': no_sort = 1; break;
    case 'r': reverse_sort = 1; break;
    case 's': print_armap = 1; break;
    case 'u': undefined_only = 1; break;
			
    case  0:
      if (!strcmp("target",(long_options[option_index]).name)) {
	target = optarg;
      }
			
      break;			/* we've been given a long option */
			
    default:
      usage ();
    }
  }
	
  /* Strangely, for the shell you should return only a nonzero value
     on sucess -- the inverse of the C sense. */
  
  /* OK, all options now parsed.  If no filename specified, do a.out. */
  if (optind == argc) return !display_file ("a.out");
  
  retval = 0;
  show_names = (argc -optind)>1;
  /* We were given several filenames to do: */
  while (optind < argc) {
    if (!display_file (argv[optind++])) {
      retval++;
    }
  }

  return retval;
}

/** Display a file's stats */

/* goto here is marginally cleaner than the nested if syntax */

static boolean
display_file (filename)
     char *filename;
{
  boolean retval = true;
  bfd *file;
  bfd *arfile = NULL;
	
  file = bfd_openr(filename, target);
  if (file == NULL) {
    fprintf (stderr, "\n%s: can't open '%s'.\n", program_name, filename);
    return false;


  }


  if (bfd_check_format(file, bfd_object)) 
      {
	if (show_names) {
	  printf ("\n%s:\n",filename);
	}
	do_one_rel_file (file);
 
      }
  else if (bfd_check_format (file, bfd_archive)) {
    if (!bfd_check_format (file, bfd_archive)) {
      fprintf (stderr, "%s:  %s: unknown format.\n", program_name, filename);
      retval = false;
      goto closer;
    }

    printf("\n%s:\n", filename);
    if (print_armap) print_symdef_entry (file);
    for (;;) {
      arfile = bfd_openr_next_archived_file (file, arfile);

      if (arfile == NULL) {
	if (bfd_error != no_more_archived_files)
	  bfd_fatal (filename);
	goto closer;
      }
			
      if (!bfd_check_format(arfile, bfd_object))
	printf("%s: not an object file\n", arfile->filename);
      else {
	printf ("\n%s:\n", arfile->filename);
	do_one_rel_file (arfile) ;
      }
    }
  }
  else {
    fprintf (stderr, "\n%s:  %s: unknown format.\n", program_name, filename);
    retval = false;
  }


 closer:
  if (bfd_close(file) == false)
    bfd_fatal (filename);

  return retval;
}


static void
do_one_rel_file (abfd)
     bfd *abfd;
{
  unsigned int storage;
  asymbol **syms;
  unsigned int symcount = 0;

  if (!(bfd_get_file_flags (abfd) & HAS_SYMS)) {
    (void) printf ("No symbols in \"%s\".\n", bfd_get_filename (abfd));
    return;
  }

      
  storage = get_symtab_upper_bound (abfd);
  if (storage == 0) {
  nosymz:
    fprintf (stderr, "%s: Symflags set but there are none?\n",
	     bfd_get_filename (abfd));
    exit (1);
  }

  syms = (asymbol **) xmalloc (storage);

  symcount = bfd_canonicalize_symtab (abfd, syms);
  if (symcount == 0) goto nosymz;

  /* Discard the symbols we don't want to print.
     It's OK to do this in place; we'll free the storage anyway
     (after printing) */

  symcount = filter_symbols (abfd, syms, symcount);
	
  if (!no_sort) 
    qsort((char *) syms, symcount, sizeof (asymbol *),
	  sorters[sort_numerically][reverse_sort]);

  if (print_each_filename && !file_on_each_line)
    printf("\n%s:\n", bfd_get_filename(abfd));
	
  print_symbols (abfd, syms, symcount);
  free (syms);

}

/* Symbol-sorting predicates */
#define valueof(x) (x)->section->vma + (x)->value 
int
numeric_forward (x, y)
     char *x;
     char *y;
{

  return (valueof(*(asymbol **)x) - valueof(*(asymbol **) y));;
}

int
numeric_reverse (x, y)
     char *x;
     char *y;
{
  return (valueof(*(asymbol **)y) - valueof(*(asymbol **) x));

}

int
non_numeric_forward (x, y)
     char *x;
     char *y;
{
  CONST char *xn = (*(asymbol **) x)->name;
  CONST char *yn = (*(asymbol **) y)->name;

  return ((xn == NULL) ? ((yn == NULL) ? 0 : -1) :
	  ((yn == NULL) ? 1 : strcmp (xn, yn)));
}

int
non_numeric_reverse (x, y)
     char *x;
     char *y;
{
  return -(non_numeric_forward (x, y));
}

int (*sorters[2][2])() = {
	{non_numeric_forward, non_numeric_reverse},
	{numeric_forward, numeric_reverse},
};


/* Choose which symbol entries to print;
   compact them downward to get rid of the rest.
   Return the number of symbols to be printed.  */
static unsigned int
filter_symbols (abfd, syms, symcount)
     bfd *abfd;
     asymbol **syms;
     unsigned long symcount;
{
  asymbol **from, **to;
  unsigned int dst_count = 0;
  unsigned int src_count;
  for (from = to = syms, src_count = 0; src_count <symcount; src_count++) {
    int keep = 0;
    flagword flags = (from[src_count])->flags;

    if (undefined_only) {
      keep = (flags & BSF_UNDEFINED);
    } else if (external_only) {
      keep = ((flags & BSF_GLOBAL) || (flags & BSF_UNDEFINED) ||
	      (flags & BSF_FORT_COMM));
    } else {
      keep = 1;
    }
		
    if (!print_debug_syms && ((flags & BSF_DEBUGGING) != 0)) {
      keep = 0;
    }

    if (keep) {
      to[dst_count++] = from[src_count];
    }
  }
	
  return dst_count;
}

static void
print_symbols (abfd, syms, symcount)
     bfd *abfd;
     asymbol **syms;
     unsigned long symcount;
{
  asymbol **sym = syms, **end = syms + symcount;

  for (; sym < end; ++sym) {
    if (file_on_each_line) printf("%s:", bfd_get_filename(abfd));

    if (undefined_only) {
      if ((*sym)->flags & BSF_UNDEFINED)
	puts ((*sym)->name);
    }
    else {
      asymbol *p = *sym;
      if (p) {
	bfd_print_symbol(abfd, stdout, p, bfd_print_symbol_nm);
	putchar('\n');
      }
    }
  }
}

static void
print_symdef_entry (abfd)
     bfd * abfd;
{
  symindex idx = BFD_NO_MORE_SYMBOLS;
  carsym *thesym;
  boolean everprinted = false;

  for (idx = bfd_get_next_mapent (abfd, idx, &thesym);
       idx != BFD_NO_MORE_SYMBOLS;
       idx = bfd_get_next_mapent (abfd, idx, &thesym)) {
    bfd *elt;
    if (!everprinted) {
      printf ("\nArchive index:\n");
      everprinted = true;
    }
    elt = bfd_get_elt_at_index (abfd, idx);
    if (thesym->name != (char *)NULL) {
    printf ("%s in %s\n", thesym->name, bfd_get_filename (elt));
}
  }
}
