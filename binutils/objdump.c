/*** objdump.c -- dump information about an object file. */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* 
   $Id$
*/
/*
 * Until there is other documentation, refer to the manual page dump(1) in
 * the system 5 program's reference manual
 */

#include "sysdep.h"
#include "bfd.h"
#include "getopt.h"
#include <stdio.h>
#include <ctype.h>



char *xmalloc();

char *default_target = NULL;	/* default at runtime */

char *program_name = NULL;

int dump_section_contents;	/* -s */
int dump_section_headers;	/* -h */
boolean dump_file_header;	/* -f */
int dump_symtab;		/* -t */
int dump_reloc_info;		/* -r */
int dump_ar_hdrs;		/* -a */
int with_line_numbers;          /* -l */
boolean disassemble;             /* -d */
char *only;

PROTO (void, display_file, (char *filename, char *target));
PROTO (void, dump_data, (bfd *abfd));
PROTO (void, dump_relocs, (bfd *abfd));
PROTO (void, dump_symbols, (bfd *abfd));
PROTO (void, print_arelt_descr, (bfd *abfd, boolean verbose));







char *machine = (char *)NULL;
  asymbol **syms;
  asymbol **syms2;


unsigned int storage;

unsigned int symcount = 0;

void
usage ()
{
  fprintf (stderr,
	   "usage: %s [-ahfdrtxsl] [-m machine] [-j section_name] obj ...\n",
	   program_name);
  exit (1);
}

static struct option long_options[] = 
	{{"syms",   0, &dump_symtab,	      1},
	 {"reloc",  0, &dump_reloc_info,      1},
	 {"header", 0, &dump_section_headers, 1},
	 {0, 0, 0, 0}};



static void
dump_headers(abfd)
bfd *abfd;
{
  asection *section;
  for (section = abfd->sections;
       section != (asection *) NULL;
       section = section->next) 
    {
      char *comma = "";
#define PF(x,y) \
      if (section->flags & x) {  printf("%s%s",comma,y); comma = ", "; }

      printf("SECTION %d [%s]\t: size %08x",
	     section->index,
	     section->name,
(unsigned)     section->size);
      printf(" vma %08lx align 2**%2u\n ",
	     section->vma,
	     section->alignment_power);
      PF(SEC_ALLOC,"ALLOC");
      PF(SEC_LOAD,"LOAD");
      PF(SEC_RELOC,"RELOC");
      PF(SEC_BALIGN,"BALIGN");
      PF(SEC_READONLY,"READONLY");
      PF(SEC_CODE,"CODE");
      PF(SEC_DATA,"DATA");
      PF(SEC_ROM,"ROM");
      printf("\n");
#undef PF
    }
}

static asymbol **
slurp_symtab(abfd)
bfd *abfd;
{
  asymbol **sy;
  if (!(bfd_get_file_flags (abfd) & HAS_SYMS)) {
    (void) printf ("No symbols in \"%s\".\n", bfd_get_filename (abfd));
    return(NULL);
  }

  storage = get_symtab_upper_bound (abfd);
  if (storage) {
    sy = (asymbol **) malloc (storage);
    if (sy == NULL) {
      fprintf (stderr, "%s: out of memory.\n", program_name);
      exit (1);
    }
  }
  symcount = bfd_canonicalize_symtab (abfd, sy);
return sy;
}
/* Sort symbols into value order */
static int comp(ap,bp)
asymbol **ap;
asymbol **bp;
{
  asymbol *a = *ap;
  asymbol *b = *bp;
  int diff;

  if ( a->name== (char *)NULL || (a->flags &( BSF_DEBUGGING| BSF_UNDEFINED) )) 
    a->the_bfd = 0;
  if  ( b->name== (char *)NULL || (b->flags &( BSF_DEBUGGING|BSF_UNDEFINED))) 
    b->the_bfd =0;

  diff = a->the_bfd - b->the_bfd;
  if (diff) {
    return -diff;
  }
  diff = a->value - b->value;
  if (diff) {
    return diff;
  }
  return   a->section - b->section;
}

