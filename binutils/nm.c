/* nm.c -- Describe symbol table of a rel file.
   Copyright 1991, 1992 Free Software Foundation, Inc.

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

static boolean
display_file PARAMS ((char *filename));

static void
do_one_rel_file PARAMS ((bfd* file, bfd *archive));

static unsigned int
filter_symbols PARAMS ((bfd *file, asymbol **syms, unsigned long symcount));

static void
print_symbols PARAMS ((bfd *file, asymbol **syms, unsigned long symcount,
		       bfd *archive));

static void
print_symdef_entry PARAMS ((bfd * abfd));

/* Command options.  */

int external_only = 0;	/* print external symbols only */
int file_on_each_line = 0; /* print file name on each line */
int no_sort = 0;	/* don't sort; print syms in order found */
int print_debug_syms = 0; /* print debugger-only symbols too */
int print_armap = 0;	/* describe __.SYMDEF data in archive files.  */
int reverse_sort = 0;	/* sort in downward(alpha or numeric) order */
int sort_numerically = 0; /* sort in numeric rather than alpha order */
int undefined_only = 0;	/* print undefined symbols only */
int show_version = 0;	/* show the version number */

boolean print_each_filename = false; /* Ick.  Used in archives. */

/* IMPORT */
extern char *program_name;
extern char *program_version;
extern char *target;
extern int print_version;

struct option long_options[] = {
	{"debug-syms",	    no_argument, &print_debug_syms,  1},
	{"extern-only",	    no_argument, &external_only,     1},
	{"no-sort",	    no_argument, &no_sort,	     1},
	{"numeric-sort",    no_argument, &sort_numerically,  1},
	{"print-armap",	    no_argument, &print_armap,       1},
	{"print-file-name", no_argument, &file_on_each_line, 1},
	{"reverse-sort",    no_argument, &reverse_sort,      1},
	{"target", 	    optional_argument, 0,            200},
	{"undefined-only",  no_argument, &undefined_only,    1},
	{"version",         no_argument, &show_version,      1},
	{0, no_argument, 0, 0}
};

int show_names = 0;

/* Some error-reporting functions */

void
usage ()
{
  fprintf(stderr, "nm %s\n\
Usage: %s [-agnoprsuV] [--debug-syms] [--extern-only] [--print-armap]\n\
       [--print-file-name] [--numeric-sort] [--no-sort] [--reverse-sort]\n\
       [--undefined-only] [--target=bfdname] [file...]\n",
	  program_version, program_name);
  exit(1);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  int c;			/* sez which option char */
  int retval;
  program_name = *argv;

  bfd_init();

  while ((c = getopt_long(argc, argv, "agnoprsuvABV", long_options, (int *) 0)) != EOF) {
    switch (c) {
    case 'a': print_debug_syms = 1; break;
    case 'g': external_only = 1; break;
    case 'n': sort_numerically = 1; break;
    case 'o': file_on_each_line = 1; break;
    case 'p': no_sort = 1; break;
    case 'r': reverse_sort = 1; break;
    case 's': print_armap = 1; break;
    case 'u': undefined_only = 1; break;
    case 'v':
    case 'V': show_version = 1; break;

    /* For MIPS compatibility, -A selects System V style output, -B
       selects BSD style output.  These are not implemented.  When
       they are, they should be added to usage ().  */
    case 'A': break;
    case 'B': break;

    case 200:			/* --target */
      target = optarg;
      break;

    default:
      usage ();
    }
  }

  if (show_version)
	printf ("%s version %s\n", program_name, program_version);

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

/* Display a file's stats */

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
    fprintf (stderr, "%s: ", program_name);
    bfd_perror (filename);
    return false;
  }

  if (bfd_check_format(file, bfd_object))
      {
	if (show_names) {
	  printf ("\n%s:\n",filename);
	}
	do_one_rel_file (file, NULL);
      }
  else if (bfd_check_format (file, bfd_archive)) {
    if (!bfd_check_format (file, bfd_archive)) {
      fprintf (stderr, "%s: %s: unknown format\n", program_name, filename);
      retval = false;
      goto closer;
    }

    if (!file_on_each_line)
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
	if (!file_on_each_line)
	  printf ("\n%s:\n", arfile->filename);
	do_one_rel_file (arfile, file) ;
      }
    }
  }
  else {
    fprintf (stderr, "\n%s: %s: unknown format\n", program_name, filename);
    retval = false;
  }

 closer:
  if (bfd_close(file) == false)
    bfd_fatal (filename);

  return retval;
}

/* Symbol-sorting predicates */
#define valueof(x) ((x)->section->vma + (x)->value)
int
numeric_forward (x, y)
     CONST void *x;
     CONST void *y;
{
  return (valueof(*(asymbol **)x) - valueof(*(asymbol **) y));
}

int
numeric_reverse (x, y)
     CONST void *x;
     CONST void *y;
{
  return (valueof(*(asymbol **)y) - valueof(*(asymbol **) x));
}

int
non_numeric_forward (x, y)
     CONST void *x;
     CONST void *y;
{
  CONST char *xn = (*(asymbol **) x)->name;
  CONST char *yn = (*(asymbol **) y)->name;

  return ((xn == NULL) ? ((yn == NULL) ? 0 : -1) :
	  ((yn == NULL) ? 1 : strcmp (xn, yn)));
}

int
non_numeric_reverse (x, y)
     CONST void *x;
     CONST void *y;
{
  return -(non_numeric_forward (x, y));
}

int (*(sorters[2][2])) PARAMS ((CONST void *, CONST void *)) = {
	{non_numeric_forward, non_numeric_reverse},
	{numeric_forward, numeric_reverse},
};

static void
do_one_rel_file (abfd, archive_bfd)
     bfd *abfd;
     bfd *archive_bfd; /* If non-NULL: archive containing abfd. */
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
    return;
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

  print_symbols (abfd, syms, symcount, archive_bfd);
  free (syms);
}

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
  asymbol *sym;

  unsigned int src_count;
  for (from = to = syms, src_count = 0; src_count <symcount; src_count++) {
    int keep = 0;

    flagword flags = (from[src_count])->flags;
    sym = from[src_count];
    if (undefined_only) {
      keep = sym->section == &bfd_und_section;
    } else if (external_only) {
      keep = ((flags & BSF_GLOBAL)
	      || (sym->section == &bfd_und_section)
	      || (bfd_is_com_section (sym->section)));
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
print_symbols (abfd, syms, symcount, archive_bfd)
     bfd *abfd;
     asymbol **syms;
     unsigned long symcount;
     bfd *archive_bfd;
{
  asymbol **sym = syms, **end = syms + symcount;

  for (; sym < end; ++sym) {
    if (file_on_each_line) {
      if (archive_bfd)
	printf("%s:", bfd_get_filename(archive_bfd));
      printf("%s:", bfd_get_filename(abfd));
    }

    if (undefined_only) {
      if ((*sym)->section == &bfd_und_section)
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
