/* readelf.c -- display contents of an ELF format file
   Copyright (C) 1998 Free Software Foundation, Inc.

   Originally developed by Eric Youngdale <eric@andante.jic.com>
   Modifications by Nick Clifton <nickc@cygnus.com>

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */


#include <assert.h>
#include <sys/stat.h>
#include <stdio.h>

#include "bfd.h"
#include "elf/common.h"
#include "elf/ppc.h"
#include "elf/m32r.h"
#include "elf/mips.h"
#include "elf/external.h"
#include "elf/internal.h"

#include "bucomm.h"
#include "getopt.h"

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif

char *          	program_name = "readelf";
unsigned int    	dynamic_addr;
unsigned int    	dynamic_size;
unsigned int    	rela_addr;
unsigned int    	rela_size;
char *          	dynamic_strings;
char *			string_table;
Elf_Internal_Sym * 	dynamic_symbols;
char            	program_interpreter [64];
int             	dynamic_info [DT_JMPREL + 1];
int             	version_info [16];
int             	loadaddr = 0;
Elf_Internal_Ehdr        elf_header;
Elf_Internal_Shdr *      section_headers;
int 			show_name;
int 			do_dynamic;
int 			do_syms;
int 			do_reloc;
int 			do_sections;
int 			do_segments;
int 			do_using_dynamic;
int 			do_header;
int 			do_dump;
int 			do_version;

static unsigned long int (* byte_get) PARAMS ((unsigned char *, int));

#define NUM_DUMP_SECTS	100
char 			dump_sects [NUM_DUMP_SECTS];

#define HEX_DUMP	1
#define DISASS_DUMP	2

/* Forward declarations for dumb compilers.  */
static const char * get_mips_dynamic_type PARAMS ((unsigned long type));
static const char * get_dynamic_type PARAMS ((unsigned long type));
static char * get_i386_rel_type    PARAMS ((unsigned long rtype));
static char * get_m68k_rel_type    PARAMS ((unsigned long rtype));
static char * get_sparc_rel_type   PARAMS ((unsigned long rtype));
static char * get_m32r_rel_type    PARAMS ((unsigned long rtype));
static char * get_v850_rel_type    PARAMS ((unsigned long rtype));
static char * get_d10v_rel_type    PARAMS ((unsigned long rtype));
/* start-sanitize-d30v */
static char * get_d30v_rel_type    PARAMS ((unsigned long rtype));
/* end-sanitize-d30v */
static char * get_sh_rel_type      PARAMS ((unsigned long rtype));
static char * get_mn10300_rel_type PARAMS ((unsigned long rtype));
static char * get_mn10200_rel_type PARAMS ((unsigned long rtype));
static char * get_ppc_rel_type	   PARAMS ((unsigned long rtype));
static int    dump_relocations
  PARAMS ((FILE *, unsigned long, unsigned long, Elf_Internal_Sym *, char *));
static char * get_file_type     PARAMS ((unsigned e_type));
static char * get_machine_name  PARAMS ((unsigned e_machine));
static char * get_machine_data  PARAMS ((unsigned e_data));
static char * get_machine_flags PARAMS ((unsigned, unsigned e_machine));
static const char * get_mips_segment_type PARAMS ((unsigned long type));
static const char * get_segment_type  PARAMS ((unsigned long p_type));
static char * get_section_type_name PARAMS ((unsigned int sh_type));
static char * get_symbol_binding PARAMS ((unsigned int binding));
static char * get_symbol_type    PARAMS ((unsigned int type));
static void   usage PARAMS ((void));
static void   parse_args PARAMS ((int argc, char ** argv));
static int    process_file_header PARAMS ((void));
static int    process_program_headers PARAMS ((FILE *));
static int    process_section_headers PARAMS ((FILE *));
static void   dynamic_segment_mips_val PARAMS ((Elf_Internal_Dyn *entry));
static int    process_dynamic_segment PARAMS ((FILE *));
static int    process_symbol_table PARAMS ((FILE *));
static int    process_section_contents PARAMS ((FILE *));
static void   process_file PARAMS ((char * file_name));
static int    process_relocs PARAMS ((FILE *));
static int    process_version_sections PARAMS ((FILE *));
static char * get_ver_flags PARAMS ((unsigned int flags));
static char * get_symbol_index_type PARAMS ((unsigned int type));
static int    get_section_headers PARAMS ((FILE * file));
static int    get_file_header PARAMS ((FILE * file));
static Elf_Internal_Sym * get_elf_symbols
  PARAMS ((FILE * file, unsigned long offset, unsigned long number));
static int *  get_dynamic_data PARAMS ((FILE * file, unsigned int number));

#define SECTION_NAME(X) 	(string_table + (X)->sh_name)

#define DT_VERSIONTAGIDX(tag)	(DT_VERNEEDNUM - (tag))	/* Reverse order! */

#define BYTE_GET(field) 	byte_get (field, sizeof (field))

#define NUM_ELEM(array) 	(sizeof (array) / sizeof ((array)[0]))

#define GET_DATA_ALLOC(offset, size, var, type, reason)			\
  if (fseek (file, offset, SEEK_SET))					\
    {									\
      error (_("Unable to seek to start of %s at %x\n"), reason, offset); \
      return 0;								\
    }									\
									\
  var = (type) malloc (size);						\
									\
  if (var == NULL)							\
    {									\
      error (_("Out of memory allocating %d bytes for %s\n"), size, reason); \
      return 0;								\
    } 									\
 									\
  if (fread (var, size, 1, file) != 1) 					\
    { 									\
      error (_("Unable to read in %d bytes of %s\n"), size, reason); 	\
      free (var); 							\
      var = NULL;							\
      return 0; 							\
    }


#define GET_DATA(offset, var, reason) 					\
  if (fseek (file, offset, SEEK_SET))					\
    { 									\
      error (_("Unable to seek to %x for %s\n"), offset, reason);	\
      return 0;								\
    }									\
  else if (fread (& var, sizeof (var), 1, file) != 1)			\
    {									\
      error (_("Unable to read data at %x for %s\n"), offset, reason);	\
      return 0;								\
    }

#ifdef ANSI_PROTOTYPES
static void
error (const char * message, ...)
{
  va_list args;

  fprintf (stderr, _("%s: Error: "), program_name);
  va_start (args, message);
  vfprintf (stderr, message, args);
  va_end (args);
  return;
}

static void
warn (const char * message, ...)
{
  va_list args;

  fprintf (stderr, _("%s: Warning: "), program_name);
  va_start (args, message);
  vfprintf (stderr, message, args);
  va_end (args);
  return;
}
#else
static void
error (va_alist)
     va_dcl
{
  char * message;
  va_list args;

  fprintf (stderr, _("%s: Error: "), program_name);
  va_start (args);
  message = va_arg (args, char *);
  vfprintf (stderr, message, args);
  va_end (args);
  return;
}

static void
warn (va_alist)
     va_dcl
{
  char * message;
  va_list args;

  fprintf (stderr, _("%s: Warning: "), program_name);
  va_start (args);
  message = va_arg (args, char *);
  vfprintf (stderr, message, args);
  va_end (args);
  return;
}
#endif

static unsigned long int
byte_get_little_endian (field, size)
     unsigned char * field;
     int             size;
{
  switch (size)
    {
    case 1:
      return * field;

    case 2:
      return  ((unsigned int) (field [0]))
	|    (((unsigned int) (field [1])) << 8);

    case 4:
      return  ((unsigned long) (field [0]))
	|    (((unsigned long) (field [1])) << 8)
	|    (((unsigned long) (field [2])) << 16)
	|    (((unsigned long) (field [3])) << 24);

    default:
      error (_("Unhandled data length: %d\n"), size);
      abort();
    }
}

static unsigned long int
byte_get_big_endian (field, size)
     unsigned char * field;
     int             size;
{
  switch (size)
    {
    case 1:
      return * field;

    case 2:
      return ((unsigned int) (field [1])) | (((int) (field [0])) << 8);

    case 4:
      return ((unsigned long) (field [3]))
	|   (((unsigned long) (field [2])) << 8)
	|   (((unsigned long) (field [1])) << 16)
	|   (((unsigned long) (field [0])) << 24);

    default:
      error (_("Unhandled data length: %d\n"), size);
      abort();
    }
}

static char *
get_i386_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_386_NONE";
    case  1: return "R_386_32";
    case  2: return "R_386_PC32";
    case  3: return "R_386_GOT32";
    case  4: return "R_386_PLT32";
    case  5: return "R_386_COPY";
    case  6: return "R_386_GLOB_DAT";
    case  7: return "R_386_JMP_SLOT";
    case  8: return "R_386_RELATIVE";
    case  9: return "R_386_GOTOFF";
    case 10: return "R_386_GOTPC";
    case 20: return "R_386_16";
    case 21: return "R_386_PC16";
    case 22: return "R_386_PC8";
    case 23: return "R_386_max";
    default: return _("*INVALID*");
    }
}

static char *
get_m68k_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_68K_NONE";
    case  1: return "R_68K_32";
    case  2: return "R_68K_16";
    case  3: return "R_68K_8";
    case  4: return "R_68K_PC32";
    case  5: return "R_68K_PC16";
    case  6: return "R_68K_PC8";
    case  7: return "R_68K_GOT32";
    case  8: return "R_68K_GOT16";
    case  9: return "R_68K_GOT8";
    case 10: return "R_68K_GOT32O";
    case 11: return "R_68K_GOT16O";
    case 12: return "R_68K_GOT8O";
    case 13: return "R_68K_PLT32";
    case 14: return "R_68K_PLT16";
    case 15: return "R_68K_PLT8";
    case 16: return "R_68K_PLT32O";
    case 17: return "R_68K_PLT16O";
    case 18: return "R_68K_PLT8O";
    case 19: return "R_68K_COPY";
    case 20: return "R_68K_GLOB_DAT";
    case 21: return "R_68K_JMP_SLOT";
    case 22: return "R_68K_RELATIVE";
    default: return _("*INVALID*");
    }
}


static char *
get_sparc_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_SPARC_NONE";
    case  1: return "R_SPARC_8";
    case  2: return "R_SPARC_16";
    case  3: return "R_SPARC_32";
    case  4: return "R_SPARC_DISP8";
    case  5: return "R_SPARC_DISP16";
    case  6: return "R_SPARC_DISP32";
    case  7: return "R_SPARC_WDISP30";
    case  8: return "R_SPARC_WDISP22";
    case  9: return "R_SPARC_HI22";
    case 10: return "R_SPARC_22";
    case 11: return "R_SPARC_13";
    case 12: return "R_SPARC_LO10";
    case 13: return "R_SPARC_GOT10";
    case 14: return "R_SPARC_GOT13";
    case 15: return "R_SPARC_GOT22";
    case 16: return "R_SPARC_PC10";
    case 17: return "R_SPARC_PC22";
    case 18: return "R_SPARC_WPLT30";
    case 19: return "R_SPARC_COPY";
    case 20: return "R_SPARC_GLOB_DAT";
    case 21: return "R_SPARC_JMP_SLOT";
    case 22: return "R_SPARC_RELATIVE";
    case 23: return "R_SPARC_UA32";
    case 24: return "R_SPARC_10";
    case 25: return "R_SPARC_11";
    case 26: return "R_SPARC_64";
    case 27: return "R_SPARC_OLO10";
    case 28: return "R_SPARC_HH22";
    case 29: return "R_SPARC_HM10";
    case 30: return "R_SPARC_LM22";
    case 31: return "R_SPARC_PC_HH22";
    case 32: return "R_SPARC_PC_HM10";
    case 33: return "R_SPARC_PC_LM22";
    case 34: return "R_SPARC_WDISP16";
    case 35: return "R_SPARC_WDISP19";
    case 36: return "R_SPARC_UNUSED_42";
    case 37: return "R_SPARC_7";
    case 38: return "R_SPARC_5";
    case 39: return "R_SPARC_6";
    case 40: return "R_SPARC_DISP64";
    case 41: return "R_SPARC_PLT64";
    case 42: return "R_SPARC_HIX22";
    case 43: return "R_SPARC_LOX10";
    case 44: return "R_SPARC_H44";
    case 45: return "R_SPARC_M44";
    case 46: return "R_SPARC_L44";
    case 47: return "R_SPARC_REGISTER";
    case 48: return "R_SPARC_UA64";
    case 49: return "R_SPARC_UA16";
    case 50: return "R_SPARC_32LE";
    default: return _("*INVALID*");
    }
}


