/* objdump.c -- dump information about an object file.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

This file is part of GNU Binutils.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "getopt.h"
#include "bucomm.h"
#include <stdio.h>
#include <ctype.h>
#include "dis-asm.h"

/* Internal headers for the ELF .stab-dump code - sorry.  */
#define	BYTES_IN_WORD	32
#include "aout/aout64.h"
#include "elf/internal.h"
extern Elf_Internal_Shdr *bfd_elf_find_section();

#ifndef FPRINTF_ALREADY_DECLARED
extern int fprintf PARAMS ((FILE *, CONST char *, ...));
#endif

char *default_target = NULL;	/* default at runtime */

extern char *program_version;

int show_version = 0;		/* show the version number */
int dump_section_contents;	/* -s */
int dump_section_headers;	/* -h */
boolean dump_file_header;	/* -f */
int dump_symtab;		/* -t */
int dump_reloc_info;		/* -r */
int dump_ar_hdrs;		/* -a */
int with_line_numbers;		/* -l */
int dump_stab_section_info;	/* --stabs */
boolean disassemble;		/* -d */
boolean formats_info;		/* -i */
char *only;			/* -j secname */

struct objdump_disasm_info {
  bfd *abfd;
  asection *sec;
};

char *machine = (char *) NULL;
asymbol **syms;

unsigned int storage;

unsigned int symcount = 0;

/* Forward declarations.  */

static void
display_file PARAMS ((char *filename, char *target));

static void
dump_data PARAMS ((bfd *abfd));

static void
dump_relocs PARAMS ((bfd *abfd));

static void
dump_symbols PARAMS ((bfd *abfd));

static void
display_bfd PARAMS ((bfd *abfd));

void
usage (stream, status)
     FILE *stream;
     int status;
{
  fprintf (stream, "\
Usage: %s [-ahifdrtxsl] [-b bfdname] [-m machine] [-j section-name]\n\
       [--archive-headers] [--target=bfdname] [--disassemble] [--file-headers]\n\
       [--section-headers] [--headers] [--info] [--section=section-name]\n\
       [--line-numbers] [--architecture=machine] [--reloc] [--full-contents]\n\
       [--stabs] [--syms] [--all-headers] [--version] [--help] objfile...\n\
at least one option besides -l (--line-numbers) must be given\n",
	   program_name);
  exit (status);
}

static struct option long_options[]=
{
  {"all-headers", no_argument, NULL, 'x'},
  {"architecture", required_argument, NULL, 'm'},
  {"archive-headers", no_argument, NULL, 'a'},
  {"disassemble", no_argument, NULL, 'd'},
  {"file-headers", no_argument, NULL, 'f'},
  {"full-contents", no_argument, NULL, 's'},
  {"headers", no_argument, NULL, 'h'},
  {"help", no_argument, NULL, 'H'},
  {"info", no_argument, NULL, 'i'},
  {"line-numbers", no_argument, NULL, 'l'},
  {"reloc", no_argument, NULL, 'r'},
  {"section", required_argument, NULL, 'j'},
  {"section-headers", no_argument, NULL, 'h'},
  {"stabs", no_argument, &dump_stab_section_info, 1},
  {"syms", no_argument, NULL, 't'},
  {"target", required_argument, NULL, 'b'},
  {"version", no_argument, &show_version,    1},
  {0, no_argument, 0, 0}
};


static void
dump_headers (abfd)
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


      printf ("SECTION %d [%s]\t: size %08x",
	      section->index,
	      section->name,
	      (unsigned) bfd_get_section_size_before_reloc (section));
      printf (" vma ");
      printf_vma (section->vma);
      printf (" align 2**%u\n ",
	      section->alignment_power);
      PF (SEC_ALLOC, "ALLOC");
      PF (SEC_CONSTRUCTOR, "CONSTRUCTOR");
      PF (SEC_CONSTRUCTOR_TEXT, "CONSTRUCTOR TEXT");
      PF (SEC_CONSTRUCTOR_DATA, "CONSTRUCTOR DATA");
      PF (SEC_CONSTRUCTOR_BSS, "CONSTRUCTOR BSS");
      PF (SEC_LOAD, "LOAD");
      PF (SEC_RELOC, "RELOC");
#ifdef SEC_BALIGN
      PF (SEC_BALIGN, "BALIGN");
#endif
      PF (SEC_READONLY, "READONLY");
      PF (SEC_CODE, "CODE");
      PF (SEC_DATA, "DATA");
      PF (SEC_ROM, "ROM");
      PF (SEC_DEBUGGING, "DEBUGGING");
      printf ("\n");
