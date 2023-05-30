/* od-pe.c -- dump information about a PE object file.
   Copyright (C) 2011-2023 Free Software Foundation, Inc.
   Written by Tristan Gingold, Adacore and Nick Clifton, Red Hat.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stddef.h>
#include <time.h>
#include "safe-ctype.h"
#include "bfd.h"
#include "objdump.h"
#include "bucomm.h"
#include "bfdlink.h"
#include "coff/internal.h"
#define L_LNNO_SIZE 4 /* FIXME: which value should we use ?  */
#include "coff/external.h"
#include "coff/pe.h"
#include "libcoff.h"
#include "libpei.h"

/* Index of the options in the options[] array.  */
#define OPT_FILE_HEADER 0
#define OPT_AOUT 1
#define OPT_SECTIONS 2
#define OPT_SYMS 3
#define OPT_RELOCS 4
#define OPT_LINENO 5
#define OPT_LOADER 6
#define OPT_EXCEPT 7
#define OPT_TYPCHK 8
#define OPT_TRACEBACK 9
#define OPT_TOC 10
#define OPT_LDINFO 11

/* List of actions.  */
static struct objdump_private_option options[] =
{
  { "header", 0 },
  { "aout", 0 },
  { "sections", 0 },
  { "syms", 0 },
  { "relocs", 0 },
  { "lineno", 0 },
  { "loader", 0 },
  { "except", 0 },
  { "typchk", 0 },
  { "traceback", 0 },
  { "toc", 0 },
  { "ldinfo", 0 },
  { NULL, 0 }
};

/* Simplified section header.  */
struct pe_section
{
  /* NUL terminated name.  */
  char name[9];

  /* Section flags.  */
  unsigned int flags;

  /* Offsets in file.  */
  ufile_ptr scnptr;
  ufile_ptr relptr;
  ufile_ptr lnnoptr;

  /* Number of relocs and line numbers.  */
  unsigned int nreloc;
  unsigned int nlnno;
};

/* Translation entry type.  The last entry must be {0, NULL}.  */

struct xlat_table
{
  unsigned int  val;
  const char *  name;
};

/* PE file flags.  */
static const struct xlat_table file_flag_xlat[] =
{
  { IMAGE_FILE_RELOCS_STRIPPED,     "RELOCS STRIPPED"},
  { IMAGE_FILE_EXECUTABLE_IMAGE,    "EXECUTABLE"},
  { IMAGE_FILE_LINE_NUMS_STRIPPED,  "LINE NUMS STRIPPED"},
  { IMAGE_FILE_LOCAL_SYMS_STRIPPED, "LOCAL SYMS STRIPPED"},
  { IMAGE_FILE_AGGRESSIVE_WS_TRIM,  "AGGRESSIVE WS TRIM"},
  { IMAGE_FILE_LARGE_ADDRESS_AWARE, "LARGE ADDRESS AWARE"},
  { IMAGE_FILE_16BIT_MACHINE,       "16BIT MACHINE"},
  { IMAGE_FILE_BYTES_REVERSED_LO,   "BYTES REVERSED LO"},
  { IMAGE_FILE_32BIT_MACHINE,       "32BIT MACHINE"},
  { IMAGE_FILE_DEBUG_STRIPPED,      "DEBUG STRIPPED"},
  { IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "REMOVABLE RUN FROM SWAP"},
  { IMAGE_FILE_NET_RUN_FROM_SWAP,   "NET RUN FROM SWAP"},
  { IMAGE_FILE_SYSTEM,              "SYSTEM"},
  { IMAGE_FILE_DLL,                 "DLL"},
  { IMAGE_FILE_UP_SYSTEM_ONLY,      "UP SYSTEM ONLY"},
  { IMAGE_FILE_BYTES_REVERSED_HI,   "BYTES REVERSED HI"},
  { 0, NULL }
};