static char *
get_m32r_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_M32R_NONE";
    case  1: return "R_M32R_16";
    case  2: return "R_M32R_32";
    case  3: return "R_M32R_24";
    case  4: return "R_M32R_10_PCREL";
    case  5: return "R_M32R_18_PCREL";
    case  6: return "R_M32R_26_PCREL";
    case  7: return "R_M32R_HI16_ULO";
    case  8: return "R_M32R_HI16_SLO";
    case  9: return "R_M32R_LO16";
    case 10: return "R_M32R_SDA16";
    default: return _("*INVALID*");
    }
}


static char *
get_v850_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_V850_NONE";
    case  1: return "R_V850_9_PCREL";
    case  2: return "R_V850_22_PCREL";
    case  3: return "R_V850_HI16_S";
    case  4: return "R_V850_HI16";
    case  5: return "R_V850_LO16";
    case  6: return "R_V850_32";
    case  7: return "R_V850_16";
    case  8: return "R_V850_8";
    case  9: return "R_V850_SDA_16_16_OFFSET";
    case 10: return "R_V850_SDA_15_16_OFFSET";
    case 11: return "R_V850_ZDA_16_16_OFFSET";
    case 12: return "R_V850_ZDA_15_16_OFFSET";
    case 13: return "R_V850_TDA_6_8_OFFSET";
    case 14: return "R_V850_TDA_7_8_OFFSET";
    case 15: return "R_V850_TDA_7_7_OFFSET";
    case 16: return "R_V850_TDA_16_16_OFFSET";
/* start-sanitize-v850e */
    case 17: return "R_V850_TDA_4_5_OFFSET";
    case 18: return "R_V850_TDA_4_4_OFFSET";
    case 19: return "R_V850_SDA_16_16_SPLIT_OFFSET";
    case 20: return "R_V850_ZDA_16_16_SPLIT_OFFSET";
    case 21: return "R_V850_CALLT_6_7_OFFSET";
    case 22: return "R_V850_CALLT_16_16_OFFSET";
/* end-sanitize-v850e */
    default: return _("*INVALID*");
    }
}


static char *
get_d10v_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case 0: return "R_D10V_NONE";
    case 1: return "R_D10V_10_PCREL_R";
    case 2: return "R_D10V_10_PCREL_L";
    case 3: return "R_D10V_16";
    case 4: return "R_D10V_18";
    case 5: return "R_D10V_18_PCREL";
    case 6: return "R_D10V_32";
    default: return _("*INVALID*");
    }
}

/* start-sanitize-d30v */
static char *
get_d30v_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_D30V_NONE";
    case  1: return "R_D30V_6";
    case  2: return "R_D30V_9_PCREL";
    case  3: return "R_D30V_9_PCREL_R";
    case  4: return "R_D30V_15";
    case  5: return "R_D30V_15_PCREL";
    case  6: return "R_D30V_15_PCREL_R";
    case  7: return "R_D30V_21";
    case  8: return "R_D30V_21_PCREL";
    case  9: return "R_D30V_21_PCREL_R";
    case 10: return "R_D30V_32";
    case 11: return "R_D30V_32_PCREL";
    case 12: return "R_D30V_32_NORMAL";
    default: return _("*INVALID*");
    }
}

/* end-sanitize-d30v */
static char *
get_sh_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case  0: return "R_SH_NONE";
    case  1: return "R_SH_DIR32";
    case  2: return "R_SH_REL32";
    case  3: return "R_SH_DIR8WPN";
    case  4: return "R_SH_IND12W";
    case  5: return "R_SH_DIR8WPL";
    case  6: return "R_SH_DIR8WPZ";
    case  7: return "R_SH_DIR8BP";
    case  8: return "R_SH_DIR8W";
    case  9: return "R_SH_DIR8L";
    case 25: return "R_SH_SWITCH16";
    case 26: return "R_SH_SWITCH32";
    case 27: return "R_SH_USES";
    case 28: return "R_SH_COUNT";
    case 29: return "R_SH_ALIGN";
    case 30: return "R_SH_CODE";
    case 31: return "R_SH_DATA";
    case 32: return "R_SH_LABEL";
    default: return _("*INVALID*");
    }
}


static char *
get_mn10300_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case 0: return "R_MN10300_NONE";
    case 1: return "R_MN10300_32";
    case 2: return "R_MN10300_16";
    case 3: return "R_MN10300_8";
    case 4: return "R_MN10300_PCREL32";
    case 5: return "R_MN10300_PCREL16";
    case 6: return "R_MN10300_PCREL8";
    default: return _("*INVALID*");
    }
}


static char *
get_mn10200_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case 0: return "R_MN10200_NONE";
    case 1: return "R_MN10200_32";
    case 2: return "R_MN10200_16";
    case 3: return "R_MN10200_8";
    case 4: return "R_MN10200_24";
    case 5: return "R_MN10200_PCREL8";
    case 6: return "R_MN10200_PCREL16";
    case 7: return "R_MN10200_PCREL24";
    default: return _("*INVALID*");
    }
}


static char *
get_ppc_rel_type (rtype)
     unsigned long rtype;
{
  switch (rtype)
    {
    case 0:   return "R_PPC_NONE,";
    case 1:   return "R_PPC_ADDR32,";
    case 2:   return "R_PPC_ADDR24,";
    case 3:   return "R_PPC_ADDR16,";
    case 4:   return "R_PPC_ADDR16_LO,";
    case 5:   return "R_PPC_ADDR16_HI,";
    case 6:   return "R_PPC_ADDR16_HA,";
    case 7:   return "R_PPC_ADDR14,";
    case 8:   return "R_PPC_ADDR14_BRTAKEN,";
    case 9:   return "R_PPC_ADDR14_BRNTAKEN,";
    case 10:  return "R_PPC_REL24,";
    case 11:  return "R_PPC_REL14,";
    case 12:  return "R_PPC_REL14_BRTAKEN,";
    case 13:  return "R_PPC_REL14_BRNTAKEN,";
    case 14:  return "R_PPC_GOT16,";
    case 15:  return "R_PPC_GOT16_LO,";
    case 16:  return "R_PPC_GOT16_HI,";
    case 17:  return "R_PPC_GOT16_HA,";
    case 18:  return "R_PPC_PLT24,";
    case 19:  return "R_PPC_COPY,";
    case 21:  return "R_PPC_JMP_SLOT,";
    case 22:  return "R_PPC_RELATIVE,";
    case 23:  return "R_PPC_LOCAL24PC,";
    case 24:  return "R_PPC_UADDR32,";
    case 25:  return "R_PPC_UADDR16,";
    case 26:  return "R_PPC_REL32,";
    case 27:  return "R_PPC_PLT32,";
    case 28:  return "R_PPC_PLTREL32,";
    case 29:  return "R_PPC_PLT16_LO,";
    case 30:  return "R_PPC_PLT16_HI,";
    case 31:  return "R_PPC_PLT16_HA,";
    case 32:  return "R_PPC_SDAREL,";
    case 33:  return "R_PPC_SECTOFF,";
    case 34:  return "R_PPC_SECTOFF_LO,";
    case 35:  return "R_PPC_SECTOFF_HI,";
    case 36:  return "R_PPC_SECTOFF_HA,";
    case 101: return "R_PPC_EMB_NADDR32,";
    case 102: return "R_PPC_EMB_NADDR16,";
    case 103: return "R_PPC_EMB_NADDR16_LO,";
    case 104: return "R_PPC_EMB_NADDR16_HI,";
    case 105: return "R_PPC_EMB_NADDR16_HA,";
    case 106: return "R_PPC_EMB_SDAI16,";
    case 107: return "R_PPC_EMB_SDA2I16,";
    case 108: return "R_PPC_EMB_SDA2REL,";
    case 109: return "R_PPC_EMB_SDA21,";
    case 110: return "R_PPC_EMB_MRKREF,";
    case 111: return "R_PPC_EMB_RELSEC16,";
    case 112: return "R_PPC_EMB_RELST_LO,";
    case 113: return "R_PPC_EMB_RELST_HI,";
    case 114: return "R_PPC_EMB_RELST_HA,";
    case 115: return "R_PPC_EMB_BIT_FLD,";
    case 116: return "R_PPC_EMB_RELSDA,";
    default: return _("*INVALID*");
    }
}


/* Display the contents of the relocation data
   found at the specified offset.  */
static int
dump_relocations (file, rel_offset, rel_size, symtab, strtab)
     FILE *                 file;
     unsigned long          rel_offset;
     unsigned long          rel_size;
     Elf_Internal_Sym *     symtab;
     char *                 strtab;
{
  unsigned int        i;
  int                 is_rela;
  Elf_Internal_Rel *  rels;
  Elf_Internal_Rela * relas;


  /* Compute number of relocations and read them in.  */
  switch (elf_header.e_machine)
    {
    case EM_386:
    case EM_486:
    case EM_CYGNUS_M32R:
    case EM_CYGNUS_D10V:
      {
	Elf32_External_Rel * erels;

	GET_DATA_ALLOC (rel_offset, rel_size, erels,
			Elf32_External_Rel *, "relocs");

	rel_size = rel_size / sizeof (Elf32_External_Rel);

	rels = (Elf_Internal_Rel *) malloc (rel_size *
					    sizeof (Elf_Internal_Rel));

	for (i = 0; i < rel_size; i++)
	  {
	    rels[i].r_offset = BYTE_GET (erels[i].r_offset);
	    rels[i].r_info   = BYTE_GET (erels[i].r_info);
	  }

	free (erels);

	is_rela = 0;
	relas   = (Elf_Internal_Rela *) rels;
      }
    break;

    case EM_68K:
    case EM_SPARC:
    case EM_PPC:
    case EM_CYGNUS_V850:
      /* start-sanitize-d30v */
    case EM_CYGNUS_D30V:
      /* end-sanitize-d30v */
    case EM_CYGNUS_MN10200:
    case EM_CYGNUS_MN10300:
    case EM_SH:
      {
	Elf32_External_Rela * erelas;

	GET_DATA_ALLOC (rel_offset, rel_size, erelas,
			Elf32_External_Rela *, "relocs");

	rel_size = rel_size / sizeof (Elf32_External_Rela);

	relas = (Elf_Internal_Rela *) malloc (rel_size *
					      sizeof (Elf_Internal_Rela));

	for (i = 0; i < rel_size; i++)
	  {
	    relas[i].r_offset = BYTE_GET (erelas[i].r_offset);
	    relas[i].r_info   = BYTE_GET (erelas[i].r_info);
	    relas[i].r_addend = BYTE_GET (erelas[i].r_addend);
	  }

	free (erelas);

	is_rela = 1;
	rels    = (Elf_Internal_Rel *) relas;
      }
    break;

    default:
      warn (_("Don't know about relocations on this machine architecture\n"));
      return 0;
    }

  if (is_rela)
    printf
      (_("  Offset    Value Type            Symbol's Value  Symbol's Name  Addend\n"));
  else
    printf
      (_("  Offset    Value Type            Symbol's Value  Symbol's Name\n"));

  for (i = 0; i < rel_size; i++)
    {
      char *        rtype;
      unsigned long offset;
      unsigned long info;
      int           symtab_index;

      if (is_rela)
	{
	  offset = relas [i].r_offset;
	  info   = relas [i].r_info;
	}
      else
	{
	  offset = rels [i].r_offset;
	  info   = rels [i].r_info;
	}

      printf ("  %8.8lx  %5.5lx ", offset, info);

      switch (elf_header.e_machine)
	{
	default:
	  rtype = "<unknown>";
	  break;

	case EM_CYGNUS_M32R:
	  rtype = get_m32r_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_386:
	case EM_486:
	  rtype = get_i386_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_68K:
	  rtype = get_m68k_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_SPARC:
	  rtype = get_sparc_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_CYGNUS_V850:
	  rtype = get_v850_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_CYGNUS_D10V:
	  rtype = get_d10v_rel_type (ELF32_R_TYPE (info));
	  break;

	  /* start-sanitize-d30v */
	case EM_CYGNUS_D30V:
	  rtype = get_d30v_rel_type (ELF32_R_TYPE (info));
	  break;

	  /* end-sanitize-d30v */
	case EM_SH:
	  rtype = get_sh_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_CYGNUS_MN10300:
	  rtype = get_mn10300_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_CYGNUS_MN10200:
	  rtype = get_mn10200_rel_type (ELF32_R_TYPE (info));
	  break;

	case EM_PPC:
	  rtype = get_ppc_rel_type (ELF32_R_TYPE (info));
	  break;
	}

      printf ("%-21.21s", rtype);

      symtab_index = ELF32_R_SYM (info);

      if (symtab_index && symtab != NULL)
	{
	  Elf_Internal_Sym * psym;

	  psym = symtab + symtab_index;

	  printf (" %08lx  ", (unsigned long) psym->st_value);

	  if (psym->st_name == 0)
	    printf ("%-17.17s",
		    SECTION_NAME (section_headers + psym->st_shndx));
	  else if (strtab == NULL)
	    printf (_("<string table index %d>"), psym->st_name);
	  else
	    printf ("%-17.17s", strtab + psym->st_name);

	  if (is_rela)
	    printf (" + %lx", (unsigned long) relas [i].r_addend);
	}

      putchar ('\n');
    }

  free (relas);

  return 1;
}