#undef PF
    }
}

static asymbol **
DEFUN (slurp_symtab, (abfd),
       bfd * abfd)
{
  asymbol **sy = (asymbol **) NULL;

  if (!(bfd_get_file_flags (abfd) & HAS_SYMS))
    {
      (void) printf ("No symbols in \"%s\".\n", bfd_get_filename (abfd));
      return (NULL);
    }

  storage = get_symtab_upper_bound (abfd);
  if (storage)
    {
      sy = (asymbol **) xmalloc (storage);
    }
  symcount = bfd_canonicalize_symtab (abfd, sy);
  if (symcount <= 0)
    {
      fprintf (stderr, "%s: Bad symbol table in \"%s\".\n",
	       program_name, bfd_get_filename (abfd));
      exit (1);
    }
  return sy;
}

/* Filter out (in place) symbols that are useless for dis-assemble.
   Return count of useful symbols. */

int remove_useless_symbols (syms, count)
     asymbol **syms;
     int count;
{
  register asymbol **in_ptr = syms;
  register asymbol **out_ptr = syms;

  while ( --count >= 0 )
    {
      asymbol *sym = *in_ptr++;

      if (sym->name == NULL || sym->name[0] == '\0')
	continue;
      if (sym->flags & (BSF_DEBUGGING))
	continue;
      if (sym->section == &bfd_und_section
	  || bfd_is_com_section (sym->section))
	continue;

      *out_ptr++ = sym;
    }
  return out_ptr - syms;
}


/* Sort symbols into value order */
static int 
comp (ap, bp)
     PTR ap;
     PTR bp;
{
  asymbol *a = *(asymbol **)ap;
  asymbol *b = *(asymbol **)bp;

  if (a->value > b->value)
    return 1;
  else if (a->value < b->value)
    return -1;

  if (a->section > b->section)
    return 1;
  else if (a->section < b->section)
    return -1;
  return 0;
}

/* Print the supplied address symbolically if possible */
void
objdump_print_address (vma, info)
     bfd_vma vma;
     struct disassemble_info *info;
{
  /* Perform a binary search looking for the closest symbol to
     the required value.  */
  /* @@ For relocateable files, should filter out symbols belonging to
     the wrong section.  Unfortunately, not enough information is supplied
     to this routine to determine the correct section in all cases.  */
  /* @@ Would it speed things up to cache the last two symbols returned,
     and maybe their address ranges?  For many processors, only one memory
     operand can be present at a time, so the 2-entry cache wouldn't be
     constantly churned by code doing heavy memory accesses.  */

  unsigned int min = 0;
  unsigned int max = symcount;

  unsigned int thisplace = 1;
  unsigned int oldthisplace;

  int vardiff;

  fprintf_vma (info->stream, vma);

