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

#include "bfd.h"
#include "bucomm.h"

#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "readelf.h"
#include "getopt.h"

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif

unsigned int dynamic_addr;
unsigned int dynamic_size;
char * pint = "";
char * program_name = "readelf";

int dynamic_info [DT_JMPREL + 1];
int version_info [16];

int must_swap = 0;

unsigned int rel_size;
int loadaddr = -1;

unsigned int rela_addr;
unsigned int rela_size;
char * 		strtab;
int 		symtab_index;
int          	lastmapped;
char *          header;

Elf_Dyn * 	dpnt;
Elf_Rel * 	rpnt;
Elf_Shdr * 	elf_sections;
Elf_Ehdr * 		epnt;
Elf_Sym * 	symtab;

int show_name;
int do_dynamic;
int do_syms;
int do_reloc;
int do_section;
int do_load;
int do_using_dynamic;
int do_header;
int do_dump;
int do_version;
long int expected_endian;

char * dyntype[] =
{
  "NULL", "NEEDED","PLTRELSZ","PLTGOT","HASH","STRTAB","SYMTAB","RELA",
  "RELASZ","RELAENT","STRSZ","SYMENT","INIT","FINI","SONAME","RPATH",
  "SYMBOLIC","REL","RELSZ","RELENT","PLTREL","DEBUG","TEXTREL","JMPREL"
};

char * vertype[] =
{
  "VERNEEDNUM", "VERNEED", "VERDEFNUM", "VERDEF",
  "", "", "", "", "", "", "", "", "", "", "", "VERSYM"
};

char * filtertype[] =
{
  "FILTER", "USED", "AUXILIARY"
};


char * sttinfo[] = {"NOTYPE","OBJECT","FUNC","SECTION","FILE"};
char * stbinfo[] = {"LOCAL","GLOBAL","WEAK"};

#define SECTION_NAME(X) (& header [lastmapped + (X)->sh_name])

#define NUM_DUMP_SECTS	100
char dump_sects [NUM_DUMP_SECTS];
#define HEX_DUMP	1
#define DISASS_DUMP	2

/* Forward declarations for dumb compilers.  */
static char * get_i386_rel_type    PARAMS ((bfd_vma rtype));
static char * get_m68k_rel_type    PARAMS ((bfd_vma rtype));
static char * get_sparc_rel_type   PARAMS ((bfd_vma rtype));
static char * get_m32r_rel_type    PARAMS ((bfd_vma rtype));
static char * get_v850_rel_type    PARAMS ((bfd_vma rtype));
static char * get_d10v_rel_type    PARAMS ((bfd_vma rtype));
/* start-sanitize-d30v */
static char * get_d30v_rel_type    PARAMS ((bfd_vma rtype));
/* end-sanitize-d30v */
static char * get_sh_rel_type      PARAMS ((bfd_vma rtype));
static char * get_mn10300_rel_type PARAMS ((bfd_vma rtype));
static char * get_mn10200_rel_type PARAMS ((bfd_vma rtype));
static char * get_ppc_rel_type	   PARAMS ((bfd_vma rtype));
static void   dump_relocations PARAMS ((Elf_Rel * rpnt, int rel_size));
static char * get_file_type PARAMS ((unsigned e_type));
static char * get_machine_name PARAMS ((unsigned e_machine));
static char * get_machine_data PARAMS ((unsigned e_data));
static char * get_machine_flags PARAMS ((unsigned e_flags, unsigned e_machine));
static char * get_segment_type PARAMS ((unsigned long p_type));
static char * get_section_type_name PARAMS ((unsigned int sh_type));
static void   usage PARAMS ((void));
static void   parse_args PARAMS ((int argc, char ** argv));
static int    process_elf_header PARAMS ((void));
static void   process_program_headers PARAMS ((void));
static void   process_section_headers PARAMS ((void));
static void   process_dynamic_segment PARAMS ((void));
static void   process_symbol_table PARAMS ((void));
static void   process_section_contents PARAMS ((void));
static void   process_file PARAMS ((char * file_name));


#define SWAP2(val)  (  (((val) << 8) & (0xff << 8)) \
		     | (((val) >> 8) & (0xff << 0)))
     
#define SWAP4(val)  (  (((val) << 24) & (0xff << 24)) \
		     | (((val) <<  8) & (0xff << 16)) \
		     | (((val) >>  8) & (0xff <<  8)) \
		     | (((val) >> 24) & (0xff <<  0)))
		       