static const char *
get_mips_dynamic_type (type)
     unsigned long type;
{
  switch (type)
    {
    case DT_MIPS_RLD_VERSION: return "MIPS_RLD_VERSION";
    case DT_MIPS_TIME_STAMP: return "MIPS_TIME_STAMP";
    case DT_MIPS_ICHECKSUM: return "MIPS_ICHECKSUM";
    case DT_MIPS_IVERSION: return "MIPS_IVERSION";
    case DT_MIPS_FLAGS: return "MIPS_FLAGS";
    case DT_MIPS_BASE_ADDRESS: return "MIPS_BASE_ADDRESS";
    case DT_MIPS_MSYM: return "MIPS_MSYM";
    case DT_MIPS_CONFLICT: return "MIPS_CONFLICT";
    case DT_MIPS_LIBLIST: return "MIPS_LIBLIST";
    case DT_MIPS_LOCAL_GOTNO: return "MIPS_LOCAL_GOTNO";
    case DT_MIPS_CONFLICTNO: return "MIPS_CONFLICTNO";
    case DT_MIPS_LIBLISTNO: return "MIPS_LIBLISTNO";
    case DT_MIPS_SYMTABNO: return "MIPS_SYMTABNO";
    case DT_MIPS_UNREFEXTNO: return "MIPS_UNREFEXTNO";
    case DT_MIPS_GOTSYM: return "MIPS_GOTSYM";
    case DT_MIPS_HIPAGENO: return "MIPS_HIPAGENO";
    case DT_MIPS_RLD_MAP: return "MIPS_RLD_MAP";
    case DT_MIPS_DELTA_CLASS: return "MIPS_DELTA_CLASS";
    case DT_MIPS_DELTA_CLASS_NO: return "MIPS_DELTA_CLASS_NO";
    case DT_MIPS_DELTA_INSTANCE: return "MIPS_DELTA_INSTANCE";
    case DT_MIPS_DELTA_INSTANCE_NO: return "MIPS_DELTA_INSTANCE_NO";
    case DT_MIPS_DELTA_RELOC: return "MIPS_DELTA_RELOC";
    case DT_MIPS_DELTA_RELOC_NO: return "MIPS_DELTA_RELOC_NO";
    case DT_MIPS_DELTA_SYM: return "MIPS_DELTA_SYM";
    case DT_MIPS_DELTA_SYM_NO: return "MIPS_DELTA_SYM_NO";
    case DT_MIPS_DELTA_CLASSSYM: return "MIPS_DELTA_CLASSSYM";
    case DT_MIPS_DELTA_CLASSSYM_NO: return "MIPS_DELTA_CLASSSYM_NO";
    case DT_MIPS_CXX_FLAGS: return "MIPS_CXX_FLAGS";
    case DT_MIPS_PIXIE_INIT: return "MIPS_PIXIE_INIT";
    case DT_MIPS_SYMBOL_LIB: return "MIPS_SYMBOL_LIB";
    case DT_MIPS_LOCALPAGE_GOTIDX: return "MIPS_LOCALPAGE_GOTIDX";
    case DT_MIPS_LOCAL_GOTIDX: return "MIPS_LOCAL_GOTIDX";
    case DT_MIPS_HIDDEN_GOTIDX: return "MIPS_HIDDEN_GOTIDX";
    case DT_MIPS_PROTECTED_GOTIDX: return "MIPS_PROTECTED_GOTIDX";
    case DT_MIPS_OPTIONS: return "MIPS_OPTIONS";
    case DT_MIPS_INTERFACE: return "MIPS_INTERFACE";
    case DT_MIPS_DYNSTR_ALIGN: return "MIPS_DYNSTR_ALIGN";
    case DT_MIPS_INTERFACE_SIZE: return "MIPS_INTERFACE_SIZE";
    case DT_MIPS_RLD_TEXT_RESOLVE_ADDR: return "MIPS_RLD_TEXT_RESOLVE_ADDR";
    case DT_MIPS_PERF_SUFFIX: return "MIPS_PERF_SUFFIX";
    case DT_MIPS_COMPACT_SIZE: return "MIPS_COMPACT_SIZE";
    case DT_MIPS_GP_VALUE: return "MIPS_GP_VALUE";
    case DT_MIPS_AUX_DYNAMIC: return "MIPS_AUX_DYNAMIC";
    default:
      return NULL;
    }
}

static const char *
get_dynamic_type (type)
     unsigned long type;
{
  static char buff [32];

  switch (type)
    {
    case DT_NULL:	return _("NULL");
    case DT_NEEDED:	return _("NEEDED");
    case DT_PLTRELSZ:	return _("PLTRELSZ");
    case DT_PLTGOT:	return _("PLTGOT");
    case DT_HASH:	return _("HASH");
    case DT_STRTAB:	return _("STRTAB");
    case DT_SYMTAB:	return _("SYMTAB");
    case DT_RELA:	return _("RELA");
    case DT_RELASZ:	return _("RELASZ");
    case DT_RELAENT:	return _("RELAENT");
    case DT_STRSZ:	return _("STRSZ");
    case DT_SYMENT:	return _("SYMENT");
    case DT_INIT:	return _("INIT");
    case DT_FINI:	return _("FINI");
    case DT_SONAME:	return _("SONAME");
    case DT_RPATH:	return _("RPATH");
    case DT_SYMBOLIC:	return _("SYMBOLIC");
    case DT_REL:	return _("REL");
    case DT_RELSZ:	return _("RELSZ");
    case DT_RELENT:	return _("RELENT");
    case DT_PLTREL:	return _("PLTREL");
    case DT_DEBUG:	return _("DEBUG");
    case DT_TEXTREL:	return _("TEXTREL");
    case DT_JMPREL:	return _("JMPREL");
    case DT_VERDEF:	return _("VERDEF");
    case DT_VERDEFNUM:	return _("VERDEFNUM");
    case DT_VERNEED:	return _("VERNEED");
    case DT_VERNEEDNUM:	return _("VERNEEDNUM");
    case DT_VERSYM:	return _("VERSYN");
    case DT_AUXILIARY:	return _("AUXILARY");
    case DT_FILTER:	return _("FILTER");

    default:
      if ((type >= DT_LOPROC) && (type <= DT_HIPROC))
	{
	  const char *result = NULL;
	  switch (elf_header.e_machine)
	    {
	    case EM_MIPS:
	    case EM_MIPS_RS4_BE:
	      result = get_mips_dynamic_type (type);
	  default:
	    }

	  if (result == NULL)
	    {
	      sprintf (buff, _("Processor Specific: (%x)"), type);
	      result = buff;
	    }
	  return result;
	}
      else
	sprintf (buff, _("<unknown>: %x"), type);
      return buff;
    }
}

static char *
get_file_type (e_type)
     unsigned e_type;
{
  static char buff [32];

  switch (e_type)
    {
    case ET_NONE:	return _("None");
    case ET_REL:	return _("Relocatable file");
    case ET_EXEC:       return _("Executable file");
    case ET_DYN:        return _("Shared object file");
    case ET_CORE:       return _("Core file");

    default:
      if ((e_type >= ET_LOPROC) && (e_type <= ET_HIPROC))
	sprintf (buff, _("Processor Specific: (%x)"), e_type);
      else
	sprintf (buff, _("<unknown>: %x"), e_type);
      return buff;
    }
}

static char *
get_machine_name (e_machine)
     unsigned e_machine;
{
  static char buff [32];

  switch (e_machine)
    {
    case EM_NONE:        	return _("None");
    case EM_M32:         	return "WE32100";
    case EM_SPARC:       	return "Sparc";
    case EM_386:         	return "80386";
    case EM_68K:         	return "MC68000";
    case EM_88K:         	return "MC88000";
    case EM_486:         	return "Intel 80486";
    case EM_860:         	return "Intel 80860";
    case EM_MIPS:        	return "MIPS R3000 big-endian";
    case EM_S370:        	return "Amdahl";
    case EM_MIPS_RS4_BE: 	return "MIPS R400 big-endian";
    case EM_OLD_SPARCV9:	return "Sparc v9 (old)";
    case EM_PARISC:      	return "HPPA";
    case EM_PPC_OLD:		return "Power PC (old)";
    case EM_SPARC32PLUS: 	return "Sparc v8+" ;
    case EM_960:         	return "Intel 90860";
    case EM_PPC:         	return "PowerPC";
    case EM_V800:         	return "NEC V800";
    case EM_FR20:         	return "Fujitsu FR20";
    case EM_RH32:         	return "TRW RH32";
    case EM_MMA:         	return "Fujitsu MMA";
    case EM_ARM:	 	return "ARM";
    case EM_OLD_ALPHA:	 	return "Digital Alpha (old)";
    case EM_SH:		 	return "Hitachi SH";
    case EM_SPARCV9:     	return "Sparc v9";
    case EM_ALPHA:       	return "Alpha";
    case EM_CYGNUS_D10V:        return "d10v";
      /* start-sanitize-d30v */
    case EM_CYGNUS_D30V:        return "d30v";
      /* end-sanitize-d30v */
    case EM_CYGNUS_M32R:	return "M32r";
    case EM_CYGNUS_V850:	return "v850";
    case EM_CYGNUS_MN10300:	return "mn10300";
    case EM_CYGNUS_MN10200:	return "mn10200";

    default:
      sprintf (buff, _("<unknown>: %x"), e_machine);
      return buff;
    }
}

static char *
get_machine_flags (e_flags, e_machine)
     unsigned e_flags;
     unsigned e_machine;
{
  static char buf [1024];

  buf[0] = '\0';
  if (e_flags)
    {
      switch (e_machine)
	{
	default:
	  break;

	case EM_PPC:
	  if (e_flags & EF_PPC_EMB)
	    strcat (buf, ", emb");

	  if (e_flags & EF_PPC_RELOCATABLE)
	    strcat (buf, ", relocatable");

	  if (e_flags & EF_PPC_RELOCATABLE_LIB)
	    strcat (buf, ", relocatable-lib");
	  break;

	case EM_CYGNUS_M32R:
	  if ((e_flags & EF_M32R_ARCH) == E_M32R_ARCH)
	    strcat (buf, ", m32r");

	  /* start-sanitize-m32rx */
#ifdef E_M32RX_ARCH
	  if ((e_flags & EF_M32R_ARCH) == E_M32RX_ARCH)
	    strcat (buf, ", m32rx");
#endif
	  /* end-sanitize-m32rx */
	  break;

	case EM_MIPS:
	case EM_MIPS_RS4_BE:
	  if (e_flags & EF_MIPS_NOREORDER)
	    strcat (buf, ", noreorder");

	  if (e_flags & EF_MIPS_PIC)
	    strcat (buf, ", pic");

	  if (e_flags & EF_MIPS_CPIC)
	    strcat (buf, ", cpic");

	  if (e_flags & EF_MIPS_ABI2)
	    strcat (buf, ", abi2");

	  if ((e_flags & EF_MIPS_ARCH) == E_MIPS_ARCH_1)
	    strcat (buf, ", mips1");

	  if ((e_flags & EF_MIPS_ARCH) == E_MIPS_ARCH_2)
	    strcat (buf, ", mips2");

	  if ((e_flags & EF_MIPS_ARCH) == E_MIPS_ARCH_3)
	    strcat (buf, ", mips3");

	  if ((e_flags & EF_MIPS_ARCH) == E_MIPS_ARCH_4)
	    strcat (buf, ", mips4");
	  break;
	}
    }

  return buf;
}

static char *
get_machine_data (e_data)
     unsigned e_data;
{
  static char buff [32];

  switch (e_data)
    {
    case ELFDATA2LSB: return _("ELFDATA2LSB (little endian)");
    case ELFDATA2MSB: return _("ELFDATA2MSB (big endian)");
    default:
      sprintf (buff, _("<unknown>: %x"), e_data);
      return buff;
    }
}

static const char *
get_mips_segment_type (type)
     unsigned long type;
{
  switch (type)
    {
    case PT_MIPS_REGINFO:
      return "Register Info";
    case PT_MIPS_RTPROC:
      return "Runtime Proc Table";
    case PT_MIPS_OPTIONS:
      return "Options";
    default:
      return "Processor Specific";
    }
}