  if (symcount > 0)
    {
      while (true)
	{
	  asymbol *sym; asection *sym_sec;
	  oldthisplace = thisplace;
	  thisplace = (max + min) / 2;
	  if (thisplace == oldthisplace)
	    break;
	  sym = syms[thisplace];
	  vardiff = sym->value - vma;
	  sym_sec = sym->section;

	  if (vardiff > 0)
	    max = thisplace;
	  else if (vardiff < 0)
	    min = thisplace;
	  else
	    goto found;
	}
      /* We've run out of places to look, print the symbol before this one
         see if this or the symbol before describes this location the best */

      if (thisplace != 0)
	{
	  if (syms[thisplace - 1]->value - vma >
	      syms[thisplace]->value - vma)
	    {
	      /* Previous symbol is in correct section and is closer */
	      thisplace--;
	    }
	}

    found:
      {
	bfd_vma val = syms[thisplace]->value;
	int i;
	if (syms[thisplace]->flags & (BSF_LOCAL|BSF_DEBUGGING))
	  for (i = thisplace - 1; i >= 0; i--)
	    {
	      if (syms[i]->value == val
		  && (!(syms[i]->flags & (BSF_LOCAL|BSF_DEBUGGING))
		      || ((syms[thisplace]->flags & BSF_DEBUGGING)
			  && !(syms[i]->flags & BSF_DEBUGGING))))
		{
		  thisplace = i;
		  break;
		}
	    }
	if (syms[thisplace]->flags & (BSF_LOCAL|BSF_DEBUGGING))
	  for (i = thisplace + 1; i < symcount; i++)
	    {
	      if (syms[i]->value == val
		  && (!(syms[i]->flags & (BSF_LOCAL|BSF_DEBUGGING))
		      || ((syms[thisplace]->flags & BSF_DEBUGGING)
			  && !(syms[i]->flags & BSF_DEBUGGING))))
		{
		  thisplace = i;
		  break;
		}
	    }
      }
      {
	/* If the file is relocateable, and the symbol could be from this
	   section, prefer a symbol from this section over symbols from
	   others, even if the other symbol's value might be closer.

	   Note that this may be wrong for some symbol references if the
	   sections have overlapping memory ranges, but in that case there's
	   no way to tell what's desired without looking at the relocation
	   table.  */
	struct objdump_disasm_info *aux;
	int i;

	aux = (struct objdump_disasm_info *) info->application_data;
	if (aux->abfd->flags & HAS_RELOC
	    && vma >= bfd_get_section_vma (aux->abfd, aux->sec)
	    && vma < (bfd_get_section_vma (aux->abfd, aux->sec)
		      + bfd_get_section_size_before_reloc (aux->sec))
	    && syms[thisplace]->section != aux->sec)
	  {
	    for (i = thisplace + 1; i < symcount; i++)
	      if (syms[i]->value != syms[thisplace]->value)
		break;
	    while (--i >= 0)
	      if (syms[i]->section == aux->sec)
		{
		  thisplace = i;
		  break;
		}
	  }
      }
      fprintf (info->stream, " <%s", syms[thisplace]->name);
      if (syms[thisplace]->value > vma)
	{
	  char buf[30], *p = buf;
	  sprintf_vma (buf, syms[thisplace]->value - vma);
	  while (*p == '0')
	    p++;
	  fprintf (info->stream, "-%s", p);
	}
      else if (vma > syms[thisplace]->value)
	{
	  char buf[30], *p = buf;
	  sprintf_vma (buf, vma - syms[thisplace]->value);
	  while (*p == '0')
	    p++;
	  fprintf (info->stream, "+%s", p);
	}
      fprintf (info->stream, ">");
    }
}

#ifdef ARCH_all
#define ARCH_a29k
#define ARCH_alpha
#define ARCH_h8300
#define ARCH_h8500
#define ARCH_hppa
#define ARCH_i386
#define ARCH_i960
#define ARCH_m68k
#define ARCH_m88k
#define ARCH_mips
#define ARCH_rs6000
#define ARCH_sh
#define ARCH_sparc
#define ARCH_z8k
#endif