/* Print the supplied address symbolically if possible */
void
print_address(vma, stream)
bfd_vma vma;
FILE *stream;
{
  /* Perform a binary search looking for the closest symbol to
     the required value */

  unsigned int min = 0;
  unsigned int max = symcount;

  unsigned int thisplace = 1;
  unsigned int oldthisplace ;

  int vardiff;
  if (symcount == 0)
    fprintf(stream,"%08lx", vma);
  else {
    while (true) {
      oldthisplace = thisplace;
      thisplace = (max + min )/2  ;
      if (thisplace == oldthisplace) break;


	vardiff = syms[thisplace]->value - vma;

      if (vardiff) {
	if (vardiff > 0) {
	  max = thisplace;
	}
	else {
	  min = thisplace;
	}
      }
      else {
	/* Totally awesome! the exact right symbol */
	fprintf(stream,"%08lx (%s)", vma, syms[thisplace]->name);
	return;
      }
    }
    /* We've run out of places to look, print the symbol before this one */
    /* see if this or the symbol before describes this location the best */

    if (thisplace != 0) {
      if (syms[thisplace-1]->value - vma >
	  syms[thisplace]->value-vma) {
	/* Previous symbol is in correct section and is closer */
	thisplace --;
      }
    }
    

    if (syms[thisplace]->value > vma) {
      fprintf(stream,"%08lx (%s-%lx)", vma, syms[thisplace]->name,
	      syms[thisplace]->value - vma);

    }
    else {
      fprintf(stream,"%08lx (%s+%lx)", vma, 
	      syms[thisplace]->name,
	      vma - syms[thisplace]->value);
    }
  }
}

void
disassemble_data(abfd)
bfd *abfd;
{
  bfd_byte *data = NULL;
  unsigned int datasize = 0;
  unsigned int i;
  int (*print)() ;
  int print_insn_m68k();
  int print_insn_i960();
  int print_insn_sparc();
  enum bfd_architecture a;
  unsigned long m;
  asection *section;
  /* Replace symbol section relative values with abs values */

  
  for (i = 0; i < symcount; i++) {
    if (syms[i]->section != (asection *)NULL) {
      syms[i]->value += syms[i]->section->vma;
    }
  }

  /* We keep a copy of the symbols in the original order */
  syms2 = slurp_symtab(abfd);

  /* Sort the symbols into section and symbol order */
  (void)   qsort(syms, symcount, sizeof(asymbol *), comp);

  /* Find the first useless symbol */
  { unsigned int i;
    for (i =0; i < symcount; i++) {
      if (syms[i]->the_bfd == 0) {
	symcount =i;
	break;
      }
    }
  }


  if (machine!= (char *)NULL) {
    if (bfd_scan_arch_mach(machine, &a, &m) == false) {
      fprintf(stderr,"%s: Can't use supplied machine %s\n",
	      program_name,
	      machine);
      exit(1);
    }
  }
  else {
    a =   bfd_get_architecture(abfd);
  }
  switch (a) {

  case bfd_arch_sparc:
    print = print_insn_sparc;
    break;
  case bfd_arch_m68k:
    print = print_insn_m68k;
    break;
  case bfd_arch_i960:
    print = print_insn_i960;
    break;
  default:
    fprintf(stderr,"%s: Can't disassemble for architecture %s\n",
	    program_name,
	    bfd_printable_arch_mach(bfd_get_architecture(abfd),0));
    exit(1);
  }


  for (section = abfd->sections;
       section != (asection *)NULL;
       section =  section->next) {

    if (only == (char *)NULL || strcmp(only,section->name) == 0){
      printf("Disassembly of section %s:\n", section->name);

      if (section->size == 0) continue;

      data = (bfd_byte *)malloc(section->size);

      if (data == (bfd_byte *)NULL) {
	fprintf (stderr, "%s: memory exhausted.\n", program_name);
	exit (1);
      }
      datasize = section->size;


      bfd_get_section_contents (abfd, section, data, 0, section->size);

      i = 0;
      while ((size_t)i <section->size) {
	if (with_line_numbers) {
	  static prevline;
	  char *filename;
	  char *functionname;
	  int line;
	  bfd_find_nearest_line(abfd,
				section,
				syms,
				section->vma + i,
				&filename,
				&functionname,
				&line);

	  if (filename && functionname && line && line != prevline) {
	    printf("%s:%d\n", filename, line);
	    prevline = line;
	  }
	}
	print_address(section->vma + i, stdout);
	printf(" ");

	i +=   print(section->vma + i, 
		     data + i,
		     stdout);
	putchar ('\n')  ;  
      }




      free(data);
    }
  }
}