static const char *
get_segment_type (p_type)
     unsigned long p_type;
{
  static char buff [32];

  switch (p_type)
    {
    case PT_NULL:       return _("Unused");
    case PT_LOAD:       return _("Loadable");
    case PT_DYNAMIC:	return _("Dynamic link info");
    case PT_INTERP:     return _("Interpreter");
    case PT_NOTE:       return _("Auxillary Info");
    case PT_SHLIB:      return _("Shared Library");
    case PT_PHDR:       return _("Program Header");

    default:
      if ((p_type >= PT_LOPROC) && (p_type <= PT_HIPROC))
	switch (elf_header.e_machine)
	  {
	  case EM_MIPS:
	  case EM_MIPS_RS4_BE:
	    return get_mips_segment_type (p_type);
	  default:
	    return "Processor Specific";
          }
      else
	{
	  sprintf (buff, _("<unknown>: %x"), p_type);
	  return buff;
	}
    }
}

static char *
get_section_type_name (sh_type)
     unsigned int sh_type;
{
  static char buff [32];

  switch (sh_type)
    {
    case SHT_NULL:		return _("Unused");
    case SHT_PROGBITS:		return _("Program data");
    case SHT_SYMTAB:		return _("Symbol table");
    case SHT_STRTAB:		return _("String table");
    case SHT_RELA:		return _("Relocations");
    case SHT_HASH:		return _("Symbol hashes");
    case SHT_DYNAMIC:		return _("Dynamic info");
    case SHT_NOTE:		return _("Notes");
    case SHT_NOBITS:		return _("Space, no data");
    case SHT_REL:		return _("Relocations");
    case SHT_SHLIB:		return _("Shared lib info");
    case SHT_DYNSYM:		return _("Dynamic symbols");
    case SHT_GNU_verdef:	return _("Version definition");
    case SHT_GNU_verneed:	return _("Version needs");
    case SHT_GNU_versym:	return _("Version symbols");
    case 0x6ffffff0:	        return "VERSYM";
    case 0x6ffffffc:	        return "VERDEF";
    case 0x7ffffffd:		return "AUXILIARY";
    case 0x7fffffff:		return "FILTER";

    default:
      if ((sh_type >= SHT_LOPROC) && (sh_type <= SHT_HIPROC))
	sprintf (buff, _("cpu defined (%d)"), sh_type - SHT_LOPROC);
      else if ((sh_type >= SHT_LOUSER) && (sh_type <= SHT_HIUSER))
	sprintf (buff, _("app defined (%d)"), sh_type - SHT_LOUSER);
      else
	sprintf (buff, _("<unknown>: %x"), sh_type);
      return buff;
    }
}

struct option options [] =
{
  {"all", no_argument, 0, 'a'},
  {"file-header", no_argument, 0, 'h'},
  {"program-headers", no_argument, 0, 'l'},
  {"headers", no_argument, 0, 'e'},
  {"segments", no_argument, 0, 'l'},
  {"sections", no_argument, 0, 'S'},
  {"section-headers", no_argument, 0, 'S'},
  {"symbols", no_argument, 0, 's'},
  {"relocs", no_argument, 0, 'r'},
  {"dynamic", no_argument, 0, 'd'},
  {"version-info", no_argument, 0, 'V'},
  {"use-dynamic", no_argument, 0, 'D'},

  {"hex-dump", required_argument, 0, 'x'},
#ifdef SUPPORT_DISASSEMBLY
  {"instruction-dump", required_argument, 0, 'i'},
#endif

  {"version", no_argument, 0, 'v'},
  {"help", no_argument, 0, 'H'},

  {0, no_argument, 0, 0}
};

static void
usage ()
{
  fprintf (stdout, _("Usage: readelf {options} elf-file(s)\n"));
  fprintf (stdout, _("  Options are:\n"));
  fprintf (stdout, _("  -a or --all               Equivalent to: -h -l -S -s -r -d -V\n"));
  fprintf (stdout, _("  -h or --file-header       Display the ELF file header\n"));
  fprintf (stdout, _("  -l or --program-headers or --segments\n"));
  fprintf (stdout, _("                            Display the program headers\n"));
  fprintf (stdout, _("  -S or --section-headers or --sections\n"));
  fprintf (stdout, _("                            Display the sections' header\n"));
  fprintf (stdout, _("  -e or --headers           Equivalent to: -h -l -S\n"));
  fprintf (stdout, _("  -s or --symbols           Display the symbol table\n"));
  fprintf (stdout, _("  -r or --relocs            Display the relocations (if present)\n"));
  fprintf (stdout, _("  -d or --dynamic           Display the dynamic segment (if present)\n"));
  fprintf (stdout, _("  -V or --version-info      Display the version sections (if present)\n"));
  fprintf (stdout, _("  -D or --use-dynamic       Use the dynamic section info when displaying symbols\n"));
  fprintf (stdout, _("  -x <number> or --hex-dump=<number>\n"));
  fprintf (stdout, _("                            Dump the contents of section <number>\n"));
#ifdef SUPPORT_DISASSEMBLY
  fprintf (stdout, _("  -i <number> or --instruction-dump=<number>\n"));
  fprintf (stdout, _("                            Disassemble the contents of section <number>\n"));
#endif
  fprintf (stdout, _("  -v or --version           Display the version number of readelf\n"));
  fprintf (stdout, _("  -H or --help              Display this information\n"));
  fprintf (stdout, _("Report bugs to bug-gnu-utils@gnu.org\n"));

  exit (0);
}

static void
parse_args (argc, argv)
     int argc;
     char ** argv;
{
  int c;

  if (argc < 2)
    usage ();

  while ((c = getopt_long
	  (argc, argv, "ersahldSDx:i:vV", options, NULL)) != EOF)
    {
      char *    cp;
      int	section;

      switch (c)
	{
	case 'H':
	  usage ();
	  break;

	case 'a':
	  do_syms ++;
	  do_reloc ++;
	  do_dynamic ++;
	  do_header ++;
	  do_sections ++;
	  do_segments ++;
	  do_version ++;
	  break;
	case 'e':
	  do_header ++;
	  do_sections ++;
	  do_segments ++;
	  break;
	case 'D':
	  do_using_dynamic ++;
	  break;
	case 'r':
	  do_reloc ++;
	  break;
	case 'h':
	  do_header ++;
	  break;
	case 'l':
	  do_segments ++;
	  break;
	case 's':
	  do_syms ++;
	  break;
	case 'S':
	  do_sections ++;
	  break;
	case 'd':
	  do_dynamic ++;
	  break;
	case 'x':
	  do_dump ++;
	  section = strtoul (optarg, & cp, 0);
	  if (! * cp && section >= 0 && section < NUM_DUMP_SECTS)
	    {
	      dump_sects [section] |= HEX_DUMP;
	      break;
	    }
	  goto oops;
#ifdef SUPPORT_DISASSEMBLY
	case 'i':
	  do_dump ++;
	  section = strtoul (optarg, & cp, 0);
	  if (! * cp && section >= 0 && section < NUM_DUMP_SECTS)
	    {
	      dump_sects [section] |= DISASS_DUMP;
	      break;
	    }
	  goto oops;
#endif
	case 'v':
	  print_version (program_name);
	  break;
	case 'V':
	  do_version ++;
	  break;
	default:
	oops:
	  /* xgettext:c-format */
	  error (_("Invalid option '-%c'\n"), c);
	  /* Drop through.  */
	case '?':
	  usage ();
	}
    }

  if (!do_dynamic && !do_syms && !do_reloc && !do_sections
      && !do_segments && !do_header && !do_dump && !do_version)
    usage ();
  else if (argc < 3)
    {
      warn (_("Nothing to do.\n"));
      usage();
    }
}

/* Decode the data held in 'elf_header'.  */
static int
process_file_header ()
{
  if (   elf_header.e_ident [EI_MAG0] != ELFMAG0
      || elf_header.e_ident [EI_MAG1] != ELFMAG1
      || elf_header.e_ident [EI_MAG2] != ELFMAG2
      || elf_header.e_ident [EI_MAG3] != ELFMAG3)
    {
      error
	(_("Not an ELF file - it has the wrong magic bytes at the start\n"));
      return 0;
    }

  if (elf_header.e_ident [EI_CLASS] != ELFCLASS32)
    {
      error (_("Not a 32 bit ELF file\n"));
      return 0;
    }

  if (do_header)
    {
      int i;

      printf (_("ELF Header:\n"));
      printf (_("  Magic:   "));
      for (i = 0; i < EI_NIDENT; i ++)
	printf ("%2.2x ", elf_header.e_ident [i]);
      printf ("\n");
      printf (_("  Type:                              %s\n"),
	      get_file_type (elf_header.e_type));
      printf (_("  Machine:                           %s\n"),
	      get_machine_name (elf_header.e_machine));
      printf (_("  Version:                           0x%lx\n"),
	      (unsigned long) elf_header.e_version);
      printf (_("  Data:                              %s\n"),
	      get_machine_data (elf_header.e_ident [EI_DATA]));
      printf (_("  Entry point address:               0x%lx\n"),
	      (unsigned long) elf_header.e_entry);
      printf (_("  Start of program headers:          %ld (bytes into file)\n"),
	      (long) elf_header.e_phoff);
      printf (_("  Start of section headers:          %ld (bytes into file)\n"),
	      (long) elf_header.e_shoff);
      printf (_("  Flags:                             0x%lx%s\n"),
	      (unsigned long) elf_header.e_flags,
	      get_machine_flags (elf_header.e_flags, elf_header.e_machine));
      printf (_("  Size of this header:               %ld (bytes)\n"),
	      (long) elf_header.e_ehsize);
      printf (_("  Size of program headers:           %ld (bytes)\n"),
	      (long) elf_header.e_phentsize);
      printf (_("  Number of program headers:         %ld\n"),
	      (long) elf_header.e_phnum);
      printf (_("  Size of section headers:           %ld (bytes)\n"),
	      (long) elf_header.e_shentsize);
      printf (_("  Number of section headers:         %ld\n"),
	      (long) elf_header.e_shnum);
      printf (_("  Section header string table index: %ld\n"),
	      (long) elf_header.e_shstrndx);
    }

  return 1;
}


static int
process_program_headers (file)
     FILE * file;
{
  Elf32_External_Phdr * phdrs;
  Elf32_Internal_Phdr * program_headers;
  Elf32_Internal_Phdr * segment;
  unsigned int	        i;

  if (elf_header.e_phnum == 0)
    {
      if (do_segments)
	printf (_("\nThere are no program headers in this file.\n"));
      return 1;
    }

  if (do_segments && !do_header)
    {
      printf (_("\nElf file is %s\n"), get_file_type (elf_header.e_type));
      printf (_("Entry point 0x%lx\n"), (unsigned long) elf_header.e_entry);
      printf (_("There are %d program headers, starting at offset %lx:\n"),
	      elf_header.e_phnum, (unsigned long) elf_header.e_phoff);
    }

  GET_DATA_ALLOC (elf_header.e_phoff,
		  elf_header.e_phentsize * elf_header.e_phnum,
		  phdrs, Elf32_External_Phdr *, "program headers");

  program_headers = (Elf32_Internal_Phdr *) malloc
    (elf_header.e_phnum * sizeof (Elf32_Internal_Phdr));

  if (program_headers == NULL)
    {
      error (_("Out of memory\n"));
      return 0;
    }

  for (i = 0, segment = program_headers;
       i < elf_header.e_phnum;
       i ++, segment ++)
    {
      segment->p_type   = BYTE_GET (phdrs[i].p_type);
      segment->p_offset = BYTE_GET (phdrs[i].p_offset);
      segment->p_vaddr  = BYTE_GET (phdrs[i].p_vaddr);
      segment->p_paddr  = BYTE_GET (phdrs[i].p_paddr);
      segment->p_filesz = BYTE_GET (phdrs[i].p_filesz);
      segment->p_memsz  = BYTE_GET (phdrs[i].p_memsz);
      segment->p_flags  = BYTE_GET (phdrs[i].p_flags);
      segment->p_align  = BYTE_GET (phdrs[i].p_align);
    }

  free (phdrs);

  if (do_segments)
    {
      printf
	(_("\nProgram Header%s:\n"), elf_header.e_phnum > 1 ? "s" : "");
      printf
	(_("  Type             Offset  VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n"));
    }

  loadaddr = -1;
  dynamic_addr = 0;

  for (i = 0, segment = program_headers;
       i < elf_header.e_phnum;
       i ++, segment ++)
    {
      if (do_segments)
	{
	  printf ("  %-16.16s ", get_segment_type (segment->p_type));
	  printf ("0x%5.5lx ", (unsigned long) segment->p_offset);
	  printf ("0x%8.8lx ", (unsigned long) segment->p_vaddr);
	  printf ("0x%8.8lx ", (unsigned long) segment->p_paddr);
	  printf ("0x%5.5lx ", (unsigned long) segment->p_filesz);
	  printf ("0x%5.5lx ", (unsigned long) segment->p_memsz);
	  printf ("%c%c%c ",
		  (segment->p_flags & PF_R ? 'R' : ' '),
		  (segment->p_flags & PF_W ? 'W' : ' '),
		  (segment->p_flags & PF_X ? 'E' : ' '));
	  printf ("%#lx", (unsigned long) segment->p_align);
	}

      switch (segment->p_type)
	{
	case PT_LOAD:
	  if (loadaddr == -1)
	    loadaddr = (segment->p_vaddr & 0xfffff000)
	      - (segment->p_offset & 0xfffff000);
	  break;

	case PT_DYNAMIC:
	  if (dynamic_addr)
	    error (_("more than one dynamic segment\n"));

	  dynamic_addr = segment->p_offset;
	  dynamic_size = segment->p_filesz;
	  break;

	case PT_INTERP:
	  if (fseek (file, segment->p_offset, SEEK_SET))
	    error (_("Unable to find program interpreter name\n"));
	  else
	    {
	      program_interpreter[0] = 0;
	      fscanf (file, "%63s", program_interpreter);

	      if (do_segments)
		printf (_("\n      [Requesting program interpreter: %s]"),
		    program_interpreter);
	    }
	  break;
	}

      if (do_segments)
	putc ('\n', stdout);
    }

  if (loadaddr == -1)
    {
      /* Very strange. */
      loadaddr = 0;
    }

  if (do_segments && section_headers != NULL)
    {
      printf (_("\n Section to Segment mapping:\n"));
      printf (_("  Segment Sections...\n"));

      assert (string_table != NULL);

      for (i = 0; i < elf_header.e_phnum; i++)
	{
	  int        j;
	  Elf32_Internal_Shdr * section;

	  segment = program_headers + i;
	  section = section_headers;

	  printf ("   %2.2d     ", i);

	  for (j = 0; j < elf_header.e_shnum; j++, section ++)
	    {
	      if (section->sh_size > 0
		  /* Compare allocated sections by VMA, unallocated
		     sections by file offset.  */
		  && (section->sh_flags & SHF_ALLOC
		      ? (section->sh_addr >= segment->p_vaddr
			 && section->sh_addr + section->sh_size
			 <= segment->p_vaddr + segment->p_memsz)
		      : (section->sh_offset >= segment->p_offset
			 && (section->sh_offset + section->sh_size
			     <= segment->p_offset + segment->p_filesz))))
		printf ("%s ", SECTION_NAME (section));
	    }

	  putc ('\n',stdout);
	}
    }

  free (program_headers);

  return 1;
}