void
disassemble_data (abfd)
     bfd *abfd;
{
  bfd_byte *data = NULL;
  bfd_arch_info_type *info;
  bfd_size_type datasize = 0;
  bfd_size_type i;
  unsigned int (*print) ()= 0; /* Old style */
  disassembler_ftype disassemble = 0; /* New style */
  enum bfd_architecture a;
  struct disassemble_info disasm_info;
  struct objdump_disasm_info aux;

  int prevline;
  CONST char *prev_function = "";

  asection *section;

  /* Replace symbol section relative values with abs values */
  boolean done_dot = false;

  INIT_DISASSEMBLE_INFO(disasm_info, stdout);
  disasm_info.application_data = (PTR) &aux;
  aux.abfd = abfd;
  disasm_info.print_address_func = objdump_print_address;

  for (i = 0; i < symcount; i++)
    {
      syms[i]->value += syms[i]->section->vma;
    }

  symcount = remove_useless_symbols (syms, symcount);

  /* Sort the symbols into section and symbol order */
  (void) qsort (syms, symcount, sizeof (asymbol *), comp);

  if (machine != (char *) NULL)
    {
      info = bfd_scan_arch (machine);
      if (info == 0)
	{
	  fprintf (stderr, "%s: Can't use supplied machine %s\n",
		   program_name,
		   machine);
	  exit (1);
	}
      abfd->arch_info = info;
    }

  /* See if we can disassemble using bfd */

  if (abfd->arch_info->disassemble)
    {
      print = abfd->arch_info->disassemble;
    }
  else
    {
      a = bfd_get_arch (abfd);
      switch (a)
	{
	  /* If you add a case to this table, also add it to the
	     ARCH_all definition right above this function.  */
#ifdef ARCH_a29k
	case bfd_arch_a29k:
	  /* As far as I know we only handle big-endian 29k objects.  */
	  disassemble = print_insn_big_a29k;
	  break;
#endif
#ifdef ARCH_alpha
	case bfd_arch_alpha:
	  disassemble = print_insn_alpha;
	  break;
#endif
#ifdef ARCH_h8300
	case bfd_arch_h8300:
	  if (bfd_get_mach(abfd) == bfd_mach_h8300h)
	   disassemble = print_insn_h8300h;
	  else 
	   disassemble = print_insn_h8300;
	  break;
#endif
#ifdef ARCH_h8500
	case bfd_arch_h8500:
	  disassemble = print_insn_h8500;
	  break;
#endif
#ifdef ARCH_hppa
	case bfd_arch_hppa:
	  disassemble = print_insn_hppa;
	  break;
#endif
#ifdef ARCH_i386
	case bfd_arch_i386:
	  disassemble = print_insn_i386;
	  break;
#endif
#ifdef ARCH_i960
	case bfd_arch_i960:
	  disassemble = print_insn_i960;
	  break;
#endif
#ifdef ARCH_m68k
	case bfd_arch_m68k:
	  disassemble = print_insn_m68k;
	  break;
#endif
#ifdef ARCH_m88k
	case bfd_arch_m88k:
	  disassemble = print_insn_m88k;
	  break;
#endif
#ifdef ARCH_mips
	case bfd_arch_mips:
	  if (abfd->xvec->byteorder_big_p)
	    disassemble = print_insn_big_mips;
	  else
	    disassemble = print_insn_little_mips;
	  break;
#endif
#ifdef ARCH_rs6000:
	case bfd_arch_rs6000:
	  disassemble = print_insn_rs6000;
	  break;
#endif
#ifdef ARCH_sh
	case bfd_arch_sh:
	  disassemble = print_insn_sh;
	  break;
#endif
#ifdef ARCH_sparc
	case bfd_arch_sparc:
	  disassemble = print_insn_sparc;
	  break;
#endif
#ifdef ARCH_z8k
        case bfd_arch_z8k:
	  if (bfd_get_mach(abfd) == bfd_mach_z8001)
	   disassemble = print_insn_z8001;
	  else 
	   disassemble = print_insn_z8002;
	  break;
#endif
	default:
	  fprintf (stderr, "%s: Can't disassemble for architecture %s\n",
		   program_name,
		   bfd_printable_arch_mach (bfd_get_arch (abfd), 0));
	  exit (1);
	}

    }

  for (section = abfd->sections;
       section != (asection *) NULL;
       section = section->next)
    {
      aux.sec = section;

      if ((section->flags & SEC_LOAD)
	  && (only == (char *) NULL || strcmp (only, section->name) == 0))
	{
	  printf ("Disassembly of section %s:\n", section->name);

	  if (bfd_get_section_size_before_reloc (section) == 0)
	    continue;

	  data = (bfd_byte *) xmalloc ((size_t) bfd_get_section_size_before_reloc (section));

	  datasize = bfd_get_section_size_before_reloc (section);

	  bfd_get_section_contents (abfd, section, data, 0, bfd_get_section_size_before_reloc (section));

	  disasm_info.buffer = data;
	  disasm_info.buffer_vma = section->vma;
	  disasm_info.buffer_length =
	    bfd_get_section_size_before_reloc (section);
	  i = 0;
	  while (i < disasm_info.buffer_length)
	    {
	      if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 0 &&
		  data[i + 3] == 0)
		{
		  if (done_dot == false)
		    {
		      printf ("...\n");
		      done_dot = true;
		    }
		  i += 4;
		}
	      else
		{
		  done_dot = false;
		  if (with_line_numbers)
		    {
		      CONST char *filename;
		      CONST char *functionname;
		      unsigned int line;

		      if (bfd_find_nearest_line (abfd,
						 section,
						 syms,
						 section->vma + i,
						 &filename,
						 &functionname,
						 &line))
			{
			  if (functionname && *functionname
			      && strcmp(functionname, prev_function))
			    {
			      printf ("%s():\n", functionname);
			      prev_function = functionname;
			    }
			  if (!filename)
			    filename = "???";
			  if (line && line != prevline)
			    {
			      printf ("%s:%u\n", filename, line);
			      prevline = line;
			    }
			}
		    }
		  objdump_print_address (section->vma + i, &disasm_info);
		  printf (" ");

		  if (disassemble) /* New style */
		    {
		      int bytes = (*disassemble)(section->vma + i,
						 &disasm_info);
		      if (bytes < 0)
			break;
		      i += bytes;
		    }
		  else /* Old style */
		    i += print (section->vma + i,
				data + i,
				stdout);
		  putchar ('\n');
		}
	    }
	  free (data);
	}
    }
}


/* Define a table of stab values and print-strings.  We wish the initializer
   could be a direct-mapped table, but instead we build one the first
   time we need it.  */

char **stab_name;

struct stab_print {
  int value;
  char *string;
};

struct stab_print stab_print[] = {
#define __define_stab(NAME, CODE, STRING) {CODE, STRING},
#include "aout/stab.def"
#undef __define_stab
  {0, ""}
};

