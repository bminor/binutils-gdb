/* nm.c -- Describe symbol table of a rel file.
   Copyright 1991, 92, 93, 94 Free Software Foundation, Inc.

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
#include "demangle.h"

static boolean
display_file PARAMS ((char *filename));

static void
display_rel_file PARAMS ((bfd * file, bfd * archive));

static unsigned int
filter_symbols PARAMS ((bfd * file, asymbol ** syms, unsigned long symcount));

static void
print_symbols PARAMS ((bfd * file, asymbol ** syms, unsigned long symcount,
		       bfd * archive));

static void
print_symdef_entry PARAMS ((bfd * abfd));


/* The output formatting functions.  */

static void
print_object_filename_bsd PARAMS ((char *filename));

static void
print_object_filename_sysv PARAMS ((char *filename));

static void
print_object_filename_posix PARAMS ((char *filename));


static void
print_archive_filename_bsd PARAMS ((char *filename));

static void
print_archive_filename_sysv PARAMS ((char *filename));

static void
print_archive_filename_posix PARAMS ((char *filename));


static void
print_archive_member_bsd PARAMS ((char *archive, CONST char *filename));

static void
print_archive_member_sysv PARAMS ((char *archive, CONST char *filename));

static void
print_archive_member_posix PARAMS ((char *archive, CONST char *filename));


static void
print_symbol_filename_bsd PARAMS ((bfd * archive_bfd, bfd * abfd));

static void
print_symbol_filename_sysv PARAMS ((bfd * archive_bfd, bfd * abfd));

static void
print_symbol_filename_posix PARAMS ((bfd * archive_bfd, bfd * abfd));


static void
print_symbol_info_bsd PARAMS ((symbol_info * info, bfd * abfd));

static void
print_symbol_info_sysv PARAMS ((symbol_info * info, bfd * abfd));

static void
print_symbol_info_posix PARAMS ((symbol_info * info, bfd * abfd));


/* Support for different output formats.  */
struct output_fns
  {
    /* Print the name of an object file given on the command line.  */
    void (*print_object_filename) PARAMS ((char *filename));

    /* Print the name of an archive file given on the command line.  */
    void (*print_archive_filename) PARAMS ((char *filename));

    /* Print the name of an archive member file.  */
    void (*print_archive_member) PARAMS ((char *archive, CONST char *filename));

    /* Print the name of the file (and archive, if there is one)
       containing a symbol.  */
    void (*print_symbol_filename) PARAMS ((bfd * archive_bfd, bfd * abfd));

    /* Print a line of information about a symbol.  */
    void (*print_symbol_info) PARAMS ((symbol_info * info, bfd * abfd));
  };
static struct output_fns formats[] =
{
  {print_object_filename_bsd,
   print_archive_filename_bsd,
   print_archive_member_bsd,
   print_symbol_filename_bsd,
   print_symbol_info_bsd},
  {print_object_filename_sysv,
   print_archive_filename_sysv,
   print_archive_member_sysv,
   print_symbol_filename_sysv,
   print_symbol_info_sysv},
  {print_object_filename_posix,
   print_archive_filename_posix,
   print_archive_member_posix,
   print_symbol_filename_posix,
   print_symbol_info_posix}
};

/* Indices in `formats'.  */
#define FORMAT_BSD 0
#define FORMAT_SYSV 1
#define FORMAT_POSIX 2
#define FORMAT_DEFAULT FORMAT_BSD

/* The output format to use.  */
static struct output_fns *format = &formats[FORMAT_DEFAULT];


/* Command options.  */

static int do_demangle = 0;	/* Pretty print C++ symbol names.  */
static int external_only = 0;	/* print external symbols only */
static int no_sort = 0;		/* don't sort; print syms in order found */
static int print_debug_syms = 0;	/* print debugger-only symbols too */
static int print_armap = 0;	/* describe __.SYMDEF data in archive files.  */
static int reverse_sort = 0;	/* sort in downward(alpha or numeric) order */
static int sort_numerically = 0;	/* sort in numeric rather than alpha order */
static int undefined_only = 0;	/* print undefined symbols only */
static int dynamic = 0;		/* print dynamic symbols.  */
static int show_version = 0;	/* show the version number */

/* When to print the names of files.  Not mutually exclusive in SYSV format.  */
static int filename_per_file = 0;	/* Once per file, on its own line.  */
static int filename_per_symbol = 0;	/* Once per symbol, at start of line.  */

/* Print formats for printing a symbol value.  */
#ifdef	BFD_HOST_64_BIT
static char value_format[] = "%08x%08x";
#else
static char value_format[] = "%08lx";
#endif
/* Print formats for printing stab info.  */
static char other_format[] = "%02x";
static char desc_format[] = "%04x";