static int
get_section_headers (file)
     FILE * file;
{
  Elf32_External_Shdr * shdrs;
  Elf32_Internal_Shdr * internal;
  unsigned int          i;

  GET_DATA_ALLOC (elf_header.e_shoff,
		  elf_header.e_shentsize * elf_header.e_shnum,
		  shdrs, Elf32_External_Shdr *, "section headers");

  section_headers = (Elf32_Internal_Shdr *) malloc
    (elf_header.e_shnum * sizeof (Elf32_Internal_Shdr));

  if (section_headers == NULL)
    {
      error (_("Out of memory\n"));
      return 0;
    }

  for (i = 0, internal = section_headers;
       i < elf_header.e_shnum;
       i ++, internal ++)
    {
      internal->sh_name      = BYTE_GET (shdrs[i].sh_name);
      internal->sh_type      = BYTE_GET (shdrs[i].sh_type);
      internal->sh_flags     = BYTE_GET (shdrs[i].sh_flags);
      internal->sh_addr      = BYTE_GET (shdrs[i].sh_addr);
      internal->sh_offset    = BYTE_GET (shdrs[i].sh_offset);
      internal->sh_size      = BYTE_GET (shdrs[i].sh_size);
      internal->sh_link      = BYTE_GET (shdrs[i].sh_link);
      internal->sh_info      = BYTE_GET (shdrs[i].sh_info);
      internal->sh_addralign = BYTE_GET (shdrs[i].sh_addralign);
      internal->sh_entsize   = BYTE_GET (shdrs[i].sh_entsize);
    }

  free (shdrs);

  return 1;
}

static Elf_Internal_Sym *
get_elf_symbols (file, offset, number)
     FILE * file;
     unsigned long offset;
     unsigned long number;
{
  Elf32_External_Sym * esyms;
  Elf_Internal_Sym *   isyms;
  Elf_Internal_Sym *   psym;
  unsigned int         j;

  GET_DATA_ALLOC (offset, number * sizeof (Elf32_External_Sym),
		  esyms, Elf32_External_Sym *, "symbols");

  isyms = (Elf_Internal_Sym *) malloc (number * sizeof (Elf_Internal_Sym));

  if (isyms == NULL)
    {
      error (_("Out of memory\n"));
      free (esyms);

      return NULL;
    }

  for (j = 0, psym = isyms;
       j < number;
       j ++, psym ++)
    {
      psym->st_name  = BYTE_GET (esyms[j].st_name);
      psym->st_value = BYTE_GET (esyms[j].st_value);
      psym->st_size  = BYTE_GET (esyms[j].st_size);
      psym->st_shndx = BYTE_GET (esyms[j].st_shndx);
      psym->st_info  = BYTE_GET (esyms[j].st_info);
      psym->st_other = BYTE_GET (esyms[j].st_other);
    }

  free (esyms);

  return isyms;
}

static int
process_section_headers (file)
     FILE * file;
{
  Elf32_Internal_Shdr * section;
  int        i;

  section_headers = NULL;

  if (elf_header.e_shnum == 0)
    {
      if (do_sections)
	printf (_("\nThere are no sections in this file.\n"));

      return 1;
    }

  if (do_sections && !do_header)
    printf (_("There are %d section headers, starting at offset %x:\n"),
	    elf_header.e_shnum, elf_header.e_shoff);

  if (! get_section_headers (file))
    return 0;

  /* Read in the string table, so that we have names to display.  */
  section = section_headers + elf_header.e_shstrndx;

  if (section->sh_size != 0)
    {
      unsigned long string_table_offset;

      string_table_offset = section->sh_offset;

      GET_DATA_ALLOC (section->sh_offset, section->sh_size,
		      string_table, char *, "string table");
    }

  /* Scan the sections for the dynamic symbol table
     and dynamic string table.  */
  dynamic_symbols = NULL;
  dynamic_strings = NULL;
  for (i = 0, section = section_headers;
       i < elf_header.e_shnum;
       i ++, section ++)
    {
      if (section->sh_type == SHT_DYNSYM)
	{
	  if (dynamic_symbols != NULL)
	    {
	      error (_("File contains multiple dynamic symbol tables\n"));
	      continue;
	    }

	  dynamic_symbols = get_elf_symbols
	    (file, section->sh_offset, section->sh_size / section->sh_entsize);
	}
      else if (section->sh_type == SHT_STRTAB
	       && strcmp (SECTION_NAME (section), ".dynstr") == 0)
	{
	  if (dynamic_strings != NULL)
	    {
	      error (_("File contains multiple dynamic string tables\n"));
	      continue;
	    }

	  GET_DATA_ALLOC (section->sh_offset, section->sh_size,
			  dynamic_strings, char *, "dynamic strings");
	}
    }

  if (! do_sections)
    return 1;

  printf (_("\nSection Header%s:\n"), elf_header.e_shnum > 1 ? "s" : "");
  printf
    (_("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n"));

  for (i = 0, section = section_headers;
       i < elf_header.e_shnum;
       i ++, section ++)
    {
      printf ("  [%2d] %-17.17s %-15.15s ",
	      i,
	      SECTION_NAME (section),
	      get_section_type_name (section->sh_type));

      printf ( "%8.8lx %6.6lx %6.6lx %2.2lx",
	       (unsigned long) section->sh_addr,
	       (unsigned long) section->sh_offset,
	       (unsigned long) section->sh_size,
	       (unsigned long) section->sh_entsize);

      printf (" %c%c%c %2ld %3lx %ld \n",
	      (section->sh_flags & SHF_WRITE ? 'W' : ' '),
	      (section->sh_flags & SHF_ALLOC ? 'A' : ' '),
	      (section->sh_flags & SHF_EXECINSTR ? 'X' : ' '),
	      (unsigned long) section->sh_link,
	      (unsigned long) section->sh_info,
	      (unsigned long) section->sh_addralign);
    }

  return 1;
}

/* Process the reloc section.  */
static int
process_relocs (file)
     FILE * file;
{
  unsigned long    rel_size;
  unsigned long	   rel_offset;


  if (!do_reloc)
    return 1;

  if (do_using_dynamic)
    {
      rel_size   = 0;
      rel_offset = 0;

      if (dynamic_info [DT_REL])
	{
	  rel_offset = dynamic_info [DT_REL];
	  rel_size   = dynamic_info [DT_RELSZ];
	}
      else if (dynamic_info [DT_RELA])
	{
	  rel_offset = dynamic_info [DT_RELA];
	  rel_size   = dynamic_info [DT_RELASZ];
	}
      else if (dynamic_info [DT_JMPREL])
	{
	  rel_offset = dynamic_info [DT_JMPREL];
	  rel_size   = dynamic_info [DT_PLTRELSZ];
	}

      if (rel_size)
	{
	  printf
	    (_("\nRelocation section at offset 0x%x contains %d bytes:\n"),
	     rel_offset, rel_size);

	  dump_relocations (file, rel_offset - loadaddr, rel_size,
			    dynamic_symbols, dynamic_strings);
	}
      else
	printf (_("\nThere are no dynamic relocations in this file.\n"));
    }
  else
    {
      Elf32_Internal_Shdr *    section;
      unsigned long i;
      int           found = 0;

      assert (string_table != NULL);

      for (i = 0, section = section_headers;
	   i < elf_header.e_shnum;
	   i++, section ++)
	{
	  if (   section->sh_type != SHT_RELA
	      && section->sh_type != SHT_REL)
	    continue;

	  rel_offset = section->sh_offset;
	  rel_size   = section->sh_size;

	  if (rel_size)
	    {
	      Elf32_Internal_Shdr * strsec;
	      Elf32_Internal_Shdr * symsec;
	      Elf_Internal_Sym *    symtab;
	      char *                strtab;

	      printf
		(_("\nRelocation section '%s' at offset 0x%x contains %d entries:\n"),
		 SECTION_NAME (section), rel_offset,
		 rel_size / section->sh_entsize);

	      symsec = section_headers + section->sh_link;

	      symtab = get_elf_symbols (file, symsec->sh_offset,
					symsec->sh_size / symsec->sh_entsize);

	      if (symtab == NULL)
		continue;

	      strsec = section_headers + symsec->sh_link;

	      GET_DATA_ALLOC (strsec->sh_offset, strsec->sh_size, strtab,
			      char *, "string table");

	      dump_relocations (file, rel_offset, rel_size, symtab, strtab);

	      free (strtab);
	      free (symtab);

	      found = 1;
	    }
	}

      if (! found)
	printf (_("\nThere are no relocations in this file.\n"));
    }

  return 1;
}


static void
dynamic_segment_mips_val (entry)
     Elf_Internal_Dyn *entry;
{
  switch (entry->d_tag)
    {
    case DT_MIPS_LOCAL_GOTNO:
    case DT_MIPS_CONFLICTNO:
    case DT_MIPS_LIBLISTNO:
    case DT_MIPS_SYMTABNO:
    case DT_MIPS_UNREFEXTNO:
    case DT_MIPS_HIPAGENO:
    case DT_MIPS_DELTA_CLASS_NO:
    case DT_MIPS_DELTA_INSTANCE_NO:
    case DT_MIPS_DELTA_RELOC_NO:
    case DT_MIPS_DELTA_SYM_NO:
    case DT_MIPS_DELTA_CLASSSYM_NO:
      if (do_dynamic)
	printf ("%#ld\n", (long) entry->d_un.d_ptr);
      break;
    default:
      if (do_dynamic)
	printf ("%#lx\n", (long) entry->d_un.d_ptr);
    }
}