void dump_stabs_1 ();

/* This dumps the stabs section from object files that have a section that
   uses Sun stabs encoding.  It has to use some hooks into BFD because
   string table sections are not normally visible to BFD callers.  */

void
dump_stabs (abfd)
     bfd *abfd;
{
  int i;

  /* Allocate and initialize stab name array if first time.  */
  if (stab_name == NULL) 
    {
      stab_name = (char **) xmalloc (256 * sizeof(char *));
      /* Clear the array. */
      for (i = 0; i < 256; i++)
	stab_name[i] = NULL;
      /* Fill in the defined stabs. */
      for (i = 0; *stab_print[i].string; i++)
	stab_name[stab_print[i].value] = stab_print[i].string;
    }

  dump_stabs_1 (abfd, ".stab", ".stabstr");
  dump_stabs_1 (abfd, ".stab.excl", ".stab.exclstr");
  dump_stabs_1 (abfd, ".stab.index", ".stab.indexstr");
  dump_stabs_1 (abfd, "$GDB_SYMBOLS$", "$GDB_STRINGS$");
}

void
dump_stabs_1 (abfd, name1, name2)
     bfd *abfd;
     char *name1;		/* Section name of .stab */
     char *name2;		/* Section name of its string section */
{
  Elf_Internal_Shdr *stab_hdr, *stabstr_hdr;
  asection *stabsect, *stabstrsect;
  char *strtab;
  struct internal_nlist *stabs, *stabs_end;
  int i;
  int stab_size, stabstr_size;
  unsigned file_string_table_offset, next_file_string_table_offset;
  int is_elf = (0 == strncmp ("elf", abfd->xvec->name, 3));

  if (is_elf)
    {
      stab_hdr = bfd_elf_find_section (abfd, name1);
    }
  else
    {
      stabsect = bfd_get_section_by_name (abfd, name1);
    }

  if (is_elf ? (0 == stab_hdr) : (0 == stabsect))
    {
      printf ("No %s section present.\n\n", name1);
      return;
    }

  if (is_elf)
    {
      stabstr_hdr = bfd_elf_find_section (abfd, name2);
    }
  else
    {
      stabstrsect = bfd_get_section_by_name (abfd, name2);
    }

  if (is_elf ? (0 == stabstr_hdr) : (0 == stabstrsect))
    {
      fprintf (stderr, "%s: %s has no %s section.\n", program_name,
	       abfd->filename, name2);
      return;
    }
 
  stab_size    = (is_elf ? stab_hdr   ->sh_size : bfd_section_size (abfd, stabsect));
  stabstr_size = (is_elf ? stabstr_hdr->sh_size : bfd_section_size (abfd, stabstrsect));

  stabs  = (struct internal_nlist *) xmalloc (stab_size);
  strtab = (char *) xmalloc (stabstr_size);
  stabs_end = (struct internal_nlist *) (stab_size + (char *) stabs);
  
  if (is_elf) 
    {
      if (bfd_seek (abfd, stab_hdr->sh_offset, SEEK_SET) < 0 ||
	  stab_size != bfd_read ((PTR) stabs, stab_size, 1, abfd))
	{
	  fprintf (stderr, "%s: reading %s section of %s failed.\n",
		   program_name, name1, 
		   abfd->filename);
	  return;
	}
    }
  else
    {
      bfd_get_section_contents (abfd, stabsect, (PTR) stabs, 0, stab_size);
    }

  if (is_elf) 
    {
      if (bfd_seek (abfd, stabstr_hdr->sh_offset, SEEK_SET) < 0 ||
	  stabstr_size != bfd_read ((PTR) strtab, stabstr_size, 1, abfd))
	{
	  fprintf (stderr, "%s: reading %s section of %s failed.\n",
		   program_name, name2,
		   abfd->filename);
	  return;
	}
    }
  else
    {
      bfd_get_section_contents (abfd, stabstrsect, (PTR) strtab, 0, stabstr_size);
    }

#define SWAP_SYMBOL(symp, abfd) \
  { \
    (symp)->n_strx = bfd_h_get_32(abfd,			\
				(unsigned char *)&(symp)->n_strx);	\
    (symp)->n_desc = bfd_h_get_16 (abfd,			\
				(unsigned char *)&(symp)->n_desc);  	\
    (symp)->n_value = bfd_h_get_32 (abfd,			\
				(unsigned char *)&(symp)->n_value); 	\
  }

  printf ("Contents of %s section:\n\n", name1);
  printf ("Symnum n_type n_othr n_desc n_value  n_strx String\n");

  file_string_table_offset = 0;
  next_file_string_table_offset = 0;

  /* Loop through all symbols and print them.

     We start the index at -1 because there is a dummy symbol on
     the front of stabs-in-{coff,elf} sections that supplies sizes. */

  for (i = -1; stabs < stabs_end; stabs++, i++)
    {
      SWAP_SYMBOL (stabs, abfd);
      printf ("\n%-6d ", i);
      /* Print either the stab name, or, if unnamed, print its number
	 again (makes consistent formatting for tools like awk). */
      if (stab_name[stabs->n_type])
	printf ("%-6s", stab_name[stabs->n_type]);
      else
	printf ("%-6d", i);
      printf (" %-6d %-6d ", stabs->n_other, stabs->n_desc);
      printf_vma (stabs->n_value);
      printf (" %-6lu", stabs->n_strx);

      /* Symbols with type == 0 (N_UNDF) specify the length of the
	 string table associated with this file.  We use that info
	 to know how to relocate the *next* file's string table indices.  */

      if (stabs->n_type == N_UNDF)
	{
	  file_string_table_offset = next_file_string_table_offset;
	  next_file_string_table_offset += stabs->n_value;
	}
      else
	{
	  /* Now, using the possibly updated string table offset, print the
	     string (if any) associated with this symbol.  */

	  if ((stabs->n_strx + file_string_table_offset) < stabstr_size)
	    printf (" %s", &strtab[stabs->n_strx + file_string_table_offset]);
	  else
	    printf (" *");
	}
    }
  printf ("\n\n");
}