/* IMPORT */
extern char *program_name;
extern char *program_version;
extern char *target;
extern int print_version;

static struct option long_options[] =
{
  {"debug-syms", no_argument, &print_debug_syms, 1},
  {"demangle", no_argument, &do_demangle, 1},
  {"dynamic", no_argument, &dynamic, 1},
  {"extern-only", no_argument, &external_only, 1},
  {"format", required_argument, 0, 'f'},
  {"help", no_argument, 0, 'h'},
  {"no-cplus", no_argument, &do_demangle, 0},  /* Linux compatibility.  */
  {"no-demangle", no_argument, &do_demangle, 0},
  {"no-sort", no_argument, &no_sort, 1},
  {"numeric-sort", no_argument, &sort_numerically, 1},
  {"portability", no_argument, 0, 'P'},
  {"print-armap", no_argument, &print_armap, 1},
  {"print-file-name", no_argument, 0, 'o'},
  {"radix", required_argument, 0, 't'},
  {"reverse-sort", no_argument, &reverse_sort, 1},
  {"target", required_argument, 0, 200},
  {"undefined-only", no_argument, &undefined_only, 1},
  {"version", no_argument, &show_version, 1},
  {0, no_argument, 0, 0}
};

/* Some error-reporting functions */

void
usage (stream, status)
     FILE *stream;
     int status;
{
  fprintf (stream, "\
Usage: %s [-aABCDgnopPrsuvV] [-t radix] [--radix=radix] [--target=bfdname]\n\
       [--debug-syms] [--extern-only] [--print-armap] [--print-file-name]\n\
       [--numeric-sort] [--no-sort] [--reverse-sort] [--undefined-only]\n\
       [--portability] [-f {bsd,sysv,posix}] [--format={bsd,sysv,posix}]\n\
       [--demangle] [--no-demangle] [--dynamic] [--version] [--help]\n\
       [file...]\n",
	   program_name);
  exit (status);
}

/* Set the radix for the symbol value and size according to RADIX.  */

void
set_print_radix (radix)
     char *radix;
{
  switch (*radix)
    {
    case 'd':
    case 'o':
    case 'x':
#ifdef	BFD_HOST_64_BIT
      value_format[3] = value_format[7] = *radix;
#else
      value_format[4] = *radix;
#endif
      other_format[3] = desc_format[3] = *radix;
      break;
    default:
      fprintf (stderr, "%s: %s: invalid radix\n", program_name, radix);
      exit (1);
    }
}

void
set_output_format (f)
     char *f;
{
  int i;

  switch (*f)
    {
    case 'b':
    case 'B':
      i = FORMAT_BSD;
      break;
    case 'p':
    case 'P':
      i = FORMAT_POSIX;
      break;
    case 's':
    case 'S':
      i = FORMAT_SYSV;
      break;
    default:
      fprintf (stderr, "%s: %s: invalid output format\n", program_name, f);
      exit (1);
    }
  format = &formats[i];
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  int c;
  int retval;

  program_name = *argv;
  xmalloc_set_program_name (program_name);

  bfd_init ();

  while ((c = getopt_long (argc, argv, "aABCDf:gnopPrst:uvV", long_options, (int *) 0)) != EOF)
    {
      switch (c)
	{
	case 'a':
	  print_debug_syms = 1;
	  break;
	case 'A':
	case 'o':
	  filename_per_symbol = 1;
	  break;
	case 'B':		/* For MIPS compatibility.  */
	  set_output_format ("bsd");
	  break;
	case 'C':
	  do_demangle = 1;
	  break;
	case 'D':
	  dynamic = 1;
	  break;
	case 'f':
	  set_output_format (optarg);
	  break;
	case 'g':
	  external_only = 1;
	  break;
	case 'h':
	  usage (stdout, 0);
	case 'n':
	case 'v':
	  sort_numerically = 1;
	  break;
	case 'p':
	  no_sort = 1;
	  break;
	case 'P':
	  set_output_format ("posix");
	  break;
	case 'r':
	  reverse_sort = 1;
	  break;
	case 's':
	  print_armap = 1;
	  break;
	case 't':
	  set_print_radix (optarg);
	  break;
	case 'u':
	  undefined_only = 1;
	  break;
	case 'V':
	  show_version = 1;
	  break;

	case 200:		/* --target */
	  target = optarg;
	  break;

	case 0:		/* A long option that just sets a flag.  */
	  break;

	default:
	  usage (stderr, 1);
	}
    }

  if (show_version)
    {
      printf ("GNU %s version %s\n", program_name, program_version);
      exit (0);
    }

  /* OK, all options now parsed.  If no filename specified, do a.out.  */
  if (optind == argc)
    return !display_file ("a.out");

  retval = 0;

  if (argc - optind > 1)
    filename_per_file = 1;

  /* We were given several filenames to do.  */
  while (optind < argc)
    {
      if (!display_file (argv[optind++]))
	retval++;
    }

  exit (retval);
  return retval;
}