/* PE section flags.  */
static const struct xlat_table section_flag_xlat[] =
{
  { IMAGE_SCN_MEM_DISCARDABLE, "DISCARDABLE" },
  { IMAGE_SCN_MEM_EXECUTE,     "EXECUTE" },
  { IMAGE_SCN_MEM_READ,        "READ" },
  { IMAGE_SCN_MEM_WRITE,       "WRITE" },
  { IMAGE_SCN_TYPE_NO_PAD,     "NO PAD" },
  { IMAGE_SCN_CNT_CODE,        "CODE" },
  { IMAGE_SCN_CNT_INITIALIZED_DATA,   "INITIALIZED DATA" },
  { IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED DATA" },
  { IMAGE_SCN_LNK_OTHER,       "OTHER" },
  { IMAGE_SCN_LNK_INFO,        "INFO" },
  { IMAGE_SCN_LNK_REMOVE,      "REMOVE" },
  { IMAGE_SCN_LNK_COMDAT,      "COMDAT" },
  { IMAGE_SCN_MEM_FARDATA,     "FARDATA" },
  { IMAGE_SCN_MEM_PURGEABLE,   "PURGEABLE" },
  { IMAGE_SCN_MEM_LOCKED,      "LOCKED" },
  { IMAGE_SCN_MEM_PRELOAD,     "PRELOAD" },
  { IMAGE_SCN_LNK_NRELOC_OVFL, "NRELOC OVFL" },
  { IMAGE_SCN_MEM_NOT_CACHED,  "NOT CACHED" },
  { IMAGE_SCN_MEM_NOT_PAGED,   "NOT PAGED" },
  { IMAGE_SCN_MEM_SHARED,      "SHARED" },    
  { 0, NULL }
};


/* Display help.  */

static void
pe_help (FILE *stream)
{
  fprintf (stream, _("\
For PE files:\n\
  header      Display the file header\n\
  sections    Display the section headers\n\
"));
}

/* Return true if ABFD is handled.  */

static int
pe_filter (bfd *abfd)
{
  return bfd_get_flavour (abfd) == bfd_target_coff_flavour;
}

/* Display the list of name (from TABLE) for FLAGS, using comma to
   separate them.  A name is displayed if FLAGS & VAL is not 0.  */

static void
dump_flags (const struct xlat_table * table, unsigned int flags)
{
  unsigned int r = flags;
  bool first = true;
  const struct xlat_table *t;

  for (t = table; t->name; t++)
    if ((flags & t->val) != 0)
      {
        r &= ~t->val;

        if (first)
          first = false;
        else
          putchar (',');
        fputs (t->name, stdout);
      }

  /* Undecoded flags.  */
  if (r != 0)
    {
      if (!first)
        putchar (',');
      printf (_("unknown: 0x%x"), r);
    }
}

static const char *
decode_machine_number (unsigned int machine)
{
  switch (machine)
    {
    case IMAGE_FILE_MACHINE_ALPHA:       return "ALPHA";
    case IMAGE_FILE_MACHINE_AMD64:       return "AMD64";
    case IMAGE_FILE_MACHINE_ARM:         return "ARM";
    case IMAGE_FILE_MACHINE_ARM64:       return "ARM64";
    case IMAGE_FILE_MACHINE_I386:        return "I386";
    case IMAGE_FILE_MACHINE_IA64:        return "IA64";
    case IMAGE_FILE_MACHINE_LOONGARCH64: return "LOONGARCH64";
    case IMAGE_FILE_MACHINE_POWERPC:     return "POWERPC";
    case 0x0500:                         return "SH (big endian)";
    case 0x0550:                         return "SH (little endian)";
    case 0x0b00:                         return "MCore";
    case 0x0093:                         return "TI C4X";
      // FIXME: Add more machine numbers.
    default: return N_("unknown");
    }
}

/* Dump the file header.  */

static void
dump_pe_file_header (bfd *                            abfd,
		     struct external_PEI_filehdr *    fhdr,
		     struct external_PEI_IMAGE_hdr *  ihdr)
{
  unsigned long ihdr_off = 0;

  if (fhdr != NULL)
    {
      printf (_("\nFile Header:\n"));

      /* FIXME: The fields in the file header are boring an generally have
	 fixed values.  Is there any benefit in displaying them ?  */

      /* Display the first string found in the stub.
	 FIXME: Look for more than one string ?
	 FIXME: Strictly speaking we may not have read the full stub, since
	 it can be longer than the dos_message array in the PEI_fileheader
	 structure.  */
      const unsigned char * message = (const unsigned char *) fhdr->dos_message;
      unsigned int len = sizeof (fhdr->dos_message);
      unsigned int i;
      unsigned int seen_count = 0;
      unsigned int string_start = 0;
  
      for (i = 0; i < len; i++)
	{
	  if (ISPRINT (message[i]))
	    {
	      if (string_start == 0)
		string_start = i;
	      ++ seen_count;
	      if (seen_count > 4)
		break;
	    }
	  else
	    {
	      seen_count = string_start = 0;
	    }
	}

      if (seen_count > 4)
	{
	  printf (_("  Stub message:  "));
	  while (string_start < len)
	    {
	      char c = message[string_start ++];
	      if (! ISPRINT (c))
		break;
	      putchar (c);
	    }
	  putchar ('\n');
	}

      ihdr_off = (long) bfd_h_get_32 (abfd, fhdr->e_lfanew);
    }

  printf (_("\nImage Header (at offset %#lx):\n"), ihdr_off);
	  
  unsigned int machine = (int) bfd_h_get_16 (abfd, ihdr->f_magic);
  printf (_("  Machine Num:   %#x\t\t- %s\n"), machine,
	  decode_machine_number (machine));

  printf (_("  Num sections:  %d\n"), (int) bfd_h_get_16 (abfd, ihdr->f_nscns));

  long timedat = bfd_h_get_32 (abfd, ihdr->f_timdat);
  printf (_("  Time and date: %#08lx\t- "), timedat);
  if (timedat == 0)
    printf (_("not set\n"));
  else
    {
      /* Not correct on all platforms, but works on unix.  */
      time_t t = timedat;
      fputs (ctime (& t), stdout);
    }

  printf (_("  Symbols off:   %#08lx\n"),
	  (long) bfd_h_get_32 (abfd, ihdr->f_symptr));
  printf (_("  Num symbols:   %ld\n"),
	  (long) bfd_h_get_32 (abfd, ihdr->f_nsyms));

  unsigned int opt_header_size = (int) bfd_h_get_16 (abfd, ihdr->f_opthdr);
  printf (_("  Opt hdr sz:    %#x\n"), opt_header_size);

  unsigned int flags = (int) bfd_h_get_16 (abfd, ihdr->f_flags);
  printf (_("  Flags:         0x%04x\t\t- "), flags);
  dump_flags (file_flag_xlat, flags);
  putchar ('\n');

  if (opt_header_size == PEPAOUTSZ)
    {
      PEPAOUTHDR xhdr;

      printf (_("\nOptional PE+ Header (at offset %#lx):\n"),
	      ihdr_off + sizeof (* ihdr));

      if (bfd_seek (abfd, ihdr_off + sizeof (* ihdr), SEEK_SET) != 0
	  || bfd_bread (& xhdr, sizeof (xhdr), abfd) != sizeof (xhdr))
	printf ("error: unable to read PE+ header\n");
      else
	{
	  /* FIXME: Check that the magic number is 0x020b ?  */
	  printf (_("  Magic:         %x\n"),
		  (int) bfd_h_get_16 (abfd, xhdr.standard.magic));
	  printf (_("  Image Base:    %lx\n"),
		  (long) bfd_h_get_64 (abfd, xhdr.ImageBase));
	  /* FIXME: Print more fields.  */
	}
    }
  else if (opt_header_size == AOUTSZ)
    {
      PEAOUTHDR xhdr;

      printf (_("\nOptional PE Header (at offset %#lx):\n"),
	      ihdr_off + sizeof (* ihdr));

      if (bfd_seek (abfd, ihdr_off + sizeof (* ihdr), SEEK_SET) != 0
	  || bfd_bread (& xhdr, sizeof (xhdr), abfd) != sizeof (xhdr))
	printf ("error: unable to read PE+ header\n");
      else
	{
	  /* FIXME: Check that the magic number is 0x010b ?  */
	  printf (_("  Magic:         %x\n"),
		  (int) bfd_h_get_16 (abfd, xhdr.standard.magic));
	  printf (_("  Image Base:    %lx\n"),
		  (long) bfd_h_get_32 (abfd, xhdr.ImageBase));
	  /* FIXME: Print more fields.  */
	}
    }
  else if (opt_header_size != 0)
    {
      printf (_("\nUnsupported size of Optional Header\n"));
    }
}

/* Dump the sections header.  */

static void
dump_pe_sections_header (bfd *                            abfd,
			 struct external_PEI_filehdr *    fhdr,
			 struct external_PEI_IMAGE_hdr *  ihdr)
{
  unsigned int opthdr = (int) bfd_h_get_16 (abfd, ihdr->f_opthdr);
  unsigned int n_scns = (int) bfd_h_get_16 (abfd, ihdr->f_nscns);
  unsigned int off;

  /* The section header starts after the file, image and optional headers.  */  
  if (fhdr == NULL)
    off = sizeof (struct external_filehdr) + opthdr;
  else
    off = (int) bfd_h_get_16 (abfd, fhdr->e_lfanew) + sizeof (* ihdr) + opthdr;

  printf (_("\nSection headers (at offset 0x%08x):\n"), off);

  if (n_scns == 0)
    {
      printf (_("  No section headers\n"));
      return;
    }
  if (bfd_seek (abfd, off, SEEK_SET) != 0)
    {
      non_fatal (_("cannot seek to section headers start\n"));
      return;
    }

  /* We don't translate this string as it consists of field names.  */
  if (wide_output)
    printf (" # Name     paddr    vaddr    size     scnptr   relptr   lnnoptr   nrel nlnno   Flags\n");
  else
    printf (" # Name     paddr    vaddr    size     scnptr   relptr   lnnoptr   nrel nlnno\n");

  unsigned int i;
  for (i = 0; i < n_scns; i++)
    {
      struct external_scnhdr scn;
      unsigned int flags;

      if (bfd_bread (& scn, sizeof (scn), abfd) != sizeof (scn))
        {
          non_fatal (_("cannot read section header"));
          return;
        }

      printf ("%2d %-8.8s %08x %08x %08x %08x %08x %08x %5d %5d",
              i + 1, scn.s_name,
              (unsigned int) bfd_h_get_32 (abfd, scn.s_paddr),
              (unsigned int) bfd_h_get_32 (abfd, scn.s_vaddr),
              (unsigned int) bfd_h_get_32 (abfd, scn.s_size),
              (unsigned int) bfd_h_get_32 (abfd, scn.s_scnptr),
              (unsigned int) bfd_h_get_32 (abfd, scn.s_relptr),
              (unsigned int) bfd_h_get_32 (abfd, scn.s_lnnoptr),
              (unsigned int) bfd_h_get_16 (abfd, scn.s_nreloc),
              (unsigned int) bfd_h_get_16 (abfd, scn.s_nlnno));

      flags = bfd_h_get_32 (abfd, scn.s_flags);
      if (wide_output)
	printf (_("   %08x "), flags);
      else
	printf (_("\n            Flags: %08x: "), flags);

      if (flags != 0)
        {
	  /* Skip the alignment bits.  */
	  flags &= ~ IMAGE_SCN_ALIGN_POWER_BIT_MASK;
          dump_flags (section_flag_xlat, flags);
        }

      putchar ('\n');
    }
}

/* Handle a PE format file.  */

static void
dump_pe (bfd *                            abfd,
	 struct external_PEI_filehdr *    fhdr,
	 struct external_PEI_IMAGE_hdr *  ihdr)
{
  if (options[OPT_FILE_HEADER].selected)
    dump_pe_file_header (abfd, fhdr, ihdr);
  
  if (options[OPT_SECTIONS].selected)
    dump_pe_sections_header (abfd, fhdr, ihdr);
}

static bool
is_pe_object_magic (unsigned short magic)
{
  switch (magic)
    {
    case IMAGE_FILE_MACHINE_ALPHA:
    case IMAGE_FILE_MACHINE_ARM:
    case IMAGE_FILE_MACHINE_ARM64:
    case IMAGE_FILE_MACHINE_ARMNT:
    case IMAGE_FILE_MACHINE_I386:
    case IMAGE_FILE_MACHINE_IA64:
    case IMAGE_FILE_MACHINE_POWERPC:
    case IMAGE_FILE_MACHINE_LOONGARCH64:
    case IMAGE_FILE_MACHINE_AMD64:
      // FIXME: Add more machine numbers.
      return true;
    case 0x0a00: /* ARMMAGIC */
    case 0x0500: /* SH_ARCH_MAGIC_BIG */
    case 0x0550: /* SH_ARCH_MAGIC_LITTLE */
    case 0x0b00: /* MCore */
    case 0x0093: /* TI C4x */
      return true;
    default:
      return false;
    }
}

/* Dump ABFD (according to the options[] array).  */

static void
pe_dump_obj (bfd *abfd)
{
  struct external_PEI_filehdr fhdr;

  /* Read file header.  */
  if (bfd_seek (abfd, 0, SEEK_SET) != 0
      || bfd_bread (& fhdr, sizeof (fhdr), abfd) != sizeof (fhdr))
    {
      non_fatal (_("cannot seek to/read file header"));
      return;
    }

  unsigned short magic = bfd_h_get_16 (abfd, fhdr.e_magic);

  /* PE format executable files have a full external_PEI_filehdr structure
     at the start.  PE format object files just have an external_filehdr
     structure at the start.  */
  if (magic == IMAGE_DOS_SIGNATURE)
    {
      unsigned int ihdr_offset = (int) bfd_h_get_16 (abfd, fhdr.e_lfanew);

      /* FIXME: We could reuse the fields in fhdr, but that might
	 confuse various sanitization and memory checker tools.  */
      struct external_PEI_IMAGE_hdr ihdr;

      if (bfd_seek (abfd, ihdr_offset, SEEK_SET) != 0
	  || bfd_bread (& ihdr, sizeof (ihdr), abfd) != sizeof (ihdr))
	{
	  non_fatal (_("cannot seek to/read image header at offset %#x"),
		     ihdr_offset);
	  return;
	}

      unsigned int signature = (int) bfd_h_get_16 (abfd, ihdr.nt_signature);
      if (signature != IMAGE_NT_SIGNATURE)
	{
	  non_fatal ("file does not have an NT format signature: %#x",
		     signature);
	  return;
	}
  
      dump_pe (abfd, & fhdr, & ihdr);
    }
  else if (is_pe_object_magic (magic))
    {
      struct external_filehdr ehdr;

      if (bfd_seek (abfd, 0, SEEK_SET) != 0
	  || bfd_bread (& ehdr, sizeof (ehdr), abfd) != sizeof (ehdr))
	{
	  non_fatal (_("cannot seek to/read image header"));
	  return;
	}

      struct external_PEI_IMAGE_hdr ihdr;
      memcpy (& ihdr.f_magic, & ehdr, sizeof (ehdr));
      dump_pe (abfd, NULL, & ihdr);
    }
  else
    {
      non_fatal ("not a PE format binary - unexpected magic number: %#x",
		 magic);
      return;
    }
}

/* Dump a PE file.  */

static void
pe_dump (bfd *abfd)
{
  /* We rely on BFD to decide if the file is a core file.  Note that core
     files are only supported on native environment by BFD.  */
  switch (bfd_get_format (abfd))
    {
    case bfd_core:
      // FIXME: Handle PE format core files ?
      break;
    default:
      pe_dump_obj (abfd);
      break;
    }
}

/* Vector for pe.  */

const struct objdump_private_desc objdump_private_desc_pe =
{
  pe_help,
  pe_filter,
  pe_dump,
  options
};