static void
display_bfd (abfd)
     bfd *abfd;
{

  if (!bfd_check_format (abfd, bfd_object))
    {
      fprintf (stderr, "%s:%s: %s\n", program_name, abfd->filename,
	       bfd_errmsg (bfd_error));
      return;
    }
  printf ("\n%s:     file format %s\n", abfd->filename, abfd->xvec->name);
  if (dump_ar_hdrs)
    print_arelt_descr (stdout, abfd, true);

  if (dump_file_header)
    {
      char *comma = "";

      printf ("architecture: %s, ",
	      bfd_printable_arch_mach (bfd_get_arch (abfd),
				       bfd_get_mach (abfd)));
      printf ("flags 0x%08x:\n", abfd->flags);

#define PF(x, y)    if (abfd->flags & x) {printf("%s%s", comma, y); comma=", ";}
      PF (HAS_RELOC, "HAS_RELOC");
      PF (EXEC_P, "EXEC_P");
      PF (HAS_LINENO, "HAS_LINENO");
      PF (HAS_DEBUG, "HAS_DEBUG");
      PF (HAS_SYMS, "HAS_SYMS");
      PF (HAS_LOCALS, "HAS_LOCALS");
      PF (DYNAMIC, "DYNAMIC");
      PF (WP_TEXT, "WP_TEXT");
      PF (D_PAGED, "D_PAGED");
      PF (BFD_IS_RELAXABLE, "BFD_IS_RELAXABLE");
      printf ("\nstart address 0x");
      printf_vma (abfd->start_address);
    }
  printf ("\n");

  if (dump_section_headers)
    dump_headers (abfd);
  if (dump_symtab || dump_reloc_info || disassemble)
    {
      syms = slurp_symtab (abfd);
    }
  if (dump_symtab)
    dump_symbols (abfd);
  if (dump_stab_section_info)
    dump_stabs (abfd);
  if (dump_reloc_info)
    dump_relocs (abfd);
  if (dump_section_contents)
    dump_data (abfd);
  /* Note that disassemble_data re-orders the syms table, but that is
     safe - as long as it is done last! */
  if (disassemble)
    disassemble_data (abfd);
}

static void
display_file (filename, target)
     char *filename;
     char *target;
{
  bfd *file, *arfile = (bfd *) NULL;

  file = bfd_openr (filename, target);
  if (file == NULL)
    {
      fprintf (stderr, "%s: ", program_name);
      bfd_perror (filename);
      return;
    }

  if (bfd_check_format (file, bfd_archive) == true)
    {
      printf ("In archive %s:\n", bfd_get_filename (file));
      for (;;)
	{
	  bfd_error = no_error;

	  arfile = bfd_openr_next_archived_file (file, arfile);
	  if (arfile == NULL)
	    {
	      if (bfd_error != no_more_archived_files)
		{
		  fprintf (stderr, "%s: ", program_name);
		  bfd_perror (bfd_get_filename (file));
		}
	      return;
	    }

	  display_bfd (arfile);
	  /* Don't close the archive elements; we need them for next_archive */
	}
    }
  else
    display_bfd (file);

  bfd_close (file);
}