static void
display_archive (file)
     bfd *file;
{
  bfd *arfile = NULL;
  bfd *last_arfile = NULL;
  char **matching;

  (*format->print_archive_filename) (bfd_get_filename (file));

  if (print_armap)
    print_symdef_entry (file);

  for (;;)
    {
      arfile = bfd_openr_next_archived_file (file, arfile);

      if (arfile == NULL)
	{
	  if (bfd_get_error () != bfd_error_no_more_archived_files)
	    bfd_fatal (bfd_get_filename (file));
	  break;
	}

      if (bfd_check_format_matches (arfile, bfd_object, &matching))
	{
	  (*format->print_archive_member) (bfd_get_filename (file),
					   bfd_get_filename (arfile));
	  display_rel_file (arfile, file);
	}
      else
	{
	  bfd_nonfatal (bfd_get_filename (arfile));
	  if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	    {
	      list_matching_formats (matching);
	      free (matching);
	    }
	}

      if (last_arfile != NULL)
	bfd_close (last_arfile);
      last_arfile = arfile;
    }

  if (last_arfile != NULL)
    bfd_close (last_arfile);
}

static boolean
display_file (filename)
     char *filename;
{
  boolean retval = true;
  bfd *file;
  char **matching;

  file = bfd_openr (filename, target);
  if (file == NULL)
    {
      bfd_nonfatal (filename);
      return false;
    }

  if (bfd_check_format (file, bfd_archive))
    {
      display_archive (file);
    }
  else if (bfd_check_format_matches (file, bfd_object, &matching))
    {
      (*format->print_object_filename) (filename);
      display_rel_file (file, NULL);
    }
  else
    {
      bfd_nonfatal (filename);
      if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	{
	  list_matching_formats (matching);
	  free (matching);
	}
      retval = false;
    }

  if (bfd_close (file) == false)
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
  return (valueof (*(asymbol **) x) - valueof (*(asymbol **) y));
}