/* Parse the dynamic segment */
static int
process_dynamic_segment (file)
     FILE * file;
{
  Elf_Internal_Dyn *    dynamic_segment;
  Elf_Internal_Dyn *    entry;
  Elf32_External_Dyn *  edyn;
  unsigned int i;

  if (dynamic_size == 0)
    {
      if (do_dynamic)
	printf (_("\nThere is no dynamic segment in this file.\n"));

      return 1;
    }

  GET_DATA_ALLOC (dynamic_addr, dynamic_size,
		  edyn, Elf32_External_Dyn *, "dynamic segment");

  dynamic_size = dynamic_size / sizeof (Elf32_External_Dyn);

  dynamic_segment = (Elf_Internal_Dyn *)
    malloc (dynamic_size * sizeof (Elf_Internal_Dyn));

  if (dynamic_segment == NULL)
    {
      error (_("Out of memory\n"));
      free (edyn);
      return 0;
    }

  for (i = 0, entry = dynamic_segment;
       i < dynamic_size;
       i ++, entry ++)
    {
      entry->d_tag      = BYTE_GET (edyn [i].d_tag);
      entry->d_un.d_val = BYTE_GET (edyn [i].d_un.d_val);
    }

  free (edyn);

  /* Find the appropriate symbol table.  */
  if (dynamic_symbols == NULL)
    {
      for (i = 0, entry = dynamic_segment;
	   i < dynamic_size;
	   ++i, ++ entry)
	{
	  unsigned long        offset;
	  long                 num_syms;

	  if (entry->d_tag != DT_SYMTAB)
	    continue;

	  dynamic_info [DT_SYMTAB] = entry->d_un.d_val;

	  /* Since we do not know how big the symbol table is,
	     we default to reading in the entire file (!) and
	     processing that.  This is overkill, I know, but it
	     should work. */

	  offset = entry->d_un.d_val - loadaddr;

	  if (fseek (file, 0, SEEK_END))
	    error (_("Unable to seek to end of file!"));

	  num_syms = (ftell (file) - offset) / sizeof (Elf32_External_Sym);

	  if (num_syms < 1)
	    {
	      error (_("Unable to determine the number of symbols to load\n"));
	      continue;
	    }

	  dynamic_symbols = get_elf_symbols (file, offset, num_syms);
	}
    }

  /* Similarly find a string table.  */
  if (dynamic_strings == NULL)
    {
      for (i = 0, entry = dynamic_segment;
	   i < dynamic_size;
	   ++i, ++ entry)
	{
	  unsigned long offset;
	  long          str_tab_len;

	  if (entry->d_tag != DT_STRTAB)
	    continue;

	  dynamic_info [DT_STRTAB] = entry->d_un.d_val;

	  /* Since we do not know how big the string table is,
	     we default to reading in the entire file (!) and
	     processing that.  This is overkill, I know, but it
	     should work. */

	  offset = entry->d_un.d_val - loadaddr;
	  if (fseek (file, 0, SEEK_END))
	    error (_("Unable to seek to end of file\n"));
	  str_tab_len = ftell (file) - offset;

	  if (str_tab_len < 1)
	    {
	      error
		(_("Unable to determine the length of the dynamic string table\n"));
	      continue;
	    }

	  GET_DATA_ALLOC (offset, str_tab_len, dynamic_strings, char *,
			  "dynamic string table");

	  break;
	}
    }

  if (do_dynamic && dynamic_addr)
    printf (_("\nDynamic segment at offset 0x%x contains %d entries:\n"),
	    dynamic_addr, dynamic_size % 1000);
  if (do_dynamic)
    printf (_("  Tag        Type                         Name/Value\n"));

  for (i = 0, entry = dynamic_segment;
       /* XXX SGI's linker produces a number which is 1000 higher.  */
       i < (dynamic_size % 1000);
       i++, entry ++)
    {
      if (do_dynamic)
	printf (_("  0x%-8.8lx (%s)%*s"),
		(unsigned long) entry->d_tag,
		get_dynamic_type (entry->d_tag),
		27 - strlen (get_dynamic_type (entry->d_tag)),
		" ");

      switch (entry->d_tag)
	{
	case DT_AUXILIARY:
	case DT_FILTER:
	  if (do_dynamic)
	    {
	      if (entry->d_tag == DT_AUXILIARY)
		printf (_("Auxiliary library"));
	      else
		printf (_("Filter library"));

	      if (dynamic_strings)
		printf (": [%s]\n", dynamic_strings + entry->d_un.d_val);
	      else
		printf (": %#lx\n", (long) entry->d_un.d_val);
	    }
	  break;

	case DT_NULL	:
	case DT_NEEDED	:
	case DT_PLTRELSZ:
	case DT_PLTGOT	:
	case DT_HASH	:
	case DT_STRTAB	:
	case DT_SYMTAB	:
	case DT_RELA	:
	case DT_RELASZ	:
	case DT_RELAENT	:
	case DT_STRSZ	:
	case DT_SYMENT	:
	case DT_INIT	:
	case DT_FINI	:
	case DT_SONAME	:
	case DT_RPATH	:
	case DT_SYMBOLIC:
	case DT_REL	:
	case DT_RELSZ	:
	case DT_RELENT	:
	case DT_PLTREL	:
	case DT_DEBUG	:
	case DT_TEXTREL	:
	case DT_JMPREL	:
	  dynamic_info [entry->d_tag] = entry->d_un.d_val;

	  if (do_dynamic)
	    {
	      char * name;

	      if (dynamic_strings == NULL)
		name = NULL;
	      else
		name = dynamic_strings + entry->d_un.d_val;

	      if (name)
		{
		  switch (entry->d_tag)
		    {
		    case DT_NEEDED:
		      printf (_("Shared library: [%s]"), name);

		      if (strcmp (name, program_interpreter))
			printf ("\n");
		      else
			printf (_(" program interpreter\n"));
		      break;

		    case DT_SONAME:
		      printf (_("Library soname: [%s]\n"), name);
		      break;

		    case DT_RPATH:
		      printf (_("Library rpath: [%s]\n"), name);
		      break;

		    default:
		      printf ("%#lx\n", (long) entry->d_un.d_val);
		    }
		}
	      else
		printf ("%#lx\n", (long) entry->d_un.d_val);
	    }
	  break;

	default:
	  if ((entry->d_tag >= DT_VERSYM) && (entry->d_tag <= DT_VERNEEDNUM))
	    {
	      version_info [DT_VERSIONTAGIDX (entry->d_tag)] =
		entry->d_un.d_val;

	      if (do_dynamic)
		printf ("%#lx\n", (long) entry->d_un.d_ptr);
	    }
	  else
	    switch (elf_header.e_machine)
	      {
	      case EM_MIPS:
	      case EM_MIPS_RS4_BE:
		dynamic_segment_mips_val (entry);
		break;
	      default:
		if (do_dynamic)
		  printf ("%#lx\n", (long) entry->d_un.d_ptr);
	      }
	  break;
	}
    }

  free (dynamic_segment);

  return 1;
}

static char *
get_ver_flags (flags)
     unsigned int flags;
{
  static char buff [32];

  buff[0] = 0;

  if (flags == 0)
    return _("none");

  if (flags & VER_FLG_BASE)
    strcat (buff, "BASE ");

  if (flags & VER_FLG_WEAK)
    {
      if (flags & VER_FLG_BASE)
	strcat (buff, "| ");

      strcat (buff, "WEAK ");
    }

  if (flags & ~(VER_FLG_BASE | VER_FLG_WEAK))
    strcat (buff, "| <unknown>");

  return buff;
}