/* Actually display the various requested regions */

static void
dump_data (abfd)
     bfd *abfd;
{
  asection *section;
  bfd_byte *data = 0;
  bfd_size_type datasize = 0;
  bfd_size_type i;

  for (section = abfd->sections; section != NULL; section =
       section->next)
    {
      int onaline = 16;

      if (only == (char *) NULL ||
	  strcmp (only, section->name) == 0)
	{
	  if (section->flags & SEC_HAS_CONTENTS)
	    {
	      printf ("Contents of section %s:\n", section->name);

	      if (bfd_section_size (abfd, section) == 0)
		continue;
	      data = (bfd_byte *) xmalloc ((size_t) bfd_section_size (abfd, section));
	      datasize = bfd_section_size (abfd, section);


	      bfd_get_section_contents (abfd, section, (PTR) data, 0, bfd_section_size (abfd, section));

	      for (i = 0; i < bfd_section_size (abfd, section); i += onaline)
		{
		  bfd_size_type j;

		  printf (" %04lx ", (unsigned long int) (i + section->vma));
		  for (j = i; j < i + onaline; j++)
		    {
		      if (j < bfd_section_size (abfd, section))
			printf ("%02x", (unsigned) (data[j]));
		      else
			printf ("  ");
		      if ((j & 3) == 3)
			printf (" ");
		    }

		  printf (" ");
		  for (j = i; j < i + onaline; j++)
		    {
		      if (j >= bfd_section_size (abfd, section))
			printf (" ");
		      else
			printf ("%c", isprint (data[j]) ? data[j] : '.');
		    }
		  putchar ('\n');
		}
	      free (data);
	    }
	}
    }
}

/* Should perhaps share code and display with nm? */
static void
dump_symbols (abfd)
     bfd *abfd;
{

  unsigned int count;
  asymbol **current = syms;

  printf ("SYMBOL TABLE:\n");

  for (count = 0; count < symcount; count++)
    {

      if (*current)
	{
	  bfd *cur_bfd = bfd_asymbol_bfd(*current);
	  if (cur_bfd)
	    {
	      bfd_print_symbol (cur_bfd,
				stdout,
				*current, bfd_print_symbol_all);
	      printf ("\n");
	    }

	}
      current++;
    }
  printf ("\n");
  printf ("\n");
}

static void
dump_relocs (abfd)
     bfd *abfd;
{
  arelent **relpp;
  unsigned int relcount;
  asection *a;

  for (a = abfd->sections; a != (asection *) NULL; a = a->next)
    {
      if (a == &bfd_abs_section)
	continue;
      if (a == &bfd_und_section)
	continue;
      if (bfd_is_com_section (a))
	continue;

      if (only)
	{
	  if (strcmp (only, a->name))
	    continue;
	}
      else if ((a->flags & SEC_RELOC) == 0)
	continue;

      printf ("RELOCATION RECORDS FOR [%s]:", a->name);

      if (bfd_get_reloc_upper_bound (abfd, a) == 0)
	{
	  printf (" (none)\n\n");
	}
      else
	{
	  arelent **p;

	  relpp = (arelent **) xmalloc (bfd_get_reloc_upper_bound (abfd, a));
	  /* Note that this must be done *before* we sort the syms table. */
	  relcount = bfd_canonicalize_reloc (abfd, a, relpp, syms);
	  if (relcount == 0)
	    {
	      printf (" (none)\n\n");
	    }
	  else
	    {
	      printf ("\n");
	      /* Get column headers lined up reasonably.  */
	      {
		static int width;
		if (width == 0)
		  {
		    char buf[30];
		    sprintf_vma (buf, (bfd_vma) -1);
		    width = strlen (buf) - 7;
		  }
		printf ("OFFSET %*s TYPE %*s VALUE \n", width, "", 12, "");
	      }

	      for (p = relpp; relcount && *p != (arelent *) NULL; p++,
		   relcount--)
		{
		  arelent *q = *p;
		  CONST char *sym_name;
		  CONST char *section_name;

		  if (q->sym_ptr_ptr && *q->sym_ptr_ptr)
		    {
		      sym_name = (*(q->sym_ptr_ptr))->name;
		      section_name = (*(q->sym_ptr_ptr))->section->name;
		    }
		  else
		    {
		      sym_name = NULL;
		      section_name = NULL;
		    }
		  if (sym_name)
		    {
		      printf_vma (q->address);
		      printf (" %-16s  %s",
			      q->howto->name,
			      sym_name);
		    }
		  else
		    {
		      if (section_name == (CONST char *) NULL)
			section_name = "*unknown*";
		      printf_vma (q->address);
		      printf (" %-16s  [%s]",
			      q->howto->name,
			      section_name);
		    }
		  if (q->addend)
		    {
		      printf ("+0x");
		      printf_vma (q->addend);
		    }
		  printf ("\n");
		}
	      printf ("\n\n");
	      free (relpp);
	    }
	}

    }
}