/* Warning: This macro assumes 8 bits in a char.  */
#define BYTE_SWAP(pointer, field) 				\
  if (sizeof ((pointer)->field) == 2)				\
    {								\
      unsigned short val = (pointer)->field ;			\
      new_header->field = SWAP2 (val);				\
    }								\
  else if (sizeof ((pointer)->field) != 4)			\
    abort ();							\
  else								\
   {								\
      unsigned long val = (pointer)->field ;			\
      new_header->field = SWAP4 (val);				\
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
     va_dcl;
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


static char *
get_i386_rel_type (rtype)
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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
     bfd_vma rtype;
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


static void
dump_relocations (rpnt, rel_size)
     Elf_Rel * rpnt;
     int       rel_size;
{
  int          i;
  int          is_rela;
  Elf_Rela * rapnt;
  Elf_Rela * relocs = NULL;

  
  rapnt = (Elf_Rela *) rpnt;

  /* Compute number of relocations.  */
  switch (epnt->e_machine)
    {
    case EM_386:
    case EM_486:
    case EM_CYGNUS_M32R:
    case EM_CYGNUS_D10V:
      rel_size = rel_size / sizeof (Elf_Rel);
      
      if (must_swap)
	{
	  Elf_Rel * new_header = malloc (sizeof (* new_header) * rel_size);

	  if (new_header == NULL)
	    {
	      error (_("out of memory\n"));
	      return;
	    }
	  
	  memcpy (new_header, rpnt, sizeof (* new_header) * rel_size);
      
	  rpnt   = new_header;
	  relocs = (Elf_Rela *) new_header;
      
	  for (i = 0; i < rel_size; i++)
	    {
	      BYTE_SWAP (rpnt + i, r_offset);
	      BYTE_SWAP (rpnt + i, r_info);
	      
	      new_header ++;
	    }
	}

      is_rela = 0;
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
      rel_size = rel_size / sizeof (Elf_Rela);
      
      if (must_swap)
	{
	  Elf_Rela * new_header = malloc (sizeof (* new_header) * rel_size);

	  if (new_header == NULL)
	    {
	      error (_("out of memory\n"));
	      return;
	    }
	  
	  memcpy (new_header, rpnt, sizeof (* new_header) * rel_size);
      
	  relocs = rapnt = new_header;
	  
	  for (i = rel_size; i--;)
	    {
	      BYTE_SWAP (new_header, r_offset);
	      BYTE_SWAP (new_header, r_info);
	      BYTE_SWAP (new_header, r_addend);
	      
	      new_header ++;
	    }
	}
      
      is_rela = 1;
      break;
      
    default:
      warn (_("Don't know about relocations on this machine architecture\n"));
      return;
    }

  if (is_rela)
    printf (_("  Offset Value Type            Symbol's Value  Symbol Name  Addend\n"));
  else
    printf (_("  Offset Value Type            Symbol's Value  Symbol Name\n"));
  
  for (i = 0; i < rel_size; i++)
    {
      char * rtype;
      
      if (is_rela)
	rpnt = (Elf_Rel *) rapnt;
      
      printf ("  %5.5lx  %5.5lx ", rpnt->r_offset, rpnt->r_info);
	     
      switch (epnt->e_machine)
	{
	default:
	  rtype = "UGG!";
	  break;
	  
	case EM_CYGNUS_M32R:
	  rtype = get_m32r_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_386:
	case EM_486:
	  rtype = get_i386_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_68K:
	  rtype = get_m68k_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_SPARC:
	  rtype = get_sparc_rel_type (ELF32_R_TYPE (rapnt->r_info));
	  break;
	  
	case EM_CYGNUS_V850:
	  rtype = get_v850_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_CYGNUS_D10V:
	  rtype = get_d10v_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	  /* start-sanitize-d30v */
	case EM_CYGNUS_D30V:
	  rtype = get_d30v_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	  /* end-sanitize-d30v */
	case EM_SH:
	  rtype = get_sh_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_CYGNUS_MN10300:
	  rtype = get_mn10300_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	  
	case EM_CYGNUS_MN10200:
	  rtype = get_mn10200_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;

	case EM_PPC:
	  rtype = get_ppc_rel_type (ELF32_R_TYPE (rpnt->r_info));
	  break;
	}
      
      printf ("%-18s", rtype);
      
      symtab_index = ELF32_R_SYM (rpnt->r_info);
      
      if (symtab_index)
	{
	  Elf_Sym   ssym;
	  Elf_Sym * psym;
	  
	  psym = symtab + symtab_index;
	  
	  if (must_swap)
	    {
	      Elf_Sym * new_header = & ssym;
	      
	      ssym = * psym;
	      
	      BYTE_SWAP (psym, st_name);
	      BYTE_SWAP (psym, st_value);
	      /* BYTE_SWAP (psym, st_size); */
	      BYTE_SWAP (psym, st_shndx);
	      
	      psym = new_header;
	    }

	  if (psym->st_name == 0)
	    printf ("    %08lx  %-15s", psym->st_value,
		    SECTION_NAME (elf_sections + psym->st_shndx));
	  else
	    printf ("    %08lx  %-15s", psym->st_value, strtab + psym->st_name);
	    
	  if (is_rela)
	    printf (" + %lx", rapnt->r_addend);
	}
      
      putchar ('\n');
      rapnt ++;
      rpnt ++;
    }

  if (relocs != NULL)
    free (relocs);
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
    case EM_PARISC:      	return "HPPA";
    case EM_SPARC32PLUS: 	return "Sparc v8+" ;
    case EM_PPC:         	return "PowerPC";
    case EM_SPARCV9:     	return "Sparc v9";      
    case EM_ARM:	 	return "ARM";
    case EM_SH:		 	return "Hitachi SH";
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

static char *
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
    case PT_PHDR:       return _("Program Headers");

    default:
      if ((p_type >= PT_LOPROC) && (p_type <= PT_HIPROC))
	return _("processor specific");
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
    case SHT_RELA:		return _("Relocs, addends");
    case SHT_HASH:		return _("Symbol hash table");
    case SHT_DYNAMIC:		return _("Dynamic linking info");
    case SHT_NOTE:		return _("Notes");
    case SHT_NOBITS:		return _("Space, no data");
    case SHT_REL:		return _("Relocs, no addends");
    case SHT_SHLIB:		return _("Shared Library info");
    case SHT_DYNSYM:		return _("Dynamic linker symbols");
    case SHT_GNU_verdef:	return _("Version definition");
    case SHT_GNU_verneed:	return _("Version needs");
    case SHT_GNU_versym:	return _("Version symbols");
    case 0x6ffffff0:	        return "VERSYM";
    case 0x6ffffffc:	        return "VERDEF";
    case 0x7ffffffd:		return "AUXILIARY";
    case 0x7fffffff:		return "FILTER";

    default:
      if ((sh_type >= SHT_LOPROC) && (sh_type <= SHT_HIPROC))
	return _("processor specific");
      else if ((sh_type >= SHT_LOUSER) && (sh_type <= SHT_HIUSER))
	return _("application specific");
      else
	{
	  sprintf (buff, _("<unknown>: %x"), sh_type);
	  return buff;
	}
    }
}