/* Display the contents of the version sections.  */
static int
process_version_sections (file)
     FILE * file;
{
  Elf32_Internal_Shdr * section;
  unsigned   i;
  int        found = 0;

  if (! do_version)
    return 1;

  for (i = 0, section = section_headers;
       i < elf_header.e_shnum;
       i++, section ++)
    {
      switch (section->sh_type)
	{
	case SHT_GNU_verdef:
	  {
	    Elf_External_Verdef * edefs;
	    unsigned int          idx;
	    unsigned int          cnt;

	    found = 1;

	    printf
	      (_("\nVersion definition section '%s' contains %d entries:\n"),
	       SECTION_NAME (section), section->sh_info);

	    printf (_("  Addr: %#08x  Offset: %#08x  Link: %x (%s)\n"),
		    section->sh_addr, section->sh_offset, section->sh_link,
		    SECTION_NAME (section_headers + section->sh_link));

	    GET_DATA_ALLOC (section->sh_offset, section->sh_size,
			    edefs, Elf_External_Verdef *,
			    "version definition section");

	    for (idx = cnt = 0; cnt < section->sh_info; ++ cnt)
	      {
		char *                 vstart;
		Elf_External_Verdef *  edef;
		Elf_Internal_Verdef    ent;
		Elf_External_Verdaux * eaux;
		Elf_Internal_Verdaux   aux;
		int                    j;
		int                    isum;

		vstart = ((char *) edefs) + idx;

		edef = (Elf_External_Verdef *) vstart;

		ent.vd_version = BYTE_GET (edef->vd_version);
		ent.vd_flags   = BYTE_GET (edef->vd_flags);
		ent.vd_ndx     = BYTE_GET (edef->vd_ndx);
		ent.vd_cnt     = BYTE_GET (edef->vd_cnt);
		ent.vd_hash    = BYTE_GET (edef->vd_hash);
		ent.vd_aux     = BYTE_GET (edef->vd_aux);
		ent.vd_next    = BYTE_GET (edef->vd_next);

		printf (_("  %#06x: Rev: %d  Flags: %s"),
			idx, ent.vd_version, get_ver_flags (ent.vd_flags));

		printf (_("  Index: %ld  Cnt: %ld  "), ent.vd_ndx, ent.vd_cnt);

		vstart += ent.vd_aux;

		eaux = (Elf_External_Verdaux *) vstart;

		aux.vda_name = BYTE_GET (eaux->vda_name);
		aux.vda_next = BYTE_GET (eaux->vda_next);

		if (dynamic_strings)
		  printf (_("Name: %s\n"), dynamic_strings + aux.vda_name);
		else
		  printf (_("Name index: %ld\n"), aux.vda_name);

		isum = idx + ent.vd_aux;

		for (j = 1; j < ent.vd_cnt; j ++)
		  {
		    isum   += aux.vda_next;
		    vstart += aux.vda_next;

		    eaux = (Elf_External_Verdaux *) vstart;

		    aux.vda_name = BYTE_GET (eaux->vda_name);
		    aux.vda_next = BYTE_GET (eaux->vda_next);

		    if (dynamic_strings)
		      printf (_("  %#06x: Parent %d: %s\n"),
			      isum, j, dynamic_strings + aux.vda_name);
		    else
		      printf (_("  %#06x: Parent %d, name index: %ld\n"),
			      isum, j, aux.vda_name);
		  }

		idx += ent.vd_next;
	      }

	    free (edefs);
	  }
	break;

	case SHT_GNU_verneed:
	  {
	    Elf_External_Verneed *  eneed;
	    unsigned int            idx;
	    unsigned int            cnt;

	    found = 1;

	    printf (_("\nVersion needs section '%s' contains %d entries:\n"),
		    SECTION_NAME (section), section->sh_info);

	    printf
	      (_(" Addr: %#08x  Offset: %#08x  Link to section: %d (%s)\n"),
	       section->sh_addr, section->sh_offset, section->sh_link,
	       SECTION_NAME (section_headers + section->sh_link));

	    GET_DATA_ALLOC (section->sh_offset, section->sh_size,
			    eneed, Elf_External_Verneed *,
			    "version need section");

	    for (idx = cnt = 0; cnt < section->sh_info; ++cnt)
	      {
		Elf_External_Verneed * entry;
		Elf_Internal_Verneed     ent;
		int                      j;
		int                      isum;
		char *                   vstart;

		vstart = ((char *) eneed) + idx;

		entry = (Elf_External_Verneed *) vstart;

		ent.vn_version = BYTE_GET (entry->vn_version);
		ent.vn_cnt     = BYTE_GET (entry->vn_cnt);
		ent.vn_file    = BYTE_GET (entry->vn_file);
		ent.vn_aux     = BYTE_GET (entry->vn_aux);
		ent.vn_next    = BYTE_GET (entry->vn_next);

		printf (_("  %#06x: Version: %d"), idx, ent.vn_version);

		if (dynamic_strings)
		  printf (_("  File: %s"), dynamic_strings + ent.vn_file);
		else
		  printf (_("  File: %lx"), ent.vn_file);

		printf (_("  Cnt: %d\n"), ent.vn_cnt);

		vstart += ent.vn_aux;

		for (j = 0, isum = idx + ent.vn_aux; j < ent.vn_cnt; ++j)
		  {
		    Elf_External_Vernaux * eaux;
		    Elf_Internal_Vernaux   aux;

		    eaux = (Elf_External_Vernaux *) vstart;

		    aux.vna_hash  = BYTE_GET (eaux->vna_hash);
		    aux.vna_flags = BYTE_GET (eaux->vna_flags);
		    aux.vna_other = BYTE_GET (eaux->vna_other);
		    aux.vna_name  = BYTE_GET (eaux->vna_name);
		    aux.vna_next  = BYTE_GET (eaux->vna_next);

		    if (dynamic_strings)
		      printf (_("  %#06x: Name: %s"),
			      isum, dynamic_strings + aux.vna_name);
		    else
		      printf (_("  %#06x: Name index: %lx"),
			      isum, aux.vna_name);

		    printf (_("  Flags: %s  Version: %d\n"),
			    get_ver_flags (aux.vna_flags), aux.vna_other);

		    isum   += aux.vna_next;
		    vstart += aux.vna_next;
		  }

		idx += ent.vn_next;
	      }

	    free (eneed);
	  }
	break;

	case SHT_GNU_versym:
	  {
	    Elf32_Internal_Shdr *       link_section;
	    int              		total;
	    int              		cnt;
	    unsigned char * 		edata;
	    unsigned short * 		data;
	    char *           		strtab;
	    Elf_Internal_Sym * 		symbols;
	    Elf32_Internal_Shdr *       string_sec;

	    link_section = section_headers + section->sh_link;
	    total = section->sh_size / section->sh_entsize;

	    found = 1;

	    symbols = get_elf_symbols
	      (file, link_section->sh_offset,
	       link_section->sh_size / link_section->sh_entsize);

	    string_sec = section_headers + link_section->sh_link;

	    GET_DATA_ALLOC (string_sec->sh_offset, string_sec->sh_size,
			    strtab, char *, "version string table");

	    printf (_("\nVersion symbols section '%s' contains %d entries:\n"),
		    SECTION_NAME (section), total);

	    printf (_(" Addr: %#08x  Offset: %#08x  Link: %x (%s)\n"),
		    section->sh_addr, section->sh_offset, section->sh_link,
		    SECTION_NAME (link_section));

	    GET_DATA_ALLOC (version_info [DT_VERSIONTAGIDX (DT_VERSYM)]
			    - loadaddr,
			    total * sizeof (short), edata,
			    char *, "version symbol data");

	    data = (unsigned short *) malloc (total * sizeof (short));

	    for (cnt = total; cnt --;)
	      data [cnt] = byte_get (edata + cnt * sizeof (short), sizeof (short));

	    free (edata);

	    for (cnt = 0; cnt < total; cnt += 4)
	      {
		int j, nn;

		printf ("  %03x:", cnt);

		for (j = 0; (j < 4) && (cnt + j) < total; ++j)
		  switch (data [cnt + j])
		    {
		    case 0:
		      fputs (_("   0 (*local*)    "), stdout);
		      break;

		    case 1:
		      fputs (_("   1 (*global*)   "), stdout);
		      break;

		    default:
		      nn = printf ("%4x%c", data [cnt + j] & 0x7fff,
				   data [cnt + j] & 0x8000 ? 'h' : ' ');

		      if (symbols [cnt + j].st_shndx < SHN_LORESERVE
			  && section_headers[symbols [cnt + j].st_shndx].sh_type
			  == SHT_NOBITS)
			{
			  /* We must test both.  */
			  Elf_Internal_Verneed     ivn;
			  unsigned long            offset;

			  offset = version_info [DT_VERSIONTAGIDX (DT_VERNEED)]
			    - loadaddr;

			  do
			    {
			      Elf_External_Verneed   evn;
			      Elf_External_Vernaux   evna;
			      Elf_Internal_Vernaux     ivna;
			      unsigned long            vna_off;

			      GET_DATA (offset, evn, "version need");

			      ivn.vn_aux  = BYTE_GET (evn.vn_aux);
			      ivn.vn_next = BYTE_GET (evn.vn_next);

			      vna_off = offset + ivn.vn_aux;

			      do
				{
				  GET_DATA (vna_off, evna,
					    "version need aux (1)");

				  ivna.vna_next  = BYTE_GET (evna.vna_next);
				  ivna.vna_other = BYTE_GET (evna.vna_other);

				  vna_off += ivna.vna_next;
				}
			      while (ivna.vna_other != data [cnt + j]
				     && ivna.vna_next != 0);

			      if (ivna.vna_other == data [cnt + j])
				{
				  ivna.vna_name = BYTE_GET (evna.vna_name);

				  nn += printf ("(%s%-*s",
						strtab + ivna.vna_name,
						12 - strlen (strtab
							     + ivna.vna_name),
						")");
				  break;
				}
			      else if (ivn.vn_next == 0)
				{
				  if (data [cnt + j] != 0x8001)
				    {
				      Elf_Internal_Verdef  ivd;
				      Elf_External_Verdef  evd;

				      offset = version_info
					[DT_VERSIONTAGIDX (DT_VERDEF)]
					- loadaddr;

				      do
					{
					  GET_DATA (offset, evd,
						    "version definition");

					  ivd.vd_next = BYTE_GET (evd.vd_next);
					  ivd.vd_ndx  = BYTE_GET (evd.vd_ndx);

					  offset += ivd.vd_next;
					}
				      while (ivd.vd_ndx
					     != (data [cnt + j] & 0x7fff)
					     && ivd.vd_next != 0);

				      if (ivd.vd_ndx
					  == (data [cnt + j] & 0x7fff))
					{
					  Elf_External_Verdaux  evda;
					  Elf_Internal_Verdaux  ivda;

					  ivd.vd_aux = BYTE_GET (evd.vd_aux);

					  GET_DATA (offset + ivd.vd_aux, evda,
						    "version definition aux");

					  ivda.vda_name =
					    BYTE_GET (evda.vda_name);

					  nn +=
					    printf ("(%s%-*s",
						    strtab + ivda.vda_name,
						    12
						    - strlen (strtab
							      + ivda.vda_name),
						    ")");
					}
				    }

				  break;
				}
			      else
				offset += ivn.vn_next;
			    }
			  while (ivn.vn_next);
			}
		      else if (symbols [cnt + j].st_shndx == SHN_UNDEF)
			{
			  Elf_Internal_Verneed     ivn;
			  unsigned long            offset;

			  offset = version_info [DT_VERSIONTAGIDX (DT_VERNEED)]
			    - loadaddr;

		          do
			    {
			      Elf_Internal_Vernaux     ivna;
			      Elf_External_Verneed   evn;
			      Elf_External_Vernaux   evna;
			      unsigned long            a_off;

			      GET_DATA (offset, evn, "version need");

			      ivn.vn_aux  = BYTE_GET (evn.vn_aux);
			      ivn.vn_next = BYTE_GET (evn.vn_next);

			      a_off = offset + ivn.vn_aux;

			      do
				{
				  GET_DATA (a_off, evna,
					    "version need aux (2)");

				  ivna.vna_next  = BYTE_GET (evna.vna_next);
				  ivna.vna_other = BYTE_GET (evna.vna_other);

				  a_off += ivna.vna_next;
				}
			      while (ivna.vna_other != data [cnt + j]
				     && ivna.vna_next != 0);

			      if (ivna.vna_other == data [cnt + j])
				{
				  ivna.vna_name = BYTE_GET (evna.vna_name);

				  nn += printf ("(%s%-*s",
						strtab + ivna.vna_name,
						12 - strlen (strtab
							     + ivna.vna_name),
						")");
				  break;
				}

			      offset += ivn.vn_next;
			    }
			  while (ivn.vn_next);
			}
		      else if (data [cnt + j] != 0x8001)
			{
			  Elf_Internal_Verdef  ivd;
			  Elf_External_Verdef  evd;
			  unsigned long        offset;

			  offset = version_info
			    [DT_VERSIONTAGIDX (DT_VERDEF)] - loadaddr;

			  do
			    {
			      GET_DATA (offset, evd, "version def");

			      ivd.vd_next = BYTE_GET (evd.vd_next);
			      ivd.vd_ndx  = BYTE_GET (evd.vd_ndx);

			      offset += ivd.vd_next;
			    }
			  while (ivd.vd_ndx != (data [cnt + j] & 0x7fff)
				 && ivd.vd_next != 0);

			  if (ivd.vd_ndx == (data [cnt + j] & 0x7fff))
			    {
			      Elf_External_Verdaux  evda;
			      Elf_Internal_Verdaux  ivda;

			      ivd.vd_aux = BYTE_GET (evd.vd_aux);

			      GET_DATA (offset - ivd.vd_next + ivd.vd_aux,
					evda, "version def aux");

			      ivda.vda_name = BYTE_GET (evda.vda_name);

			      nn += printf ("(%s%-*s",
					    strtab + ivda.vda_name,
					    12 - strlen (strtab
							 + ivda.vda_name),
					    ")");
			    }
			}

		      if (nn < 18)
			printf ("%*c", 18 - nn, ' ');
		    }

		putchar ('\n');
	      }

	    free (data);
	    free (strtab);
	    free (symbols);
	  }
	break;

	default:
	break;
	}
    }

  if (! found)
    printf (_("\nNo version information found in this file.\n"));

  return 1;
}

static char *
get_symbol_binding (binding)
     unsigned int binding;
{
  static char buff [32];

  switch (binding)
    {
    case STB_LOCAL:  return _("LOCAL");
    case STB_GLOBAL: return _("GLOBAL");
    case STB_WEAK:   return _("WEAK");
    default:
      if (binding >= STB_LOPROC && binding <= STB_HIPROC)
	sprintf (buff, _("<processor specific>: %d"), binding);
      else
	sprintf (buff, _("<unknown>: %d"), binding);
      return buff;
    }
}

static char *
get_symbol_type (type)
     unsigned int type;
{
  static char buff [32];

  switch (type)
    {
    case STT_NOTYPE:   return _("NOTYPE");
    case STT_OBJECT:   return _("OBJECT");
    case STT_FUNC:     return _("FUNC");
    case STT_SECTION:  return _("SECTION");
    case STT_FILE:     return _("FILE");
    default:
      if (type >= STT_LOPROC && type <= STT_HIPROC)
	sprintf (buff, _("<processor specific>: %d"), type);
      else
	sprintf (buff, _("<unknown>: %d"), type);
      return buff;
    }
}

static char *
get_symbol_index_type (type)
     unsigned int type;
{
  switch (type)
    {
    case SHN_UNDEF:  return "UND";
    case SHN_ABS:    return "ABS";
    case SHN_COMMON: return "COM";
    default:
      if (type >= SHN_LOPROC && type <= SHN_HIPROC)
	return "PRC";
      else if (type >= SHN_LORESERVE && type <= SHN_HIRESERVE)
	return "RSV";
      else
	{
	  static char buff [32];

	  sprintf (buff, "%3d", type);
	  return buff;
	}
    }
}


static int *
get_dynamic_data (file, number)
     FILE *       file;
     unsigned int number;
{
  char * e_data;
  int *  i_data;

  e_data = (char *) malloc (number * 4);

  if (e_data == NULL)
    {
      error (_("Out of memory\n"));
      return NULL;
    }

  if (fread (e_data, 4, number, file) != number)
    {
      error (_("Unable to read in dynamic data\n"));
      return NULL;
    }

  i_data = (int *) malloc (number * sizeof (* i_data));

  if (i_data == NULL)
    {
      error (_("Out of memory\n"));
      free (e_data);
      return NULL;
    }

  while (number--)
    i_data [number] = byte_get (e_data + number * 4, 4);

  free (e_data);

  return i_data;
}