#ifdef unix
#define _DUMMY_NAME_ "/dev/null"
#else
#define _DUMMY_NAME_ "##dummy"
#endif
static void
DEFUN (display_info_table, (first, last),
       int first AND int last)
{
  unsigned int i, j;
  extern bfd_target *target_vector[];

  printf ("\n%12s", " ");
  for (i = first; i++ < last && target_vector[i];)
    printf ("%s ", target_vector[i]->name);
  printf ("\n");

  for (j = (int) bfd_arch_obscure + 1; (int) j < (int) bfd_arch_last; j++)
    if (strcmp (bfd_printable_arch_mach (j, 0), "UNKNOWN!") != 0)
      {
	printf ("%11s ", bfd_printable_arch_mach (j, 0));
	for (i = first; i++ < last && target_vector[i];)
	  {
	    bfd_target *p = target_vector[i];
	    bfd *abfd = bfd_openw (_DUMMY_NAME_, p->name);
	    int l = strlen (p->name);
	    int ok;
	    bfd_set_format (abfd, bfd_object);
	    ok = bfd_set_arch_mach (abfd, j, 0);

	    if (ok)
	      printf ("%s ", p->name);
	    else
	      {
		while (l--)
		  printf ("%c", ok ? '*' : '-');
		printf (" ");
	      }
	  }
	printf ("\n");
      }
}

static void
DEFUN_VOID (display_info)
{
  char *colum;
  unsigned int i, j, columns;
  extern bfd_target *target_vector[];
  extern char *getenv ();

  printf ("BFD header file version %s\n", BFD_VERSION);
  for (i = 0; target_vector[i]; i++)
    {
      bfd_target *p = target_vector[i];
      bfd *abfd = bfd_openw (_DUMMY_NAME_, p->name);
      bfd_set_format (abfd, bfd_object);
      printf ("%s\n (header %s, data %s)\n", p->name,
	      p->header_byteorder_big_p ? "big endian" : "little endian",
	      p->byteorder_big_p ? "big endian" : "little endian");
      for (j = (int) bfd_arch_obscure + 1; j < (int) bfd_arch_last; j++)
	if (bfd_set_arch_mach (abfd, (enum bfd_architecture) j, 0))
	  printf ("  %s\n",
		  bfd_printable_arch_mach ((enum bfd_architecture) j, 0));
    }
  columns = 0;
  if ((colum = getenv ("COLUMNS")) != (char *) NULL)
    columns = atoi (colum);
  if (!columns)
    columns = 80;
  for (i = 0; target_vector[i];)
    {
      int old;
      old = i;
      for (j = 12; target_vector[i] && j < columns; i++)
	j += strlen (target_vector[i]->name) + 1;
      i--;
      if (old == i)
	break;
      display_info_table (old, i);
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

  bfd_init ();
  program_name = *argv;

  while ((c = getopt_long (argc, argv, "ib:m:Vdlfahrtxsj:", long_options,
			   (int *) 0))
	 != EOF)
    {
      seenflag = true;
      switch (c)
	{
	case 0:
	  break;		/* we've been given a long option */
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
	case 'i':
	  formats_info = true;
	  break;
	case 'x':
	  dump_symtab = 1;
	  dump_reloc_info = 1;
	  dump_file_header = true;
	  dump_ar_hdrs = 1;
	  dump_section_headers = 1;
	  break;
	case 't':
	  dump_symtab = 1;
	  break;
	case 'd':
	  disassemble = true;
	  break;
	case 's':
	  dump_section_contents = 1;
	  break;
	case 'r':
	  dump_reloc_info = 1;
	  break;
	case 'a':
	  dump_ar_hdrs = 1;
	  break;
	case 'h':
	  dump_section_headers = 1;
	  break;
	case 'H':
	  usage (stdout, 0);
	case 'V':
	  show_version = 1;
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

  if (seenflag == false)
    usage (stderr, 1);

  if (formats_info)
    {
      display_info ();
    }
  else
    {
      if (optind == argc)
	display_file ("a.out", target);
      else
	for (; optind < argc;)
	  display_file (argv[optind++], target);
    }
  return 0;
}
