/*** nm.c -- Describe symbol table of a rel file. */
#include "sysdep.h"
#include "bfd.h"
#include "getopt.h"
#include "stab.gnu.h"
#include <ranlib.h>



PROTO(static boolean, display_file, (char *filename));
PROTO(static boolean, do_one_rel_file, (bfd *file));
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
	{"target", 	    2, NULL,	           NULL},
	{"undefined-only",  0, &undefined_only,    1},
	{0, 0, 0, 0}
};

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
  int c; /* sez which option char */
  int ind = 0; /* used by getopt and ignored by us */
  extern int optind; /* steps thru options */
	
  program_name = *argv;
	
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
			
      break; /* we've been given a long option */
			
    default:
      usage ();
    }
  }
	
  /* Strangely, for the shell you should return only a nonzero value
     on sucess -- the inverse of the C sense. */
  
  /* OK, all options now parsed.  If no filename specified, do a.out. */
  if (optind == argc) return !display_file ("a.out");
  
  /* We were given several filenames to do: */
  while (optind < argc)
    if (!display_file (argv[optind++])) return 1;

  return 0;
}

/** Display a file's stats */

/* goto here is marginally cleaner than the nested if syntax */

static boolean
display_file (filename)
     char *filename;
{
  boolean retval = false;
  bfd *file;
  bfd *arfile = NULL;
	
  file = bfd_openr(filename, target);
  if (file == NULL) {
    bfd_fatal (filename);
  }

  if (bfd_check_format(file, bfd_object)) {
    retval = do_one_rel_file (file);
    goto closer;
  }

  if (!bfd_check_format (file, bfd_archive)) {
    fprintf (stderr, "%s:  %s: unknown format.\n", program_name, filename);
    retval = false;
    goto closer;
  }

  printf("In archive %s:\n", filename);
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
      if (!do_one_rel_file (arfile)) return false;
    }
  }

 closer:
  if (bfd_close(file) == false)
    bfd_fatal (filename);

  return retval;
}


static boolean
do_one_rel_file (abfd)
     bfd *abfd;
{
  unsigned int storage;
  asymbol **syms;
  unsigned int symcount = 0;

  if (!(bfd_get_file_flags (abfd) & HAS_SYMS)) {
    (void) printf ("No symbols in \"%s\".\n", bfd_get_filename (abfd));
    return true;
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
  return true;
}

/* Symbol-sorting predicates */
#define valueof(x)  ((x)->section ? (x)->section->vma + (x)->value : (x)->value)
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
  char *xn = (*(asymbol **) x)->name;
  char *yn = (*(asymbol **) y)->name;

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


/* Return a lower-case character corresponding to the symbol class of sym */
char
decode_symclass (sym)
     asymbol *sym;
{
  flagword flags = sym->flags;
  
  if ((sym->value == 0) && (sym->section != NULL))
    /* Huh?  All section names don't begin with "." */
    return (sym->section->name)[1];

  if (flags & BSF_FORT_COMM) return 'C';
  if (flags & BSF_UNDEFINED) return 'U';
  if (flags & BSF_ABSOLUTE)  return 'a';

 
   if ( (flags & BSF_GLOBAL) || (flags & BSF_LOCAL) ){
     if ( !strcmp(sym->section->name, ".text") ){
       return 't';
     } else if ( !strcmp(sym->section->name, ".data") ){
       return 'd';
     } else if ( !strcmp(sym->section->name, ".bss") ){
       return 'b';
     } else {
       return 'o';
     }
    }

  /* We don't have to handle these cases just yet, but we will soon:
     N_SETV: 'v'; 
     N_SETA: 'l'; 
     N_SETT: 'x';
     N_SETD: 'z';
     N_SETB: 's';
     N_INDR: 'i';
     */
 
  return '?';
}

static void
print_symbols (abfd, syms, symcount)
     bfd *abfd;
     asymbol **syms;
     unsigned long symcount;
{
  asymbol **sym = syms, **end = syms + symcount;
  char class;

  for (; sym < end; ++sym) {
    if (file_on_each_line) printf("%s:", bfd_get_filename(abfd));

    if (undefined_only) {
      if ((*sym)->flags & BSF_UNDEFINED)
	puts ((*sym)->name);
    }
    else {
      asymbol *p = *sym;
      if (p) {
      class = decode_symclass (p);

      if (p->flags & BSF_GLOBAL)
	class = toupper (class);
      
      if (p->value || ((p->flags & BSF_UNDEFINED) !=  BSF_UNDEFINED))
	printf ("%08lx ", (p->section ? p->value + p->section->vma : p->value));
      else fputs ("         ", stdout);

      printf ("%c %s\n", class, p->name);
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