int
numeric_reverse (x, y)
     CONST void *x;
     CONST void *y;
{
  return (valueof (*(asymbol **) y) - valueof (*(asymbol **) x));
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

static int (*(sorters[2][2])) PARAMS ((CONST void *, CONST void *)) =
{
  { non_numeric_forward, non_numeric_reverse },
  { numeric_forward, numeric_reverse }
};

/* If ARCHIVE_BFD is non-NULL, it is the archive containing ABFD.  */

static void
display_rel_file (abfd, archive_bfd)
     bfd *abfd;
     bfd *archive_bfd;
{
  long storage;
  asymbol **syms;
  long symcount = 0;

  if (dynamic)
    {
      if (!(bfd_get_file_flags (abfd) & DYNAMIC))
	{
	  printf ("\"%s\" is not a dynamic object.\n",
		  bfd_get_filename (abfd));
	  return;
	}
    }
  else
    {
      if (!(bfd_get_file_flags (abfd) & HAS_SYMS))
	{
	  printf ("No symbols in \"%s\".\n", bfd_get_filename (abfd));
	  return;
	}
    }

  if (dynamic)
    storage = bfd_get_dynamic_symtab_upper_bound (abfd);
  else
    storage = bfd_get_symtab_upper_bound (abfd);
  if (storage < 0)
    bfd_fatal (bfd_get_filename (abfd));
  if (storage == 0)
    {
    nosymz:
      if (dynamic)
	fprintf (stderr, "%s: no symbols\n", bfd_get_filename (abfd));
      else
	fprintf (stderr, "%s: Symflags set but there are none?\n",
		 bfd_get_filename (abfd));
      return;
    }

  syms = (asymbol **) xmalloc (storage);

  if (dynamic)
    symcount = bfd_canonicalize_dynamic_symtab (abfd, syms);
  else
    symcount = bfd_canonicalize_symtab (abfd, syms);
  if (symcount < 0)
    bfd_fatal (bfd_get_filename (abfd));
  if (symcount == 0)
    {
      free (syms);
      goto nosymz;
    }

  /* Discard the symbols we don't want to print.
     It's OK to do this in place; we'll free the storage anyway
     (after printing).  */

  symcount = filter_symbols (abfd, syms, symcount);

  if (!no_sort)
    qsort ((char *) syms, symcount, sizeof (asymbol *),
	   sorters[sort_numerically][reverse_sort]);

  print_symbols (abfd, syms, symcount, archive_bfd);
  free (syms);
}

/* Choose which symbol entries to print;
   compact them downward to get rid of the rest.
   Return the number of symbols to be printed.  */

static unsigned int
filter_symbols (abfd, syms, symcount)
     bfd *abfd;			/* Unused.  */
     asymbol **syms;
     unsigned long symcount;
{
  asymbol **from, **to;
  unsigned int src_count;
  unsigned int dst_count = 0;
  asymbol *sym;

  for (from = to = syms, src_count = 0; src_count < symcount; src_count++)
    {
      int keep = 0;
      flagword flags = (from[src_count])->flags;

      sym = from[src_count];
      if (undefined_only)
	keep = sym->section == &bfd_und_section;
      else if (external_only)
	keep = ((flags & BSF_GLOBAL)
		|| (sym->section == &bfd_und_section)
		|| (bfd_is_com_section (sym->section)));
      else
	keep = 1;

      if (!print_debug_syms && ((flags & BSF_DEBUGGING) != 0))
	keep = 0;

      if (keep)
	to[dst_count++] = from[src_count];
    }

  return dst_count;
}

/* Print symbol name NAME, read from ABFD, with printf format FORMAT,
   demangling it if requested.  */

static void
print_symname (format, name, abfd)
     char *format, *name;
     bfd *abfd;
{
  if (do_demangle)
    {
      char *res;

      /* In this mode, give a user-level view of the symbol name
	 even if it's not mangled; strip off any leading
	 underscore.  */
      if (bfd_get_symbol_leading_char (abfd) == name[0])
	name++;

      res = cplus_demangle (name, DMGL_ANSI | DMGL_PARAMS);
      if (res)
	{
	  printf (format, res);
	  free (res);
	  return;
	}
    }

  printf (format, name);
}

/* If ARCHIVE_BFD is non-NULL, it is the archive containing ABFD.  */

static void
print_symbols (abfd, syms, symcount, archive_bfd)
     bfd *abfd;
     asymbol **syms;
     unsigned long symcount;
     bfd *archive_bfd;
{
  asymbol **sym = syms, **end = syms + symcount;
  symbol_info syminfo;

  for (; sym < end; ++sym)
    {
      (*format->print_symbol_filename) (archive_bfd, abfd);

      if (undefined_only)
	{
	  if ((*sym)->section == &bfd_und_section)
	    {
	      print_symname ("%s\n", (*sym)->name, abfd);
	    }
	}
      else
	{
	  asymbol *p = *sym;
	  if (p)
	    {
	      bfd_get_symbol_info (abfd, p, &syminfo);
	      (*format->print_symbol_info) (&syminfo, abfd);
	      putchar ('\n');
	    }
	}
    }
}

/* The following 3 groups of functions are called unconditionally,
   once at the start of processing each file of the appropriate type.
   They should check `filename_per_file' and `filename_per_symbol',
   as appropriate for their output format, to determine whether to
   print anything.  */

/* Print the name of an object file given on the command line.  */

static void
print_object_filename_bsd (filename)
     char *filename;
{
  if (filename_per_file && !filename_per_symbol)
    printf ("\n%s:\n", filename);
}

static void
print_object_filename_sysv (filename)
     char *filename;
{
  if (undefined_only)
    printf ("\n\nUndefined symbols from %s:\n\n", filename);
  else
    printf ("\n\nSymbols from %s:\n\n", filename);
  printf ("\
Name                  Value   Class        Type         Size   Line  Section\n\n");
}

static void
print_object_filename_posix (filename)
     char *filename;
{
  if (filename_per_file && !filename_per_symbol)
    printf ("%s:\n", filename);
}

/* Print the name of an archive file given on the command line.  */

static void
print_archive_filename_bsd (filename)
     char *filename;
{
  if (filename_per_file)
    printf ("\n%s:\n", filename);
}

static void
print_archive_filename_sysv (filename)
     char *filename;
{
}

static void
print_archive_filename_posix (filename)
     char *filename;
{
}

/* Print the name of an archive member file.  */

static void
print_archive_member_bsd (archive, filename)
     char *archive;
     CONST char *filename;
{
  if (!filename_per_symbol)
    printf ("\n%s:\n", filename);
}

static void
print_archive_member_sysv (archive, filename)
     char *archive;
     CONST char *filename;
{
  if (undefined_only)
    printf ("\n\nUndefined symbols from %s[%s]:\n\n", archive, filename);
  else
    printf ("\n\nSymbols from %s[%s]:\n\n", archive, filename);
  printf ("\
Name                  Value   Class        Type         Size   Line  Section\n\n");
}

static void
print_archive_member_posix (archive, filename)
     char *archive;
     CONST char *filename;
{
  if (!filename_per_symbol)
    printf ("%s[%s]:\n", archive, filename);
}

/* Print the name of the file (and archive, if there is one)
   containing a symbol.  */

static void
print_symbol_filename_bsd (archive_bfd, abfd)
     bfd *archive_bfd, *abfd;
{
  if (filename_per_symbol)
    {
      if (archive_bfd)
	printf ("%s:", bfd_get_filename (archive_bfd));
      printf ("%s:", bfd_get_filename (abfd));
    }
}

static void
print_symbol_filename_sysv (archive_bfd, abfd)
     bfd *archive_bfd, *abfd;
{
  if (filename_per_symbol)
    {
      if (archive_bfd)
	printf ("%s:", bfd_get_filename (archive_bfd));
      printf ("%s:", bfd_get_filename (abfd));
    }
}

static void
print_symbol_filename_posix (archive_bfd, abfd)
     bfd *archive_bfd, *abfd;
{
  if (filename_per_symbol)
    {
      if (archive_bfd)
	printf ("%s[%s]: ", bfd_get_filename (archive_bfd),
		bfd_get_filename (abfd));
      else
	printf ("%s: ", bfd_get_filename (abfd));
    }
}

/* Print a line of information about a symbol.  */

static void
print_symbol_info_bsd (info, abfd)
     symbol_info *info;
     bfd *abfd;
{
  if (info->type == 'U')
    printf ("        ");
  else
    {
#ifdef BFD_HOST_64_BIT
      printf (value_format, uint64_typeHIGH (info->value),
	      uint64_typeLOW (info->value));
#else
      printf (value_format, info->value);
#endif
    }
  printf (" %c", info->type);
  if (info->type == '-')
    {
      /* A stab.  */
      printf (" ");
      printf (other_format, info->stab_other);
      printf (" ");
      printf (desc_format, info->stab_desc);
      printf (" %5s", info->stab_name);
    }
  print_symname (" %s", info->name, abfd);
}

static void
print_symbol_info_sysv (info, abfd)
     symbol_info *info;
     bfd *abfd;
{
  print_symname ("%-20s|", info->name, abfd);	/* Name */
  if (info->type == 'U')
    printf ("        ");	/* Value */
  else
    {
#ifdef BFD_HOST_64_BIT
      printf (value_format, uint64_typeHIGH (info->value),
	      uint64_typeLOW (info->value));
#else
      printf (value_format, info->value);
#endif
    }
  printf ("|   %c  |", info->type);	/* Class */
  if (info->type == '-')
    {
      /* A stab.  */
      printf ("%18s|  ", info->stab_name);	/* (C) Type */
      printf (desc_format, info->stab_desc);	/* Size */
      printf ("|     |");	/* Line, Section */
    }
  else
    printf ("                  |      |     |");	/* Type, Size, Line, Section */
}

static void
print_symbol_info_posix (info, abfd)
     symbol_info *info;
     bfd *abfd;
{
  print_symname ("%s ", info->name, abfd);
  printf ("%c ", info->type);
  if (info->type == 'U')
    printf ("        ");
  else
    {
#ifdef BFD_HOST_64_BIT
      printf (value_format, uint64_typeHIGH (info->value),
	      uint64_typeLOW (info->value));
#else
      printf (value_format, info->value);
#endif
    }
  /* POSIX.2 wants the symbol size printed here, when applicable;
     BFD currently doesn't provide it, so we take the easy way out by
     considering it to never be applicable.  */
}

static void
print_symdef_entry (abfd)
     bfd *abfd;
{
  symindex idx = BFD_NO_MORE_SYMBOLS;
  carsym *thesym;
  boolean everprinted = false;

  for (idx = bfd_get_next_mapent (abfd, idx, &thesym);
       idx != BFD_NO_MORE_SYMBOLS;
       idx = bfd_get_next_mapent (abfd, idx, &thesym))
    {
      bfd *elt;
      if (!everprinted)
	{
	  printf ("\nArchive index:\n");
	  everprinted = true;
	}
      elt = bfd_get_elt_at_index (abfd, idx);
      if (thesym->name != (char *) NULL)
	{
	  printf ("%s in %s\n", thesym->name, bfd_get_filename (elt));
	}
    }
}