void
display_bfd (abfd)
     bfd *abfd;
{

  if (!bfd_check_format (abfd, bfd_object)) {
    fprintf (stderr,"%s: %s not an object file\n", program_name,
	     abfd->filename);
    return;
  }
  printf ("\n%s:     file format %s\n", abfd->filename, abfd->xvec->name);
  if (dump_ar_hdrs) print_arelt_descr (abfd, true);

  if (dump_file_header) {
    char *comma = "";

    printf("architecture: %s, ",
	   bfd_printable_arch_mach (bfd_get_architecture (abfd),
				    bfd_get_machine (abfd)));
    printf("flags 0x%08x:\n", abfd->flags);
    
#define PF(x, y)    if (abfd->flags & x) {printf("%s%s", comma, y); comma=", ";}
    PF(HAS_RELOC, "HAS_RELOC");
    PF(EXEC_P, "EXEC_P");
    PF(HAS_LINENO, "HAS_LINENO");
    PF(HAS_DEBUG, "HAS_DEBUG");
    PF(HAS_SYMS, "HAS_SYMS");
    PF(HAS_LOCALS, "HAS_LOCALS");
    PF(DYNAMIC, "DYNAMIC");
    PF(WP_TEXT, "WP_TEXT");
    PF(D_PAGED, "D_PAGED");
    printf("\nstart address 0x%08lx", abfd->start_address);
  }
  printf("\n");

  if (dump_section_headers)
    dump_headers(abfd);
  if (dump_symtab || dump_reloc_info || disassemble) {
syms =  slurp_symtab(abfd);
  }
  if (dump_symtab) dump_symbols (abfd);
  if (dump_reloc_info) dump_relocs(abfd);
  if (dump_section_contents) dump_data (abfd);
  if (disassemble) disassemble_data(abfd);
}

void
display_file (filename, target)
     char *filename;
     char *target;
{
  bfd *file, *arfile = (bfd *) NULL;

  file = bfd_openr (filename, target);
  if (file == NULL) {
    bfd_perror (filename);
    return;
  }

  if (bfd_check_format (file, bfd_archive) == true) {
    printf ("In archive %s:\n", bfd_get_filename (file));
    for(;;) {
      bfd_error = no_error;

      arfile = bfd_openr_next_archived_file (file, arfile);
      if (arfile == NULL) {
	if (bfd_error != no_more_archived_files)
	  bfd_perror (bfd_get_filename(file));
	return;
      }

      display_bfd (arfile);
      /* Don't close the archive elements; we need them for next_archive */
    }
  }
  else
    display_bfd(file);

  bfd_close(file);
}

/* Actually display the various requested regions */










void
dump_data (abfd)
     bfd *abfd;
{
  asection *section;
  bfd_byte  *data ;
  unsigned int datasize = 0;
  size_t i;

  for (section = abfd->sections; section != NULL; section =
       section->next) {
    int onaline = 16;

    if (only == (char *)NULL || 
	strcmp(only,section->name) == 0){



      printf("Contents of section %s:\n", section->name);

      if (section->size == 0) continue;
      data = (bfd_byte *)malloc(section->size);
      if (data == (bfd_byte *)NULL) {
	fprintf (stderr, "%s: memory exhausted.\n", program_name);
	exit (1);
      }
      datasize = section->size;


      bfd_get_section_contents (abfd, section, data, 0, section->size);

      for (i= 0; i < section->size; i += onaline) {
	size_t j;
	printf(" %04lx ", i + section->vma);
	for (j = i; j < i+ onaline; j++) {
	  if (j < section->size)
	    printf("%02x", (unsigned)(data[j]));
	  else 
	    printf("  ");
	  if ((j & 3 ) == 3) printf(" ");
	}

	printf(" ");
	for (j = i; j < i+onaline ; j++) {
	  if (j >= section->size)
	    printf(" ");
	  else
	    printf("%c", isprint(data[j]) ?data[j] : '.');
	}
	putchar ('\n');
      }
    }

    free (data);
  }
}