/* Dump the symbol table */
static int
process_symbol_table (file)
     FILE * file;
{
  Elf32_Internal_Shdr *   section;

  if (! do_syms)
    return 1;

  if (dynamic_info [DT_HASH] && do_using_dynamic && dynamic_strings != NULL)
    {
      char   nb [4];
      char   nc [4];
      int    nbuckets;
      int    nchains;
      int *  buckets;
      int *  chains;
      int    hn;
      int    si;

      if (fseek (file, dynamic_info [DT_HASH] - loadaddr, SEEK_SET))
	{
	  error (_("Unable to seek to start of dynamic information"));
	  return 0;
	}

      if (fread (& nb, sizeof (nb), 1, file) != 1)
	{
	  error (_("Failed to read in number of buckets\n"));
	  return 0;
	}

      if (fread (& nc, sizeof (nc), 1, file) != 1)
	{
	  error (_("Failed to read in number of chains\n"));
	  return 0;
	}

      nbuckets = byte_get (nb, 4);
      nchains  = byte_get (nc, 4);

      buckets = get_dynamic_data (file, nbuckets);
      chains  = get_dynamic_data (file, nchains);

      if (buckets == NULL || chains == NULL)
	return 0;

      printf (_("\nSymbol table for image:\n"));
      printf (_("  Num Buc:    Value  Size   Type   Bind Ot Ndx Name\n"));

      for (hn = 0; hn < nbuckets; hn++)
	{
	  if (! buckets [hn])
	    continue;

	  for (si = buckets [hn]; si; si = chains [si])
	    {
	      Elf_Internal_Sym * psym;

	      psym = dynamic_symbols + si;

	      printf ("  %3d %3d: %8lx %5ld %6s %6s %2d ",
		      si, hn,
		      (unsigned long) psym->st_value,
		      (unsigned long) psym->st_size,
		      get_symbol_type (ELF_ST_TYPE (psym->st_info)),
		      get_symbol_binding (ELF_ST_BIND (psym->st_info)),
		      psym->st_other);

	      printf ("%3.3s", get_symbol_index_type (psym->st_shndx));

	      printf (" %s\n", dynamic_strings + psym->st_name);
	    }
	}

      free (buckets);
      free (chains);
    }
  else if (!do_using_dynamic)
    {
      unsigned int     i;

      for (i = 0, section = section_headers;
	   i < elf_header.e_shnum;
	   i++, section++)
	{
	  unsigned int          si;
	  char *                strtab;
	  Elf_Internal_Sym *    symtab;
	  Elf_Internal_Sym *    psym;


	  if (   section->sh_type != SHT_SYMTAB
	      && section->sh_type != SHT_DYNSYM)
	    continue;

	  printf (_("\nSymbol table '%s' contains %d entries:\n"),
		  SECTION_NAME (section),
		  section->sh_size / section->sh_entsize);
	  fputs (_("  Num:    Value  Size Type    Bind   Ot Ndx Name\n"),
		 stdout);

	  symtab = get_elf_symbols (file, section->sh_offset,
				    section->sh_size / section->sh_entsize);
	  if (symtab == NULL)
	    continue;

	  if (section->sh_link == elf_header.e_shstrndx)
	    strtab = string_table;
	  else
	    {
	      Elf32_Internal_Shdr * string_sec;

	      string_sec = section_headers + section->sh_link;

	      GET_DATA_ALLOC (string_sec->sh_offset, string_sec->sh_size,
			      strtab, char *, "string table");
	    }

	  for (si = 0, psym = symtab;
	       si < section->sh_size / section->sh_entsize;
	       si ++, psym ++)
	    {
	      printf ("  %3d: %8lx %5ld %-7s %-6s %2d ",
		      si,
		      (unsigned long) psym->st_value,
		      (unsigned long) psym->st_size,
		      get_symbol_type (ELF_ST_TYPE (psym->st_info)),
		      get_symbol_binding (ELF_ST_BIND (psym->st_info)),
		      psym->st_other);

	      if (psym->st_shndx == 0)
		fputs ("UND", stdout);
	      else if ((psym->st_shndx & 0xffff) == 0xfff1)
		fputs ("ABS", stdout);
	      else if ((psym->st_shndx & 0xffff) == 0xfff2)
		fputs ("COM", stdout);
	      else
		printf ("%3d", psym->st_shndx);

	      printf (" %s", strtab + psym->st_name);

	      if (section->sh_type == SHT_DYNSYM &&
		  version_info [DT_VERSIONTAGIDX (DT_VERSYM)] != 0)
		{
		  unsigned char   data[2];
		  unsigned short  vers_data;
		  unsigned long   offset;
		  int             is_nobits;
		  int             check_def;

		  offset = version_info [DT_VERSIONTAGIDX (DT_VERSYM)]
		    - loadaddr;

		  GET_DATA (offset + si * sizeof (vers_data), data,
			    "version data");

		  vers_data = byte_get (data, 2);

		  is_nobits = psym->st_shndx < SHN_LORESERVE ?
		    (section_headers [psym->st_shndx].sh_type == SHT_NOBITS)
		    : 0;

		  check_def = (psym->st_shndx != SHN_UNDEF);

		  if ((vers_data & 0x8000) || vers_data > 1)
		    {
		      if (is_nobits || ! check_def)
			{
			  Elf_External_Verneed  evn;
			  Elf_Internal_Verneed  ivn;
			  Elf_Internal_Vernaux  ivna;

			  /* We must test both.  */
			  offset = version_info
			    [DT_VERSIONTAGIDX (DT_VERNEED)] - loadaddr;

			  GET_DATA (offset, evn, "version need");

			  ivn.vn_aux  = BYTE_GET (evn.vn_aux);
			  ivn.vn_next = BYTE_GET (evn.vn_next);

			  do
			    {
			      unsigned long  vna_off;

			      vna_off = offset + ivn.vn_aux;

			      do
				{
				  Elf_External_Vernaux  evna;

				  GET_DATA (vna_off, evna,
					    "version need aux (3)");

				  ivna.vna_other = BYTE_GET (evna.vna_other);
				  ivna.vna_next  = BYTE_GET (evna.vna_next);
				  ivna.vna_name  = BYTE_GET (evna.vna_name);

				  vna_off += ivna.vna_next;
				}
			      while (ivna.vna_other != vers_data
				     && ivna.vna_next != 0);

			      if (ivna.vna_other == vers_data)
				break;

			      offset += ivn.vn_next;
			    }
			  while (ivn.vn_next != 0);

			  if (ivna.vna_other == vers_data)
			    {
			      printf ("@%s (%d)",
				      strtab + ivna.vna_name, ivna.vna_other);
			      check_def = 0;
			    }
			  else if (! is_nobits)
			    error (_("bad dynamic symbol"));
			  else
			    check_def = 1;
			}

		      if (check_def)
			{
			  if (vers_data != 0x8001)
			    {
			      Elf_Internal_Verdef     ivd;
			      Elf_Internal_Verdaux    ivda;
			      Elf_External_Verdaux  evda;
			      unsigned long           offset;

			      offset =
				version_info [DT_VERSIONTAGIDX (DT_VERDEF)]
				- loadaddr;

			      do
				{
				  Elf_External_Verdef   evd;

				  GET_DATA (offset, evd, "version def");

				  ivd.vd_ndx  = BYTE_GET (evd.vd_ndx);
				  ivd.vd_aux  = BYTE_GET (evd.vd_aux);
				  ivd.vd_next = BYTE_GET (evd.vd_next);

				  offset += ivd.vd_next;
				}
			      while (ivd.vd_ndx != (vers_data & 0x7fff)
				     && ivd.vd_next != 0);

			      offset -= ivd.vd_next;
			      offset += ivd.vd_aux;

			      GET_DATA (offset, evda, "version def aux");

			      ivda.vda_name = BYTE_GET (evda.vda_name);

			      if (psym->st_name != ivda.vda_name)
				printf ((vers_data & 0x8000)
					? "@%s" : "@@%s",
					strtab + ivda.vda_name);
			    }
			}
		    }
		}

	      putchar ('\n');
	    }

	  free (symtab);
	  if (strtab != string_table)
	    free (strtab);
	}
    }
  else
    printf
      (_("\nDynamic symbol information is not available for displaying symbols.\n"));

  return 1;
}

static int
process_section_contents (file)
     FILE * file;
{
  Elf32_Internal_Shdr *    section;
  unsigned int  i;

  if (! do_dump)
    return 1;

  for (i = 0, section = section_headers;
       i < elf_header.e_shnum;
       i ++, section ++)
    {
#ifdef SUPPORT_DISASSEMBLY
      /* See if we need an assembly dump of this section */

      if ((i < NUM_DUMP_SECTS) && (dump_sects[i] & DISASS_DUMP))
	{
	  printf (_("\nAssembly dump of section %s\n"),
		  SECTION_NAME (section));

	  /* XXX -- to be done --- XXX */
	}
#endif
      /* See if we need a hex dump of this section.  */
      if ((i < NUM_DUMP_SECTS) && (dump_sects[i] & HEX_DUMP))
	{
	  int             bytes;
	  int             addr;
	  unsigned char * data;
	  char *          start;

	  printf (_("\nHex dump of section '%s':\n"), SECTION_NAME (section));

	  bytes = section->sh_size;
	  addr  = section->sh_addr;

	  GET_DATA_ALLOC (section->sh_offset, bytes, start, char *,
			  "section data");

	  data = start;

	  while (bytes)
	    {
	      int j;
	      int k;
	      int lbytes;

	      lbytes = (bytes > 16 ? 16 : bytes);

	      printf ("  0x%8.8x ", addr);

	      switch (elf_header.e_ident [EI_DATA])
		{
		case ELFDATA2LSB:
		  for (j = 15; j >= 0; j --)
		    {
		      if (j < lbytes)
			printf ("%2.2x", data [j]);
		      else
			printf ("  ");

		      if (!(j & 0x3))
			printf (" ");
		    }
		  break;

		case ELFDATA2MSB:
		  for (j = 0; j < 16; j++)
		    {
		      if (j < lbytes)
			printf ("%2.2x", data [j]);
		      else
			printf ("  ");

		      if ((j & 3) == 3)
			printf (" ");
		    }
		  break;
		}

	      for (j = 0; j < lbytes; j++)
		{
		  k = data [j];
		  if (k >= ' ' && k < 0x80)
		    printf ("%c", k);
		  else
		    printf (".");
		}

	      putchar ('\n');

	      data  += lbytes;
	      addr  += lbytes;
	      bytes -= lbytes;
	    }

	  free (start);
	}
    }

  return 1;
}

static int
get_file_header (file)
     FILE * file;
{
  Elf32_External_Ehdr ehdr;

  if (fread (& ehdr, sizeof (ehdr), 1, file) != 1)
    return 0;

  memcpy (elf_header.e_ident, ehdr.e_ident, EI_NIDENT);

  if (elf_header.e_ident [EI_DATA] == ELFDATA2LSB)
    byte_get = byte_get_little_endian;
  else
    byte_get = byte_get_big_endian;

  elf_header.e_entry     = BYTE_GET (ehdr.e_entry);
  elf_header.e_phoff     = BYTE_GET (ehdr.e_phoff);
  elf_header.e_shoff     = BYTE_GET (ehdr.e_shoff);
  elf_header.e_version   = BYTE_GET (ehdr.e_version);
  elf_header.e_flags     = BYTE_GET (ehdr.e_flags);
  elf_header.e_type      = BYTE_GET (ehdr.e_type);
  elf_header.e_machine   = BYTE_GET (ehdr.e_machine);
  elf_header.e_ehsize    = BYTE_GET (ehdr.e_ehsize);
  elf_header.e_phentsize = BYTE_GET (ehdr.e_phentsize);
  elf_header.e_phnum     = BYTE_GET (ehdr.e_phnum);
  elf_header.e_shentsize = BYTE_GET (ehdr.e_shentsize);
  elf_header.e_shnum     = BYTE_GET (ehdr.e_shnum);
  elf_header.e_shstrndx  = BYTE_GET (ehdr.e_shstrndx);

  return 1;
}

static void
process_file (file_name)
     char * file_name;
{
  FILE *       file;
  struct stat  statbuf;
  unsigned int i;

  if (stat (file_name, & statbuf) < 0)
    {
      error (_("Cannot stat input file %s.\n"), file_name);
      return;
    }

  file = fopen (file_name, "rb");
  if (file == NULL)
    {
      error (_("Input file %s not found.\n"), file_name);
      return;
    }

  if (! get_file_header (file))
    {
      error (_("%s: Failed to read file header\n"), file_name);
      fclose (file);
      return;
    }

  /* Initialise per file variables.  */
  for (i = NUM_ELEM (version_info); i--;)
    version_info [i] = 0;

  for (i = NUM_ELEM (dynamic_info); i--;)
    dynamic_info [i] = 0;


  /* Process the file.  */
  if (show_name)
    printf (_("\nFile: %s\n"), file_name);

  if (! process_file_header ())
    {
      fclose (file);
      return;
    }

  process_section_headers (file);

  process_program_headers (file);

  process_dynamic_segment (file);

  process_relocs (file);

  process_symbol_table (file);

  process_version_sections (file);

  process_section_contents (file);

  fclose (file);

  if (section_headers)
    {
      free (section_headers);
      section_headers = NULL;
    }

  if (string_table)
    {
      free (string_table);
      string_table = NULL;
    }

  if (dynamic_strings)
    {
      free (dynamic_strings);
      dynamic_strings = NULL;
    }

  if (dynamic_symbols)
    {
      free (dynamic_symbols);
      dynamic_symbols = NULL;
    }
}

#ifdef SUPPORT_DISASSEMBLY
/* Needed by the i386 disassembler.  For extra credit, someone could
fix this so that we insert symbolic addresses here, esp for GOT/PLT
symbols */

void
print_address (unsigned int addr, FILE * outfile)
{
  fprintf (outfile,"0x%8.8x", addr);
}

/* Needed by the i386 disassembler. */
void
db_task_printsym (unsigned int addr)
{
  print_address (addr, stderr);
}
#endif

int
main (argc, argv)
     int     argc;
     char ** argv;
{
  parse_args (argc, argv);

  if (optind < (argc - 1))
    show_name = 1;

  while (optind < argc)
    process_file (argv [optind ++]);

  return 0;
}