struct option options [] =
{
  {"all", no_argument, 0, 'a'},
  {"file-header", no_argument, 0, 'h'},
  {"program-headers", no_argument, 0, 'l'},
  {"segments", no_argument, 0, 'l'},
  {"sections", no_argument, 0, 'S'},
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
  fprintf (stdout, _("  -a or --all               Display all the information\n"));
  fprintf (stdout, _("  -h or --file-header       Display the ELF file header\n"));
  fprintf (stdout, _("  -l or --program-headers or --segments\n"));
  fprintf (stdout, _("                            Display the program headers\n"));
  fprintf (stdout, _("  -S or --sections          Display the sections' headers\n"));
  fprintf (stdout, _("  -s or --symbols           Display the symbol table\n"));
  fprintf (stdout, _("  -r or --relocs            Display the relocations (if present)\n"));
  fprintf (stdout, _("  -d or --dynamic           Display the dynamic section (if present)\n"));
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
	  (argc, argv, "rsahldSDx:i:vV", options, NULL)) != EOF)
    {
      char *    cp;
      int	section;
      
      switch (c)
	{
	case 'H':
	  usage ();
	  break;
	  
	case 'a':
	  do_syms++;
	  do_reloc++;
	  do_dynamic++;
	  do_header++;
	  do_section++;
	  do_load++;
	  do_version++;
	  break;
	case 'D':
	  do_using_dynamic++;
	  break;
	case 'r':
	  do_reloc++;
	  break;
	case 'h':
	  do_header++;
	  break;
	case 'l':
	  do_load++;
	  break;
	case 's':
	  do_syms++;
	  break;
	case 'S':
	  do_section++;
	  break;
	case 'd':
	  do_dynamic++;
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

  if (!do_dynamic && !do_syms && !do_reloc && !do_section
      && !do_load && !do_header && !do_dump && !do_version)
    usage ();
  else if (argc < 3)
    {
      warn (_("Nothing to do.\n"));
      usage();
    }
}

static int
process_elf_header ()
{
  if (   epnt->e_ident [EI_MAG0] != ELFMAG0
      || epnt->e_ident [EI_MAG1] != ELFMAG1
      || epnt->e_ident [EI_MAG2] != ELFMAG2
      || epnt->e_ident [EI_MAG3] != ELFMAG3)
    {
      error (_("Not an ELF file - it has the wrong magic bytes at the start\n"));
      return 0;
    }
    
  if (epnt->e_ident [EI_CLASS] != ELFCLASS32)
    {
      error (_("Not a 32 bit ELF file\n"));
      return 0;
    }
    
  if (epnt->e_ident [EI_DATA] != expected_endian)
    must_swap = 1;
    
  if (must_swap)
    {
      Elf_Ehdr * new_header = malloc (sizeof (* new_header));
      
      if (new_header == NULL)
	{
	  error (_("out of memory\n"));
	  return 0;
	}
      
      memcpy (new_header, epnt, sizeof (* new_header));
      
      BYTE_SWAP (epnt, e_type);
      BYTE_SWAP (epnt, e_machine);
      BYTE_SWAP (epnt, e_version);
      BYTE_SWAP (epnt, e_entry);
      BYTE_SWAP (epnt, e_phoff);
      BYTE_SWAP (epnt, e_shoff);
      BYTE_SWAP (epnt, e_flags);
      BYTE_SWAP (epnt, e_ehsize);
      BYTE_SWAP (epnt, e_phentsize);
      BYTE_SWAP (epnt, e_phnum);
      BYTE_SWAP (epnt, e_shentsize);
      BYTE_SWAP (epnt, e_shnum);
      BYTE_SWAP (epnt, e_shstrndx);
      
      epnt = new_header;
    }
    
  if (do_header)
    {
      int i;
      
      printf (_("ELF Header....\n"));
      printf (_(" Magic:   "));
      for (i = 0; i < EI_NIDENT; i ++)
	printf ("%2.2x ", epnt->e_ident [i]);
      printf ("\n"); 
      printf (_(" Type:                              %s\n"), get_file_type (epnt->e_type));
      printf (_(" Machine:                           %s\n"), get_machine_name (epnt->e_machine));
      printf (_(" Version:                           0x%lx\n"), (unsigned long)epnt->e_version);
      printf (_(" Data:                              %s\n"), get_machine_data (epnt->e_ident [EI_DATA]));
      printf (_(" Entry point address:               0x%lx\n"), (unsigned long)epnt->e_entry);
      printf (_(" Start of program headers:          %ld (bytes into file)\n"), (long)epnt->e_phoff);
      printf (_(" Start of section headers:          %ld (bytes into file)\n"), (long)epnt->e_shoff);
      printf (_(" Flags:                             0x%lx%s\n"), (unsigned long)epnt->e_flags,
	      get_machine_flags (epnt->e_flags, epnt->e_machine));
      printf (_(" Size of this header:               %ld (bytes)\n"), (long)epnt->e_ehsize);
      printf (_(" Size of program headers:           %ld (bytes)\n"), (long)epnt->e_phentsize);
      printf (_(" Number of program headers:         %ld\n"), (long)epnt->e_phnum);
      printf (_(" Size of section headers:           %ld (bytes)\n"), (long)epnt->e_shentsize);
      printf (_(" Number of section headers:         %ld\n"), (long)epnt->e_shnum);
      printf (_(" Section header string table index: %ld\n"), (long)epnt->e_shstrndx);
    }
  
  return 1;
}


static void
process_program_headers ()
{
  Elf_Phdr * elf_segments;
  Elf_Phdr * ppnt;
  int	       i;
  
  if (epnt->e_phnum == 0)
    {
      if (do_load)
	printf (_("\nThere are no program headers in this file\n"));
      return;
    }
  
  if (do_load && !do_header)
    {
      printf (_("\nElf file is %s\n"), get_file_type (epnt->e_type));
      printf (_("Entry point 0x%x\n"), epnt->e_entry);
      printf (_("There are %d program headers, starting at offset %x:\n"),
	      epnt->e_phnum, epnt->e_phoff);
    }

  if (must_swap)
    {
      Elf_Phdr * new_header = malloc (sizeof (* new_header) * epnt->e_phnum);

      if (new_header == NULL)
	{
	  error (_("out of memory\n"));
	  return;
	}
      
      memcpy (new_header, & header [epnt->e_phoff],
	      sizeof (* new_header) * epnt->e_phnum);
      
      elf_segments = ppnt = new_header;
      
      for (i = 0; i < epnt->e_phnum; i++)
	{
	  BYTE_SWAP (ppnt + i, p_type);
	  BYTE_SWAP (ppnt + i, p_flags);
	  BYTE_SWAP (ppnt + i, p_offset);
	  BYTE_SWAP (ppnt + i, p_vaddr);
	  BYTE_SWAP (ppnt + i, p_paddr);
	  BYTE_SWAP (ppnt + i, p_filesz);
	  BYTE_SWAP (ppnt + i, p_memsz);
	  BYTE_SWAP (ppnt + i, p_align);
	  
	  new_header ++;
	}
    }
  else
    {
      ppnt = (Elf_Phdr *) & header [epnt->e_phoff];
      elf_segments = NULL;
    }
    
  if (do_load)
    {
      printf (_("\nProgram Header%s....\n"), epnt->e_phnum > 1 ? "s" : "");
      printf (_(" Type        Offset  VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n"));
    }

  loadaddr = -1;
  dynamic_addr = 0;

  for (i = 0; i < epnt->e_phnum; i++)
    {
      if (loadaddr == -1 && ppnt->p_type == PT_LOAD)
	loadaddr = (ppnt->p_vaddr & 0xfffff000)
	  - (ppnt->p_offset & 0xfffff000);
      
      if (do_load)
	{
	  printf ("  %-10s ", get_segment_type (ppnt->p_type));
	  printf ("0x%5.5lx ",ppnt->p_offset);
	  printf ("0x%8.8lx ",ppnt->p_vaddr);
	  printf ("0x%8.8lx ",ppnt->p_paddr);
	  printf ("0x%5.5lx 0x%5.5lx ",ppnt->p_filesz, ppnt->p_memsz);
	  printf ("%c%c%c ",
		  (ppnt->p_flags & 4 ? 'R' : ' '),
		  (ppnt->p_flags & 2 ? 'W' : ' '),
		  (ppnt->p_flags & 1 ? 'E' : ' '));
	  printf ("%#lx", ppnt->p_align);
	}
      
      if (ppnt->p_type == PT_DYNAMIC)
	{
	  if (dynamic_addr)
	    error (_("more than one dynamic section\n"));
	  
	  dynamic_addr = ppnt->p_offset;
	  dynamic_size = ppnt->p_filesz;
	}
      
      if (ppnt->p_type == PT_INTERP)
	{
	  if (do_load)
	    printf (_("\nRequesting program interpreter [%s]"),
		    & header [ppnt->p_offset]);
	  pint = strdup (& header [ppnt->p_offset]);
	}
      
      if (do_load)
	putc ('\n', stdout);
      
      ppnt ++;
    }

  if (do_load)
    {
      printf (_("\n Section to Segment mapping:\n"));
      printf (_("  Segment Sections...\n"));

      if (elf_segments)
	ppnt = elf_segments;
      else	
	ppnt = (Elf_Phdr *) & header [epnt->e_phoff];
      
      for (i = 0; i < epnt->e_phnum; i++, ppnt++)
	{
	  int j;
	  Elf_Shdr * spnt;
	  
	  printf ("   %2.2d     ", i);
	  
	  spnt = (Elf_Shdr *) & header [epnt->e_shoff];

	  if (must_swap)
	    {
	      lastmapped = SWAP4 (spnt[epnt->e_shstrndx].sh_offset);
	      
	      for (j = 0; j < epnt->e_shnum; j++)
		{
		  bfd_vma addr; 
		  bfd_size_type size;
		  unsigned int  name;

		  addr = SWAP4 (spnt[j].sh_addr);
		  size = SWAP4 (spnt[j].sh_size);
		  name = SWAP4 (spnt[j].sh_name);
		  
		  if (size > 0
		      && (addr >= ppnt->p_vaddr)
		      && (addr + size) <= (ppnt->p_vaddr + ppnt->p_memsz))
  		    printf ("%s ", header + lastmapped + name);
		}
	    }
	  else
	    {
	      lastmapped = spnt[epnt->e_shstrndx].sh_offset;
	      
	      for (j = 0; j < epnt->e_shnum; j++, spnt++)
		{
		  if (spnt->sh_size > 0
		      && (spnt->sh_addr >= ppnt->p_vaddr)
		      && (spnt->sh_addr + spnt->sh_size)
		      <= (ppnt->p_vaddr + ppnt->p_memsz))
  		    printf ("%s ", SECTION_NAME (spnt));
		}
	    }

	  putc ('\n',stdout);
	}
    }
  
  if (elf_segments)
    free (elf_segments);
}


static void
process_section_headers ()
{
  Elf_Shdr * spnt;
  int          i;

  if (must_swap)
    {
      Elf_Shdr * new_header = malloc (sizeof (* new_header) * epnt->e_shnum);
      
      if (new_header == NULL)
	{
	  error (_("out of memory\n"));
	  return;
	}
      
      memcpy (new_header, & header [epnt->e_shoff],
	      sizeof (* new_header) * epnt->e_shnum);
      
      elf_sections = spnt = new_header;
      
      for (i = 0; i < epnt->e_shnum; i++)
	{
	  BYTE_SWAP (spnt + i, sh_name);
	  BYTE_SWAP (spnt + i, sh_type);
	  BYTE_SWAP (spnt + i, sh_flags);
	  BYTE_SWAP (spnt + i, sh_addr);
	  BYTE_SWAP (spnt + i, sh_offset);
	  BYTE_SWAP (spnt + i, sh_size);
	  BYTE_SWAP (spnt + i, sh_link);
	  BYTE_SWAP (spnt + i, sh_info);
	  BYTE_SWAP (spnt + i, sh_addralign);
	  BYTE_SWAP (spnt + i, sh_entsize);
	  
	  new_header ++;
	}
    }
  else
    {
      elf_sections = spnt = (Elf_Shdr *) & header [epnt->e_shoff];
    }
  
  spnt += epnt->e_shstrndx;
  lastmapped = spnt->sh_offset;
  spnt = elf_sections;
  
  if (! do_section || (epnt->e_shnum == 0))
    return;
  
  if (! do_header)
    printf (_("There are %d section headers, starting at offset %x:\n"),
	    epnt->e_shnum, epnt->e_shoff);
  
  printf (_("\nSection Header%s....\n"), epnt->e_shnum > 1 ? "s" : "");
  printf (_("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk In Al\n"));
      
  for (i = 0; i < epnt->e_shnum; i++)
    {
      printf ("  [%2d] %-17s", i, SECTION_NAME (spnt));
	  
      printf (" %-15s ",get_section_type_name (spnt->sh_type));
      printf ( "%8.8lx %6.6lx %6.6lx %2.2lx",
	       spnt->sh_addr,
	       spnt->sh_offset,
	       spnt->sh_size,
	       spnt->sh_entsize);
      
      printf (" %c%c%c %2ld %2lx %ld \n",
	      (spnt->sh_flags & 1 ? 'W' : ' '),
	      (spnt->sh_flags & 2 ? 'A' : ' '),
	      (spnt->sh_flags & 4 ? 'X' : ' '),
	      spnt->sh_link,
	      spnt->sh_info,
	      spnt->sh_addralign);
      spnt ++;
    }
}

/* Parse the dynamic segment */
static void
process_dynamic_segment ()
{
  Elf_Dyn * elf_dynamic;
  unsigned int i;
  
  dynamic_size = dynamic_size / sizeof (Elf_Dyn);

  if (must_swap)
    {
      Elf_Dyn * new_header = malloc (sizeof (* new_header) * dynamic_size);
      
      if (new_header == NULL)
	{
	  error (_("out of memory\n"));
	  return;
	}
      
      memcpy (new_header, & header [dynamic_addr],
	      sizeof (* new_header) * dynamic_size);
      
      elf_dynamic = dpnt = new_header;
      
      for (i = 0; i < dynamic_size; i++)
	{
	  BYTE_SWAP (dpnt + i, d_tag);
	  BYTE_SWAP (dpnt + i, d_un.d_ptr);
	  
	  new_header ++;
	}
    }
  else
    {
      dpnt = (Elf_Dyn *) & header [dynamic_addr];
      elf_dynamic = NULL;
    }

  /* Find symtab.  */
  for (i = 0; i < dynamic_size; ++i, ++dpnt)
    if (dpnt->d_tag == DT_SYMTAB)
      {
	dynamic_info [DT_SYMTAB] = dpnt->d_un.d_val;
	symtab =  (Elf_Sym *) (header - loadaddr
				 + dynamic_info[DT_SYMTAB]);
      }
    else if (dpnt->d_tag == DT_STRTAB)
      {
	dynamic_info [DT_STRTAB] = dpnt->d_un.d_val;
	strtab = (char *) (header - loadaddr + dynamic_info[DT_STRTAB]);
      }
  
  if (do_dynamic && dynamic_addr)
    {
      printf (_("\n Dynamic section data: %x, %d entries\n"),
	      dynamic_addr, dynamic_size );
    }      

  for (i = 0; i < dynamic_size; i++)
    {
      if (do_dynamic)
	printf (_("  Tag: %#10x: "), dpnt->d_tag);
      
      switch (dpnt->d_tag)
	{
	case DT_AUXILIARY:
	case DT_FILTER:
	  if (do_dynamic)
	    {
	      printf ("(%-11s)", filtertype [DT_FILTER - dpnt->d_tag]);
	      
	      if (dynamic_info [DT_STRTAB])
		{
		  if (dpnt->d_tag == DT_AUXILIARY)
		    printf (_("Auxiliary library"));
		  else
		    printf (_("Filter library"));

		  printf (": [%s]\n", (dpnt->d_un.d_val + strtab));
		}
	      else
		printf (_("Value %x\n"), dpnt->d_un.d_val);
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
	  dynamic_info [dpnt->d_tag] = dpnt->d_un.d_val;
	  
	  if (do_dynamic)
	    {
	      printf ("(%-11s)", dyntype [dpnt->d_tag]);
	      
	      if (dynamic_info [DT_STRTAB])
		{
		  switch (dpnt->d_tag)
		    {
		    case DT_NEEDED:
		      printf (_("Shared library: [%s]\n"),
			      (dpnt->d_un.d_val + strtab));
		      
		      if (strcmp (dpnt->d_un.d_val + strtab, pint))
			printf ("\n");
		      else
			printf (_(" program interpreter\n"));
		      break;

		    case DT_SONAME:
		      printf (_("Library soname: [%s]\n"),
			      (dpnt->d_un.d_val + strtab));
		      break;

		    case DT_RPATH:
		      printf (_("Library rpath: [%s]\n"),
			      (dpnt->d_un.d_val + strtab));
		      break;

		    default:
		      printf (_("Value %x\n"), dpnt->d_un.d_val);
		    }
		}
	      else
		printf (_("Value %x\n"), dpnt->d_un.d_val);
	    }
	  break;

	default:
	  if ((dpnt->d_tag >= DT_VERSYM) && (dpnt->d_tag <= DT_VERNEEDNUM))
	    {
	      version_info [DT_VERSIONTAGIDX (dpnt->d_tag)] = dpnt->d_un.d_val;

	      if (do_dynamic)
		printf (_("(%-11s) Value %#x\n"),
			vertype [DT_VERSIONTAGIDX (dpnt->d_tag)],
			dpnt->d_un.d_ptr);
	    }
	  else
	    warn (_("<Invalid>     Value %#x\n"), dpnt->d_un.d_ptr);
	  break;
	}
      
      dpnt ++;
    }

  if (do_reloc)
    {
      if (do_using_dynamic)
	{
	  if (dynamic_info [DT_REL])
	    {
	      rpnt = (Elf_Rel *) (header + dynamic_info [DT_REL] - loadaddr);
	      rel_size = dynamic_info [DT_RELSZ];
	      if (rel_size)
		{
		  printf (_("\nRelocation section data: %x %x\n"),
			  dynamic_info[DT_REL], rel_size);
		  dump_relocations (rpnt, rel_size);
		}
	      else
		printf (_("\nNo Relocations in this file\n"));
	    }
	  
	  if (dynamic_info[DT_RELA])
	    {
	      rpnt = (Elf_Rel *) (header + dynamic_info[DT_RELA] - loadaddr);
	      rel_size = dynamic_info[DT_RELASZ];
	      if (rel_size)
		{
		  printf (_("\nRelocation section data: %x %x\n"),
			  dynamic_info[DT_RELA], rel_size);
		  dump_relocations (rpnt, rel_size);
		}
	      else
		printf (_("\nNo Relocations in this file\n"));
	    }
	  
	  if (dynamic_info[DT_JMPREL])
	    {
	      rpnt = (Elf_Rel *) (header + dynamic_info[DT_JMPREL]
				    - loadaddr);
	      rel_size = dynamic_info[DT_PLTRELSZ];
	      if (rel_size)
		{
		  printf (_("\nJumptable Relocation section data: %x %x\n"),
			  dynamic_info[DT_JMPREL], rel_size);
		  dump_relocations (rpnt, rel_size);
		}
	      else
		printf (_("\nNo Relocations in this file\n"));
	    }
	}
      else
	{
	  Elf_Shdr * spnt;
	  
	  spnt = elf_sections;
	  
	  for (i = 0; i < epnt->e_shnum; i++, spnt++)
	    {
	      Elf_Shdr * symsec;

	      
	      if (spnt->sh_type != SHT_RELA && spnt->sh_type != SHT_REL)
		continue;
	      
	      rpnt = (Elf_Rel *) (header + spnt->sh_offset);
	      
	      rel_size = spnt->sh_size;

	      if (rel_size)
		{
		  printf (_("\nRelocation section data: %s (%#x entries)\n"),
			  SECTION_NAME (spnt), rel_size / spnt->sh_entsize);
		  
		  symsec = & elf_sections [spnt->sh_link];
		  symtab = (Elf_Sym *) (header + symsec->sh_offset);
		  strtab = (char *) (header
				     + elf_sections [symsec->sh_link].sh_offset);
		  
		  dump_relocations (rpnt, rel_size);
		}
	      else
		printf (_("\nNo Relocations in this file\n"));
	    }
	}
    }
  
  if (elf_dynamic)
    free (elf_dynamic);
}

/* Dump the symbol table */
static void
process_symbol_table ()
{
  char *       pnt;
  int          i;
  Elf_Shdr * spnt;
  
  if (! do_syms)
    return;
  
  if (dynamic_info [DT_HASH] && do_using_dynamic)
    {
      int   nbucket;
      int   nchain;
      int * elf_buckets;
      int * chains;
      int   hn;
      int   si;
      int * hash_addr;

      hash_addr = (int *) (dynamic_info [DT_HASH] + header - loadaddr);
      
      nbucket     = *hash_addr++;
      nchain      = *hash_addr++;
      elf_buckets = hash_addr;
      hash_addr  += nbucket;
      chains      = hash_addr;
	  
      printf (_("\n Symbol table for image\n"));
      printf (_("  Num Buc:    Value  Size   Type   Bind Ot Ndx Name\n"));
      
      for (hn = 0; hn < nbucket; hn++)
	{
	  if (! elf_buckets [hn])
	    continue;
	  
	  for (si = elf_buckets[hn]; si; si = chains[si])
	    {
	      pnt = strtab + symtab[si].st_name;
	      
	      printf ("%3d %3d: %8lx %5ld %6s %6s %2d ", si, hn,
		      symtab[si].st_value,
		      symtab[si].st_size,
		      sttinfo [ELF_ST_TYPE (symtab[si].st_info)],
		      stbinfo [ELF_ST_BIND (symtab[si].st_info)],
		      symtab[si].st_other);
	      
	      if (symtab[si].st_shndx == 0)
		printf ("UND");
	      else if ((symtab[si].st_shndx & 0xffff) == 0xfff1)
		printf ("ABS");
	      else if ((symtab[si].st_shndx & 0xffff) == 0xfff2)
		printf ("COM");
	      else
		printf ("%3d", symtab[si].st_shndx);
	      printf (" %s\n", pnt);
	    }
	}
    }
  else if (!do_using_dynamic)
    {
      int i;
      unsigned short * vers_addr;
	  
      spnt      = elf_sections;
      vers_addr = (short *) (version_info [DT_VERNEEDNUM - DT_VERSYM]
			     + header - loadaddr);

      for (i = 0; i < epnt->e_shnum; i++, spnt++)
	{
	  unsigned int si;
	  
	  if (spnt->sh_type != SHT_SYMTAB && spnt->sh_type != SHT_DYNSYM)
	    continue;
	  
	  printf (_("\nSymbol data for: %s\n"), SECTION_NAME (spnt));
	  printf (_("  Num:    Value  Size Type    Bind   Ot Ndx Name\n"));
	  
	  symtab = (Elf_Sym *) (header + spnt->sh_offset);
	  strtab = (char *) (header + elf_sections [spnt->sh_link].sh_offset);
	  
	  for (si = 0; si < spnt->sh_size / spnt->sh_entsize; si++)
	    {
	      Elf_Sym   ssym;
	      Elf_Sym * psym;
	      Elf_Sym * new_header;

	      psym = symtab + si;
	      
	      if (must_swap)
		{
		  ssym = * psym;
		  
		  new_header = & ssym;

		  BYTE_SWAP (psym, st_name);
		  BYTE_SWAP (psym, st_value);
		  BYTE_SWAP (psym, st_size);
		  BYTE_SWAP (psym, st_shndx);

		  psym = new_header;
		}
	      
	      pnt = strtab + psym->st_name;
	      
	      printf ("  %3d: %8lx %5ld %-7s %-6s %2d ", si,
		      psym->st_value,
		      psym->st_size,
		      sttinfo [ELF_ST_TYPE (psym->st_info)],
		      stbinfo [ELF_ST_BIND (psym->st_info)],
		      psym->st_other);
	      
	      if (psym->st_shndx == 0)
		printf ("UND");
	      else if ((psym->st_shndx & 0xffff) == 0xfff1)
		printf ("ABS");
	      else if ((psym->st_shndx & 0xffff) == 0xfff2)
		printf ("COM");
	      else
		printf ("%3d", psym->st_shndx);
	      printf (" %s", pnt);
	      
	      if (spnt->sh_type == SHT_DYNSYM &&
		  version_info [DT_VERSIONTAGIDX (DT_VERSYM)] != 0 &&
		  ((vers_addr[si] & 0x8000) || vers_addr[si] > 1))
		{
		  Elf_Vernaux * a;
		  
		  if (elf_sections [psym->st_shndx].sh_type == SHT_NOBITS
		      || psym->st_shndx == SHN_UNDEF)
		    {
		      Elf_Verneed * v;
		      
		      /* We must test both.  */
		      v = (Elf_Verneed *)
			(version_info [DT_VERSIONTAGIDX (DT_VERNEED)]
			 + header - loadaddr);
		      
		      for (;;)
			{
			  a = (Elf_Vernaux *)((char *)v + v->vn_aux);
			  
			  while (a->vna_other != vers_addr[si]
				 && a->vna_next != 0)
			    a = (Elf_Vernaux *)((char *)a + a->vna_next);
			  
			  if (a->vna_other == vers_addr[si])
			    break;

			  if (v->vn_next == 0)
			    {
			      if (elf_sections [psym->st_shndx].sh_type
				  != SHT_NOBITS)
				error (_("bad dynamic symbol"));
			      
			      a = NULL;
			      break;
			    }
			  
			  v = (Elf_Verneed *)((char *)v + v->vn_next);
			}
		      
		      if (a != NULL)
			printf ("@%s (%d)", strtab + a->vna_name, a->vna_other);
		    }
		  else if ((elf_sections [psym->st_shndx].sh_type
			    == SHT_NOBITS && a == NULL)
			   || psym->st_shndx != SHN_UNDEF)
		    {
		      Elf_Verdef * v;
		      Elf_Verdaux * b;
		      
		      v = (Elf_Verdef *)
			(version_info [DT_VERSIONTAGIDX (DT_VERDEF)]
			 + header - loadaddr);
			  
		      if (vers_addr[si] == 0x8001)
			pnt = "";
		      else
			{
			  while (v->vd_ndx != (vers_addr [si] & 0x7fff))
			    v = (Elf_Verdef *)((char *)v + v->vd_next);
			  
			  b = (Elf_Verdaux *) ((char *)v + v->vd_aux);
			  
			  if (psym->st_name != b->vda_name)
			    pnt = strtab + b->vda_name;
			  else
			    pnt = NULL;
			}
		      
		      if (pnt)
			printf ((vers_addr [si] & 0x8000)
				? "@%s" : "@@%s", pnt);
		    }
		}
	      
	      puts ("");
	    }
	}
    }

  if (! do_version)
    return;
  
  spnt = elf_sections;
  
  for (i = 0; i < epnt->e_shnum; i++, spnt++)
    {
      if (spnt->sh_type == SHT_GNU_verdef)
	{
	  Elf_Shdr * dspnt = &elf_sections[spnt->sh_link];
	  unsigned int idx;
	  unsigned int cnt;
		  
	  strtab = (char *) (header - loadaddr + dynamic_info[DT_STRTAB]);
		  
	  printf (_("\n Version definitions:%s (%#0x entries)\n"),
		  SECTION_NAME(spnt), spnt->sh_info);
	  printf (_("Addr: %#08x  Offset: %#08x  Link: %x (%s)\n"),
		  spnt->sh_addr, spnt->sh_offset, spnt->sh_link,
		  SECTION_NAME(dspnt));
		  
	  for (idx = cnt = 0; cnt < spnt->sh_info; ++cnt)
	    {
	      Elf_Verdef *  ent = (Elf_Verdef *)
		((char *) header + spnt->sh_offset + idx);
	      Elf_Verdaux * aux = (Elf_Verdaux *)
		((char *) ent + ent->vd_aux);
	      int j, isum;
	      
	      printf (_("%#06x: Rev: %d  Flags: "), idx, ent->vd_version);
	      
	      if (ent->vd_flags == 0)
		printf (_("none"));
	      else
		{
		  int f = 1;
		  if (ent->vd_flags & 0x1)
		    {
		      printf (_("BASE"));
		      f = 0;
		    }
		  if (ent->vd_flags & 0x2)
		    {
		      printf (_("%sWEAK"), f ? "" : "|");
		      f = 0;
		    }
		}
	      printf (_("  Index: %d  Cnt: %d  Name: %s\n"),
		      ent->vd_ndx, ent->vd_cnt, strtab + aux->vda_name);
	      j = 1;
	      isum = idx + ent->vd_aux;
	      while (j < ent->vd_cnt)
		{
		  isum += aux->vda_next;
		  aux = (Elf_Verdaux *)((char *)aux + aux->vda_next);
		  printf (_("  %#06x: Parent %d: %s\n"), isum, j,
			 strtab + aux->vda_name);
		  ++j;
		}

	      idx += ent->vd_next;
	    }
	}
      
      if (spnt->sh_type == SHT_GNU_verneed)
	{
	  Elf_Shdr * dspnt = &elf_sections[spnt->sh_link];
	  unsigned int idx;
	  unsigned int cnt;
	  
	  strtab = (char *) (header - loadaddr + dynamic_info[DT_STRTAB]);
	  printf (_("\n Needed versions:%s (%#0x entries)\n"),
		 SECTION_NAME (spnt), spnt->sh_info);
	  printf (_("Addr: %#08x  Offset: %#08x  Link: %x (%s)\n"),
		 spnt->sh_addr, spnt->sh_offset, spnt->sh_link,
		 SECTION_NAME (dspnt));

	  for (idx = cnt = 0; cnt < spnt->sh_info; ++cnt)
	    {
	      Elf_Verneed * ent = (Elf_Verneed *)
		((char *) header + spnt->sh_offset + idx);
	      Elf_Vernaux * aux = (Elf_Vernaux *)
		((char *) ent + ent->vn_aux);
	      int j, isum;
	      
	      printf (_("%#06x: Version: %d  File: %s  Cnt: %d\n"),
		      idx, ent->vn_version, strtab + ent->vn_file,ent->vn_cnt);
	      
	      for (j = 0, isum = idx + ent->vn_aux; j < ent->vn_cnt; ++j)
		{
		  printf (_("  %#06x: Name: %s  Flags: %s  Version: %d\n"),
			  isum, strtab+aux->vna_name,
			  aux->vna_flags & 0x2 ? "WEAK" : "none",
			  aux->vna_other);
		  isum += aux->vna_next;
		  aux = (Elf_Vernaux *)((char *) aux + aux->vna_next);
		}

	      idx += ent->vn_next;
	    }
	}
      
      if (spnt->sh_type == SHT_GNU_versym)
	{
	  Elf_Shdr * dspnt = &elf_sections[spnt->sh_link];
	  int total = spnt->sh_size / spnt->sh_entsize;
	  int cnt;
	  unsigned short * p = (short *)
	    (version_info[DT_VERNEEDNUM - DT_VERSYM] + header - loadaddr);

	  symtab = (Elf_Sym *) (header + dspnt->sh_offset);
	  strtab = (char *) (header + elf_sections[dspnt->sh_link].sh_offset);

	  printf (_("\n Version symbols:%s (%#0x entries)\n"),
		  SECTION_NAME (spnt), total);
	  printf (_("Addr: %#08x  Offset: %#08x  Link: %x (%s)\n"),
		  spnt->sh_addr, spnt->sh_offset, spnt->sh_link,
		  SECTION_NAME (dspnt));
	  
	  for (cnt = 0; cnt < total; cnt += 4)
	    {
	      int j, nn;
	      
	      printf ("%#08x:", cnt);
	      
	      for (j = 0; (j < 4) && (cnt + j) < total; ++j)
		switch (p[cnt + j])
		  {
		  case 0:
		    printf ("   0 (*local*)  ");
		    break;
		  case 1:
		    printf ("   1 (*global*) ");
		    break;
		  default:
		    nn = printf ("%4x%c", p[cnt + j] & 0x7fff,
				 p[cnt + j] & 0x8000 ? 'h' : ' ');
		    if (elf_sections[symtab[cnt + j].st_shndx].sh_type
			== SHT_NOBITS)
		      {
			/* We must test both.  */
			Elf_Verneed * v = (Elf_Verneed *)
			  (version_info [DT_VERNEEDNUM - DT_VERNEED]
			   + header - loadaddr);
			Elf_Vernaux * a = NULL;
		      
			for (;;)
			  {
			    a = (Elf_Vernaux *)((char *) v + v->vn_aux);
			    
			    while (a->vna_other != p[cnt + j]
				   && a->vna_next != 0)
			      a = (Elf_Vernaux *)((char *) a + a->vna_next);
			    
			    if (a->vna_other == p[cnt + j])
			      break;
			    
			    if (v->vn_next == 0)
			      {
				a = NULL;
				break;
			      }
			    
			    v = (Elf_Verneed *)((char *)v + v->vn_next);
			  }
		      
			if (a != NULL)
			  nn += printf ("(%s)", strtab + a->vna_name);
			else
			  {
			    Elf_Verdef * v = (Elf_Verdef *)
			      (version_info [DT_VERNEEDNUM - DT_VERDEF]
			       + header - loadaddr);
			    Elf_Verdaux * a;
			    
			    if (p[cnt + j] == 0x8001)
			      pnt = "";
			    else
			      {
				while (v->vd_ndx != (p[cnt + j]&0x7fff))
				  v = (Elf_Verdef *)((char *)v + v->vd_next);
				
				a = (Elf_Verdaux *) ((char *) v + v->vd_aux);
				pnt = strtab + a->vda_name;
			      }
			    
			    if (pnt)
			      nn += printf ("(%s)", pnt);
			  }
			
			if (nn <16)
			  printf ("%*c", 16 - nn, ' ');
		      }
		    else if (symtab[cnt + j].st_shndx ==SHN_UNDEF)
		      {
			Elf_Verneed * v = (Elf_Verneed *)
			  (version_info [DT_VERNEEDNUM - DT_VERNEED]
			   + header - loadaddr);
			Elf_Vernaux * a;
		      
			for (;;)
			  {
			    a = (Elf_Vernaux *)((char *) v + v->vn_aux);
			    
			    while (a->vna_other != p[cnt + j]
				   && a->vna_next != 0)
			      a = (Elf_Vernaux *)((char *)a + a->vna_next);
			    
			    if (a->vna_other == p[cnt + j])
			      break;
			    
			    v = (Elf_Verneed *)((char *) v + v->vn_next);
			  }
		      
			nn += printf ("(%s)", strtab + a->vna_name);
			
			if (nn <16)
			  printf ("%*c", 16 - nn, ' ');
		      }
		    else
		      {
			Elf_Verdef * v = (Elf_Verdef *)
			  (version_info [DT_VERNEEDNUM - DT_VERDEF]
			   + header - loadaddr);
			Elf_Verdaux * a;
		      
			if (p[cnt + j] == 0x8001)
			  pnt = "";
			else
			  {
			    while (v->vd_ndx != (p[cnt + j] & 0x7fff))
			      v = (Elf_Verdef *)((char *) v + v->vd_next);
			    
			    a = (Elf_Verdaux *) ((char *) v + v->vd_aux);
			    pnt = strtab + a->vda_name;
			  }
		      
			if (pnt)
			  nn += printf ("(%s)", pnt);
			
			if (nn <16)
			  printf ("%*c", 16 - nn, ' ');
		      }
		  }
	      
	      printf ("\n");
	    }
	}
    }
}

static void
process_section_contents ()
{
  Elf_Shdr *    spnt;
  int             i;

  if (! do_dump)
    return;

  spnt = elf_sections;

  for (i = 0; i < epnt->e_shnum; i++, spnt++)
    {
      int             bytes;
      int             addr;
      int             lbytes;
      unsigned char * my_addr;

#ifdef SUPPORT_DISASSEMBLY
      /* See if we need an assembly dump of this section */
      
      if ((i < NUM_DUMP_SECTS) && (dump_sects[i] & DISASS_DUMP))
	{
	  printf (_("\nAssembly dump of section %s\n"), SECTION_NAME (spnt));
	  
	  bytes   = spnt->sh_size;
	  addr    = spnt->sh_addr;
	  my_addr = (unsigned char *) (header + spnt->sh_offset);
	  
	  while (bytes > 0)
	    {
	      printf ("0x%8.8x  ", addr);
	      
	      switch (epnt->e_machine)
		{
		  case EM_386:
		  case EM_486:
		    lbytes =  db_disasm ((unsigned int) my_addr, 0, 0) -
		      ((unsigned int) my_addr);
		    break;
		  case EM_68K:
		    lbytes = (m68k_disass ((unsigned int) my_addr, addr)
			      - (unsigned int) my_addr);
		    break;
		  default:
		    warn (_("Unable to disassemble code for this platform\n"));
		    return;
		  }
		
		addr    += lbytes;
		my_addr += lbytes;
		bytes   -= lbytes;
		
		printf ("\n");
	      }
	  }
#endif
      
	/* OK, see if we need a hex dump of this section.  */
	if ((i < NUM_DUMP_SECTS) && (dump_sects[i] & HEX_DUMP))
	  {
	    int j;
	    int k;
	    
	    printf (_("\nHex dump of section %s\n"), SECTION_NAME (spnt));
	    
	    bytes   = spnt->sh_size;
	    addr    = spnt->sh_addr;
	    my_addr = (unsigned char *) (header + spnt->sh_offset);
	    
	    while (bytes)
	      {
		lbytes = (bytes > 16 ? 16 : bytes);
		
		printf ("0x%8.8x ",addr);
		
		switch (epnt->e_ident [EI_DATA])
		  {
		  case ELFDATA2LSB:
		    for (j = 15; j >= 0; j --)
		      {
			if (j < lbytes)
			  printf ("%2.2x", my_addr[j]);
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
			  printf ("%2.2x", my_addr[j]);
			else
			  printf ("  ");
			
			if ((j & 3) == 3)
			  printf (" ");
		      }
		    break;
		  }
		
		for (j = 0; j < lbytes; j++)
		  {
		    k = my_addr [j];
		    if (k >= ' ' && k < 0x80)
		      printf ("%c", k);
		    else
		      printf (".");
		  }
		
		printf ("\n");
		
		my_addr += lbytes;
		addr    += lbytes;
		bytes   -= lbytes;
	      }
	  }
    }
}

static void
process_file (file_name)
     char * file_name;
{
  int          fd;
  struct stat  statbuf;
    
  must_swap = 0;
  
  fd = open (file_name, O_RDONLY);
  if (fd == -1)
    {
      error (_("Input file %s not found.\n"), file_name);
      return;
    }
      
  if (fstat (fd, & statbuf) < 0)
    {
      error (_("Cannot stat input file %s.\n"), file_name);
      close (fd);
      return;
    }
  
  header = mmap (0, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  
  if ((header == (char *) -1) || (header == NULL))
    {
      error (_("Cannot mmap %s: %s\n"), file_name, strerror (errno));
      close (fd);
      return;
    }
  
  close (fd);

  epnt = (Elf_Ehdr *) header;

  if (show_name)
    printf (_("\nFile: %s\n"), file_name);
  
  if (! process_elf_header ())
    {
      munmap (header, statbuf.st_size);
      return;
    }
  
  process_program_headers ();
  
  if (loadaddr == -1)
    {
      /* Very strange. */
      loadaddr = 0;
    }

  process_section_headers ();

  process_dynamic_segment ();

  process_symbol_table ();

  process_section_contents ();
  
  munmap (header, statbuf.st_size);

  if (must_swap)
    {
      if (epnt)
	{
	  free (epnt);
	  epnt = NULL;
	}

      if (elf_sections)
	{
	  free (elf_sections);
	  elf_sections = NULL;
	}
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

  expected_endian = 0x12345678;
  
  if (* ((char *) & expected_endian) == 0x12)
    expected_endian = ELFDATA2MSB;
  else
    expected_endian = ELFDATA2LSB;

  if (optind < (argc - 1))
    show_name = 1;
  
  while (optind < argc)
    process_file (argv [optind ++]);

  return 0;
}