/* Should perhaps share code and display with nm? */
void
dump_symbols (abfd)
     bfd *abfd;
{

  unsigned int count;
  asymbol **current = syms;
  printf("SYMBOL TABLE:\n");

  for (count = 0; count < symcount; count++) {
    if ((*current)->the_bfd) {
      bfd_print_symbol((*current)->the_bfd,
		       stdout,
		       *current, bfd_print_symbol_all_enum);

      printf("\n");
    }
    current++;
  }
  printf("\n");
  printf("\n");
}


void
dump_relocs(abfd)
bfd *abfd;
{
  arelent **relpp;
  unsigned int relcount;
  asection *a;
  for (a = abfd->sections; a != (asection *)NULL; a = a->next) {
    printf("RELOCATION RECORDS FOR [%s]:",a->name);
    
    if (get_reloc_upper_bound(abfd, a) == 0) {
      printf(" (none)\n\n");
    }
    else {
      arelent **p;

      relpp = (arelent **) xmalloc( get_reloc_upper_bound(abfd,a) );
      relcount = bfd_canonicalize_reloc(abfd,a,relpp, syms);
      if (relcount == 0) {
	printf(" (none)\n\n");
      }
      else {
	printf("\n");
	printf("OFFSET   TYPE      VALUE \n");

	for (p =relpp; *p != (arelent *)NULL; p++) {
	  arelent *q = *p;
	  CONST char *sym_name;
	  CONST char *section_name =	    q->section == (asection *)NULL ? "*abs" :
	  q->section->name;
	  if (q->sym_ptr_ptr && *q->sym_ptr_ptr) {
	    sym_name =  (*(q->sym_ptr_ptr))->name ;
	  }
	  else {
	    sym_name = 0;
	  }
	  if (sym_name) {
	    printf("%08lx %-8s  %s",
		   q->address,
		   q->howto->name,
		   sym_name);
	  }
	  else {
	    printf("%08lx %-8s  [%s]",
		   q->address,
		   q->howto->name,
		   section_name);
	  }
	  if (q->addend) {
	    printf("+0x%lx(%ld)", q->addend, (long) q->addend);
	  }
	  printf("\n");
	}
	printf("\n\n");
	free(relpp);
      }
    }

  }
}


/** main and like trivia */
int
main (argc, argv)
     int argc;
     char **argv;
{
  int c;
  extern int optind;
  extern char *optarg;
  char *target = default_target;
  boolean seenflag = false;
  int ind = 0;

  program_name = *argv;

  while ((c = getopt_long (argc, argv, "b:m:dlfahrtxsj:", long_options, &ind))
	 != EOF) {
    seenflag = true;
    switch (c) {
    case 'm':
      machine = optarg;
      break;
    case 'j':
      only = optarg;
      break;
    case 'l':
      with_line_numbers = 1;
      break;
    case 'b':
      target = optarg;
      break;
    case 'f':
      dump_file_header = true;
      break;
    case 'x':
      dump_symtab = 1; 
      dump_reloc_info = 1;
      dump_file_header = true;
      dump_ar_hdrs = 1;
      dump_section_headers = 1;
      break;
    case  0 : break;		/* we've been given a long option */
    case 't': dump_symtab = 1; break;
    case 'd': disassemble = true ; break;
    case 's': dump_section_contents = 1; break;
    case 'r': dump_reloc_info = 1; break;
    case 'a': dump_ar_hdrs = 1; break;
    case 'h': dump_section_headers = 1; break;
    default:
      usage ();
    }
  }

  if (seenflag == false)
    usage ();

  if (optind == argc)
    display_file ("a.out", target);
  else
    for (; optind < argc;)
      display_file (argv[optind++], target);
  return 0;
}
