/* Mach-O support for BFD.
   Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "mach-o.h"
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"
#include "aout/stab_gnu.h"
#include <ctype.h>

#define bfd_mach_o_object_p bfd_mach_o_gen_object_p
#define bfd_mach_o_core_p bfd_mach_o_gen_core_p
#define bfd_mach_o_mkobject bfd_false

static unsigned int 
bfd_mach_o_version (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = NULL;

  BFD_ASSERT (bfd_mach_o_valid (abfd));
  mdata = abfd->tdata.mach_o_data;

  return mdata->header.version;
}

bfd_boolean
bfd_mach_o_valid (bfd *abfd)
{
  if (abfd == NULL || abfd->xvec == NULL)
    return FALSE;

  if (abfd->xvec->flavour != bfd_target_mach_o_flavour)
    return FALSE;

  if (abfd->tdata.mach_o_data == NULL)
    return FALSE;
  return TRUE;
}

/* Tables to translate well known Mach-O segment/section names to bfd
   names.  Use of canonical names (such as .text or .debug_frame) is required
   by gdb.  */

struct mach_o_section_name_xlat
{
  const char *bfd_name;
  const char *mach_o_name;
};

static const struct mach_o_section_name_xlat dwarf_section_names_xlat[] =
  {
    { ".debug_frame", "__debug_frame" },
    { ".debug_info", "__debug_info" },
    { ".debug_abbrev", "__debug_abbrev" },
    { ".debug_aranges", "__debug_aranges" },
    { ".debug_macinfo", "__debug_macinfo" },
    { ".debug_line", "__debug_line" },
    { ".debug_loc", "__debug_loc" },
    { ".debug_pubnames", "__debug_pubnames" },
    { ".debug_pubtypes", "__debug_pubtypes" },
    { ".debug_str", "__debug_str" },
    { ".debug_ranges", "__debug_ranges" },
    { NULL, NULL}
  };

static const struct mach_o_section_name_xlat text_section_names_xlat[] =
  {
    { ".text", "__text" },
    { ".cstring", "__cstring" },
    { ".eh_frame", "__eh_frame" },
    { NULL, NULL}
  };

static const struct mach_o_section_name_xlat data_section_names_xlat[] =
  {
    { ".data", "__data" },
    { ".bss", "__bss" },
    { NULL, NULL}
  };

struct mach_o_segment_name_xlat
{
  const char *segname;
  const struct mach_o_section_name_xlat *sections;
};

static const struct mach_o_segment_name_xlat segsec_names_xlat[] = 
  {
    { "__DWARF", dwarf_section_names_xlat },
    { "__TEXT", text_section_names_xlat },
    { "__DATA", data_section_names_xlat },
    { NULL, NULL }
  };


/* Mach-O to bfd names.  */

static char *
bfd_mach_o_convert_section_name_to_bfd (bfd *abfd, bfd_mach_o_section *section)
{
  const struct mach_o_segment_name_xlat *seg;
  char *res;
  unsigned int len;

  for (seg = segsec_names_xlat; seg->segname; seg++)
    {
      if (strcmp (seg->segname, section->segname) == 0)
        {
          const struct mach_o_section_name_xlat *sec;

          for (sec = seg->sections; sec->mach_o_name; sec++)
            {
              if (strcmp (sec->mach_o_name, section->sectname) == 0)
                {
                  len = strlen (sec->bfd_name);
                  res = bfd_alloc (abfd, len + 1);
                  
                  if (res == NULL)
                    return NULL;
                  strcpy (res, sec->bfd_name);
                  return res;
                }
            }
        }
    }

  len = sizeof ("LC_SEGMENT") - 1 + 1
    + strlen (section->segname) + 1
    + strlen (section->sectname) + 1;

  res = bfd_alloc (abfd, len);
  if (res == NULL)
    return NULL;
  snprintf (res, len, "LC_SEGMENT.%s.%s", section->segname, section->sectname);
  return res;
}

/* Convert a bfd sectio name to a Mach-O segment + section name.  */

static void
bfd_mach_o_convert_section_name_to_mach_o (bfd *abfd ATTRIBUTE_UNUSED,
                                           asection *sect,
                                           bfd_mach_o_section *section)
{
  const struct mach_o_segment_name_xlat *seg;
  const char *name = bfd_get_section_name (abfd, sect);
  const char *dot;
  unsigned int len;
  unsigned int seglen;
  unsigned int seclen;

  /* List of well known names.  */
  for (seg = segsec_names_xlat; seg->segname; seg++)
    {
      const struct mach_o_section_name_xlat *sec;

      for (sec = seg->sections; sec->mach_o_name; sec++)
        {
          if (strcmp (sec->bfd_name, name) == 0)
            {
              strcpy (section->segname, seg->segname);
              strcpy (section->sectname, sec->mach_o_name);
              return;
            }
        }
    }

  /* Strip LC_SEGMENT. prefix.  */
  if (strncmp (name, "LC_SEGMENT.", 11) == 0)
    name += 11;

  /* Find a dot.  */
  dot = strchr (name, '.');
  len = strlen (name);

  /* Try to split name into segment and section names.  */
  if (dot && dot != name)
    {
      seglen = dot - name;
      seclen = len - (dot + 1 - name);

      if (seglen < 16 && seclen < 16)
        {
          memcpy (section->segname, name, seglen);
          section->segname[seglen] = 0;
          memcpy (section->sectname, dot + 1, seclen);
          section->sectname[seclen] = 0;
          return;
        }
    }

  if (len > 16)
    len = 16;
  memcpy (section->segname, name, len);
  section->segname[len] = 0;
  memcpy (section->sectname, name, len);
  section->sectname[len] = 0;
}

/* Copy any private info we understand from the input symbol
   to the output symbol.  */

bfd_boolean
bfd_mach_o_bfd_copy_private_symbol_data (bfd *ibfd ATTRIBUTE_UNUSED,
					 asymbol *isymbol ATTRIBUTE_UNUSED,
					 bfd *obfd ATTRIBUTE_UNUSED,
					 asymbol *osymbol ATTRIBUTE_UNUSED)
{
  return TRUE;
}

/* Copy any private info we understand from the input section
   to the output section.  */

bfd_boolean
bfd_mach_o_bfd_copy_private_section_data (bfd *ibfd ATTRIBUTE_UNUSED,
					  asection *isection ATTRIBUTE_UNUSED,
					  bfd *obfd ATTRIBUTE_UNUSED,
					  asection *osection ATTRIBUTE_UNUSED)
{
  return TRUE;
}

/* Copy any private info we understand from the input bfd
   to the output bfd.  */

bfd_boolean
bfd_mach_o_bfd_copy_private_bfd_data (bfd *ibfd, bfd *obfd)
{
  if (bfd_get_flavour (ibfd) != bfd_target_mach_o_flavour
      || bfd_get_flavour (obfd) != bfd_target_mach_o_flavour)
    return TRUE;

  BFD_ASSERT (bfd_mach_o_valid (ibfd));
  BFD_ASSERT (bfd_mach_o_valid (obfd));

  /* FIXME: copy commands.  */

  return TRUE;
}

/* Count the total number of symbols.  Traverse all sections.  */

static long
bfd_mach_o_count_symbols (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = NULL;
  long nsyms = 0;
  unsigned long i;

  BFD_ASSERT (bfd_mach_o_valid (abfd));
  mdata = abfd->tdata.mach_o_data;

  for (i = 0; i < mdata->header.ncmds; i++)
    if (mdata->commands[i].type == BFD_MACH_O_LC_SYMTAB)
      {
	bfd_mach_o_symtab_command *sym = &mdata->commands[i].command.symtab;
	nsyms += sym->nsyms;
      }

  return nsyms;
}

long
bfd_mach_o_get_symtab_upper_bound (bfd *abfd)
{
  long nsyms = bfd_mach_o_count_symbols (abfd);

  if (nsyms < 0)
    return nsyms;

  return ((nsyms + 1) * sizeof (asymbol *));
}

long
bfd_mach_o_canonicalize_symtab (bfd *abfd, asymbol **alocation)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  long nsyms = bfd_mach_o_count_symbols (abfd);
  asymbol **csym = alocation;
  unsigned long i, j;

  if (nsyms < 0)
    return nsyms;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      if (mdata->commands[i].type == BFD_MACH_O_LC_SYMTAB)
	{
	  bfd_mach_o_symtab_command *sym = &mdata->commands[i].command.symtab;

	  if (bfd_mach_o_scan_read_symtab_symbols (abfd, &mdata->commands[i].command.symtab) != 0)
	    {
	      fprintf (stderr, "bfd_mach_o_canonicalize_symtab: unable to load symbols for section %lu\n", i);
	      return 0;
	    }

	  BFD_ASSERT (sym->symbols != NULL);

	  for (j = 0; j < sym->nsyms; j++)
	    {
	      BFD_ASSERT (csym < (alocation + nsyms));
	      *csym++ = &sym->symbols[j];
	    }
	}
    }

  *csym++ = NULL;

  return nsyms;
}

void
bfd_mach_o_get_symbol_info (bfd *abfd ATTRIBUTE_UNUSED,
			    asymbol *symbol,
			    symbol_info *ret)
{
  bfd_symbol_info (symbol, ret);
}

void
bfd_mach_o_print_symbol (bfd *abfd,
			 PTR afile,
			 asymbol *symbol,
			 bfd_print_symbol_type how)
{
  FILE *file = (FILE *) afile;
  unsigned char ntype;
  unsigned char nsect;
  unsigned int ndesc;
  const char *name;

  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    default:
      bfd_print_symbol_vandf (abfd, (PTR) file, symbol);
      ntype = BFD_MACH_O_SYM_NTYPE (symbol);
      nsect = BFD_MACH_O_SYM_NSECT (symbol);
      ndesc = BFD_MACH_O_SYM_NDESC (symbol);
      if (ntype & BFD_MACH_O_N_STAB)
	name = bfd_get_stab_name (ntype);
      else
	switch (ntype & BFD_MACH_O_N_TYPE)
	  {
	  case BFD_MACH_O_N_UNDF:
	    name = "UND";
	    break;
	  case BFD_MACH_O_N_ABS:
	    name = "ABS";
	    break;
	  case BFD_MACH_O_N_INDR:
	    name = "INDR";
	    break;
	  case BFD_MACH_O_N_PBUD:
	    name = "PBUD";
	    break;
	  case BFD_MACH_O_N_SECT:
	    name = "SECT";
	    break;
	  default:
	    name = "???";
	    break;
	  }
      if (name == NULL)
	name = "";
      fprintf (file, " %02x %-6s %02x %04x", ntype, name, nsect, ndesc);
      if ((ntype & BFD_MACH_O_N_STAB) == 0
	  && (ntype & BFD_MACH_O_N_TYPE) == BFD_MACH_O_N_SECT)
	fprintf (file, " %-5s", symbol->section->name);
      fprintf (file, " %s", symbol->name);
    }
}

static void
bfd_mach_o_convert_architecture (bfd_mach_o_cpu_type mtype,
				 bfd_mach_o_cpu_subtype msubtype ATTRIBUTE_UNUSED,
				 enum bfd_architecture *type,
				 unsigned long *subtype)
{
  *subtype = bfd_arch_unknown;

  switch (mtype)
    {
    case BFD_MACH_O_CPU_TYPE_VAX: *type = bfd_arch_vax; break;
    case BFD_MACH_O_CPU_TYPE_MC680x0: *type = bfd_arch_m68k; break;
    case BFD_MACH_O_CPU_TYPE_I386:
      *type = bfd_arch_i386;
      *subtype = bfd_mach_i386_i386;
      break;
    case BFD_MACH_O_CPU_TYPE_X86_64:
      *type = bfd_arch_i386;
      *subtype = bfd_mach_x86_64;
      break;
    case BFD_MACH_O_CPU_TYPE_MIPS: *type = bfd_arch_mips; break;
    case BFD_MACH_O_CPU_TYPE_MC98000: *type = bfd_arch_m98k; break;
    case BFD_MACH_O_CPU_TYPE_HPPA: *type = bfd_arch_hppa; break;
    case BFD_MACH_O_CPU_TYPE_ARM: *type = bfd_arch_arm; break;
    case BFD_MACH_O_CPU_TYPE_MC88000: *type = bfd_arch_m88k; break;
    case BFD_MACH_O_CPU_TYPE_SPARC:
      *type = bfd_arch_sparc;
      *subtype = bfd_mach_sparc;
      break;
    case BFD_MACH_O_CPU_TYPE_I860: *type = bfd_arch_i860; break;
    case BFD_MACH_O_CPU_TYPE_ALPHA: *type = bfd_arch_alpha; break;
    case BFD_MACH_O_CPU_TYPE_POWERPC:
      *type = bfd_arch_powerpc;
      *subtype = bfd_mach_ppc; 
      break;
    case BFD_MACH_O_CPU_TYPE_POWERPC_64:
      *type = bfd_arch_powerpc;
      *subtype = bfd_mach_ppc64; 
      break;
    default:
      *type = bfd_arch_unknown;
      break;
    }
}

static bfd_boolean
bfd_mach_o_write_header (bfd *abfd, bfd_mach_o_header *header)
{
  unsigned char buf[32];
  unsigned int size;

  size = (header->version == 2) ? 
    BFD_MACH_O_HEADER_64_SIZE : BFD_MACH_O_HEADER_SIZE;

  bfd_h_put_32 (abfd, header->magic, buf + 0);
  bfd_h_put_32 (abfd, header->cputype, buf + 4);
  bfd_h_put_32 (abfd, header->cpusubtype, buf + 8);
  bfd_h_put_32 (abfd, header->filetype, buf + 12);
  bfd_h_put_32 (abfd, header->ncmds, buf + 16);
  bfd_h_put_32 (abfd, header->sizeofcmds, buf + 20);
  bfd_h_put_32 (abfd, header->flags, buf + 24);

  if (header->version == 2)
    bfd_h_put_32 (abfd, header->reserved, buf + 28);

  bfd_seek (abfd, 0, SEEK_SET);
  if (bfd_bwrite ((PTR) buf, size, abfd) != size)
    return FALSE;

  return TRUE;
}

static int
bfd_mach_o_scan_write_thread (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_thread_command *cmd = &command->command.thread;
  unsigned int i;
  unsigned char buf[8];
  bfd_vma offset;
  unsigned int nflavours;

  BFD_ASSERT ((command->type == BFD_MACH_O_LC_THREAD)
	      || (command->type == BFD_MACH_O_LC_UNIXTHREAD));

  offset = 8;
  nflavours = 0;
  for (i = 0; i < cmd->nflavours; i++)
    {
      BFD_ASSERT ((cmd->flavours[i].size % 4) == 0);
      BFD_ASSERT (cmd->flavours[i].offset == (command->offset + offset + 8));

      bfd_h_put_32 (abfd, cmd->flavours[i].flavour, buf);
      bfd_h_put_32 (abfd, (cmd->flavours[i].size / 4), buf + 4);

      bfd_seek (abfd, command->offset + offset, SEEK_SET);
      if (bfd_bwrite ((PTR) buf, 8, abfd) != 8)
	return -1;

      offset += cmd->flavours[i].size + 8;
    }

  return 0;
}

static int
bfd_mach_o_scan_write_section_32 (bfd *abfd,
				  bfd_mach_o_section *section,
				  bfd_vma offset)
{
  unsigned char buf[BFD_MACH_O_SECTION_SIZE];

  memcpy (buf, section->sectname, 16);
  memcpy (buf + 16, section->segname, 16);
  bfd_h_put_32 (abfd, section->addr, buf + 32);
  bfd_h_put_32 (abfd, section->size, buf + 36);
  bfd_h_put_32 (abfd, section->offset, buf + 40);
  bfd_h_put_32 (abfd, section->align, buf + 44);
  bfd_h_put_32 (abfd, section->reloff, buf + 48);
  bfd_h_put_32 (abfd, section->nreloc, buf + 52);
  bfd_h_put_32 (abfd, section->flags, buf + 56);
  bfd_h_put_32 (abfd, section->reserved1, buf + 60);
  bfd_h_put_32 (abfd, section->reserved2, buf + 64);

  bfd_seek (abfd, offset, SEEK_SET);
  if (bfd_bwrite ((PTR) buf, BFD_MACH_O_SECTION_SIZE, abfd)
      != BFD_MACH_O_SECTION_SIZE)
    return -1;

  return 0;
}

static int
bfd_mach_o_scan_write_section_64 (bfd *abfd,
				  bfd_mach_o_section *section,
				  bfd_vma offset)
{
  unsigned char buf[BFD_MACH_O_SECTION_64_SIZE];

  memcpy (buf, section->sectname, 16);
  memcpy (buf + 16, section->segname, 16);
  bfd_h_put_64 (abfd, section->addr, buf + 32);
  bfd_h_put_64 (abfd, section->size, buf + 40);
  bfd_h_put_32 (abfd, section->offset, buf + 48);
  bfd_h_put_32 (abfd, section->align, buf + 52);
  bfd_h_put_32 (abfd, section->reloff, buf + 56);
  bfd_h_put_32 (abfd, section->nreloc, buf + 60);
  bfd_h_put_32 (abfd, section->flags, buf + 64);
  bfd_h_put_32 (abfd, section->reserved1, buf + 68);
  bfd_h_put_32 (abfd, section->reserved2, buf + 72);
  bfd_h_put_32 (abfd, section->reserved3, buf + 76);

  bfd_seek (abfd, offset, SEEK_SET);
  if (bfd_bwrite ((PTR) buf, BFD_MACH_O_SECTION_64_SIZE, abfd)
      != BFD_MACH_O_SECTION_64_SIZE)
    return -1;

  return 0;
}

static int
bfd_mach_o_scan_write_section (bfd *abfd, 
			       bfd_mach_o_section *section,
			       bfd_vma offset,
			       unsigned int wide)
{
  if (wide)
    return bfd_mach_o_scan_write_section_64 (abfd, section, offset);
  else
    return bfd_mach_o_scan_write_section_32 (abfd, section, offset);
}

static int
bfd_mach_o_scan_write_segment (bfd *abfd,
			       bfd_mach_o_load_command *command,
			       unsigned int wide)
{
  unsigned char buf[64];
  bfd_mach_o_segment_command *seg = &command->command.segment;
  unsigned long i;

  if (wide)
    {
      BFD_ASSERT (command->type == BFD_MACH_O_LC_SEGMENT_64);
      
      memcpy (buf, seg->segname, 16);

      bfd_h_put_64 (abfd, seg->vmaddr, buf + 16);
      bfd_h_put_64 (abfd, seg->vmsize, buf + 24);
      bfd_h_put_64 (abfd, seg->fileoff, buf + 32);
      bfd_h_put_64 (abfd, seg->filesize, buf + 40);
      bfd_h_put_32 (abfd, seg->maxprot, buf + 48);
      bfd_h_put_32 (abfd, seg->initprot, buf + 52);
      bfd_h_put_32 (abfd, seg->nsects, buf + 56);
      bfd_h_put_32 (abfd, seg->flags, buf + 60);

      bfd_seek (abfd, command->offset + 8, SEEK_SET);
      if (bfd_bwrite ((PTR) buf, 64, abfd) != 64)
	return -1;
    }
  else
    {
      BFD_ASSERT (command->type == BFD_MACH_O_LC_SEGMENT);
      
      memcpy (buf, seg->segname, 16);

      bfd_h_put_32 (abfd, seg->vmaddr, buf + 16);
      bfd_h_put_32 (abfd, seg->vmsize, buf + 20);
      bfd_h_put_32 (abfd, seg->fileoff, buf + 24);
      bfd_h_put_32 (abfd, seg->filesize, buf + 28);
      bfd_h_put_32 (abfd, seg->maxprot, buf + 32);
      bfd_h_put_32 (abfd, seg->initprot, buf + 36);
      bfd_h_put_32 (abfd, seg->nsects, buf + 40);
      bfd_h_put_32 (abfd, seg->flags, buf + 44);

      bfd_seek (abfd, command->offset + 8, SEEK_SET);
      if (bfd_bwrite ((PTR) buf, 48, abfd) != 48)
	return -1;
    }

  for (i = 0; i < seg->nsects; i++)
    {
      bfd_vma segoff;
      if (wide)
	segoff = command->offset + BFD_MACH_O_LC_SEGMENT_64_SIZE 
          + (i * BFD_MACH_O_SECTION_64_SIZE);
      else
	segoff = command->offset + BFD_MACH_O_LC_SEGMENT_SIZE 
          + (i * BFD_MACH_O_SECTION_SIZE);

      if (bfd_mach_o_scan_write_section
	  (abfd, &seg->sections[i], segoff, wide) != 0)
	return -1;
    }

  return 0;
}

static int
bfd_mach_o_scan_write_segment_32 (bfd *abfd, bfd_mach_o_load_command *command)
{
  return bfd_mach_o_scan_write_segment (abfd, command, 0);
}

static int
bfd_mach_o_scan_write_segment_64 (bfd *abfd, bfd_mach_o_load_command *command)
{
  return bfd_mach_o_scan_write_segment (abfd, command, 1);
}

static int
bfd_mach_o_scan_write_symtab_symbols (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_symtab_command *sym = &command->command.symtab;
  asymbol *s = NULL;
  unsigned long i;

  for (i = 0; i < sym->nsyms; i++)
    {
      unsigned char buf[12];
      bfd_vma symoff = sym->symoff + (i * 12);
      unsigned char ntype = 0;
      unsigned char nsect = 0;
      short ndesc = 0;

      s = &sym->symbols[i];

      /* Instead just set from the stored values.  */
      ntype = BFD_MACH_O_SYM_NTYPE (s);
      nsect = BFD_MACH_O_SYM_NSECT (s);
      ndesc = BFD_MACH_O_SYM_NDESC (s);

      bfd_h_put_32 (abfd, s->name - sym->strtab, buf);
      bfd_h_put_8 (abfd, ntype, buf + 4);
      bfd_h_put_8 (abfd, nsect, buf + 5);
      bfd_h_put_16 (abfd, ndesc, buf + 6);
      bfd_h_put_32 (abfd, s->section->vma + s->value, buf + 8);

      bfd_seek (abfd, symoff, SEEK_SET);
      if (bfd_bwrite ((PTR) buf, 12, abfd) != 12)
	{
	  fprintf (stderr, "bfd_mach_o_scan_write_symtab_symbols: unable to write %d bytes at %lu\n",
		   12, (unsigned long) symoff);
	  return -1;
	}
    }

  return 0;
}

static int
bfd_mach_o_scan_write_symtab (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_symtab_command *seg = &command->command.symtab;
  unsigned char buf[16];

  BFD_ASSERT (command->type == BFD_MACH_O_LC_SYMTAB);

  bfd_h_put_32 (abfd, seg->symoff, buf);
  bfd_h_put_32 (abfd, seg->nsyms, buf + 4);
  bfd_h_put_32 (abfd, seg->stroff, buf + 8);
  bfd_h_put_32 (abfd, seg->strsize, buf + 12);

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bwrite ((PTR) buf, 16, abfd) != 16)
    return -1;

  if (bfd_mach_o_scan_write_symtab_symbols (abfd, command) != 0)
    return -1;

  return 0;
}

bfd_boolean
bfd_mach_o_write_contents (bfd *abfd)
{
  unsigned int i;
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;

  /* Now write header information.  */
  if (!bfd_mach_o_write_header (abfd, &mdata->header))
    return FALSE;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      unsigned char buf[8];
      bfd_mach_o_load_command *cur = &mdata->commands[i];
      unsigned long typeflag;

      typeflag = cur->type | (cur->type_required ? BFD_MACH_O_LC_REQ_DYLD : 0);

      bfd_h_put_32 (abfd, typeflag, buf);
      bfd_h_put_32 (abfd, cur->len, buf + 4);

      bfd_seek (abfd, cur->offset, SEEK_SET);
      if (bfd_bwrite ((PTR) buf, 8, abfd) != 8)
	return FALSE;

      switch (cur->type)
	{
	case BFD_MACH_O_LC_SEGMENT:
	  if (bfd_mach_o_scan_write_segment_32 (abfd, cur) != 0)
	    return FALSE;
	  break;
	case BFD_MACH_O_LC_SEGMENT_64:
	  if (bfd_mach_o_scan_write_segment_64 (abfd, cur) != 0)
	    return FALSE;
	  break;
	case BFD_MACH_O_LC_SYMTAB:
	  if (bfd_mach_o_scan_write_symtab (abfd, cur) != 0)
	    return FALSE;
	  break;
	case BFD_MACH_O_LC_SYMSEG:
	  break;
	case BFD_MACH_O_LC_THREAD:
	case BFD_MACH_O_LC_UNIXTHREAD:
	  if (bfd_mach_o_scan_write_thread (abfd, cur) != 0)
	    return FALSE;
	  break;
	case BFD_MACH_O_LC_LOADFVMLIB:
	case BFD_MACH_O_LC_IDFVMLIB:
	case BFD_MACH_O_LC_IDENT:
	case BFD_MACH_O_LC_FVMFILE:
	case BFD_MACH_O_LC_PREPAGE:
	case BFD_MACH_O_LC_DYSYMTAB:
	case BFD_MACH_O_LC_LOAD_DYLIB:
	case BFD_MACH_O_LC_LOAD_WEAK_DYLIB:
	case BFD_MACH_O_LC_ID_DYLIB:
	case BFD_MACH_O_LC_LOAD_DYLINKER:
	case BFD_MACH_O_LC_ID_DYLINKER:
	case BFD_MACH_O_LC_PREBOUND_DYLIB:
	case BFD_MACH_O_LC_ROUTINES:
	case BFD_MACH_O_LC_SUB_FRAMEWORK:
	  break;
	default:
	  fprintf (stderr,
		   "unable to write unknown load command 0x%lx\n",
		   (unsigned long) cur->type);
	  return FALSE;
	}
    }

  return TRUE;
}

/* Build Mach-O load commands from the sections.  */

bfd_boolean
bfd_mach_o_build_commands (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = bfd_get_mach_o_data (abfd);
  unsigned int wide = (mdata->header.version == 2);
  bfd_mach_o_segment_command *seg;
  bfd_mach_o_section *sections;
  asection *sec;
  file_ptr filepos;

  /* Return now if commands are already built.  */
  if (mdata->header.ncmds)
    return FALSE;

  /* Very simple version: 1 command (segment) containing all sections.  */
  mdata->header.ncmds = 1;
  mdata->commands = bfd_alloc (abfd, mdata->header.ncmds 
                               * sizeof (bfd_mach_o_load_command));
  if (mdata->commands == NULL)
    return FALSE;
  seg = &mdata->commands[0].command.segment;
  seg->nsects = bfd_count_sections (abfd);
  sections = bfd_alloc (abfd, seg->nsects * sizeof (bfd_mach_o_section));
  if (sections == NULL)
    return FALSE;
  seg->sections = sections;

  /* Set segment command.  */
  if (wide)
    {
      mdata->commands[0].type = BFD_MACH_O_LC_SEGMENT_64;
      mdata->commands[0].offset = BFD_MACH_O_HEADER_64_SIZE;
      mdata->commands[0].len = BFD_MACH_O_LC_SEGMENT_64_SIZE
        + BFD_MACH_O_SECTION_64_SIZE * seg->nsects;
    }
  else
    {
      mdata->commands[0].type = BFD_MACH_O_LC_SEGMENT;
      mdata->commands[0].offset = BFD_MACH_O_HEADER_SIZE;
      mdata->commands[0].len = BFD_MACH_O_LC_SEGMENT_SIZE
        + BFD_MACH_O_SECTION_SIZE * seg->nsects;
    }
  mdata->commands[0].type_required = FALSE;
  mdata->header.sizeofcmds = mdata->commands[0].len;
      
  filepos = mdata->commands[0].offset + mdata->commands[0].len;

  memset (seg->segname, 0, sizeof (seg->segname));
  seg->vmaddr = 0;
  seg->fileoff = filepos;
  seg->filesize = 0;
  seg->maxprot = BFD_MACH_O_PROT_READ | BFD_MACH_O_PROT_WRITE
    | BFD_MACH_O_PROT_EXECUTE;
  seg->initprot = seg->maxprot;
  seg->flags = 0;

  /* Create Mach-O sections.  */
  for (sec = abfd->sections; sec; sec = sec->next)
    {
      sections->bfdsection = sec;
      bfd_mach_o_convert_section_name_to_mach_o (abfd, sec, sections);
      sections->addr = bfd_get_section_vma (abfd, sec);
      sections->size = bfd_get_section_size (sec);
      sections->align = bfd_get_section_alignment (abfd, sec);
      
      filepos = (filepos + ((file_ptr) 1 << sections->align) - 1)
        & ((file_ptr) -1 << sections->align);
      sections->offset = filepos;
      sections->reloff = 0;
      sections->nreloc = 0;
      sections->reserved1 = 0;
      sections->reserved2 = 0;
      sections->reserved3 = 0;

      sec->filepos = filepos;

      filepos += sections->size;
      sections++;
    }
  seg->filesize = filepos - seg->fileoff;
  seg->vmsize = seg->filesize;

  return TRUE;
}

/* Set the contents of a section.  */

bfd_boolean
bfd_mach_o_set_section_contents (bfd *abfd,
				 asection *section,
				 const void * location,
				 file_ptr offset,
				 bfd_size_type count)
{
  file_ptr pos;

  /* This must be done first, because bfd_set_section_contents is
     going to set output_has_begun to TRUE.  */
  if (! abfd->output_has_begun && ! bfd_mach_o_build_commands (abfd))
    return FALSE;

  if (count == 0)
    return TRUE;

  pos = section->filepos + offset;
  if (bfd_seek (abfd, pos, SEEK_SET) != 0
      || bfd_bwrite (location, count, abfd) != count)
    return FALSE;

  return TRUE;
}

int
bfd_mach_o_sizeof_headers (bfd *a ATTRIBUTE_UNUSED,
			   struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Make an empty symbol.  This is required only because
   bfd_make_section_anyway wants to create a symbol for the section.  */

asymbol *
bfd_mach_o_make_empty_symbol (bfd *abfd)
{
  asymbol *new;

  new = bfd_zalloc (abfd, sizeof (* new));
  if (new == NULL)
    return new;
  new->the_bfd = abfd;
  return new;
}

static bfd_boolean
bfd_mach_o_read_header (bfd *abfd, bfd_mach_o_header *header)
{
  unsigned char buf[32];
  unsigned int size;
  bfd_vma (*get32) (const void *) = NULL;

  /* Just read the magic number.  */
  bfd_seek (abfd, 0, SEEK_SET);
  if (bfd_bread ((PTR) buf, 4, abfd) != 4)
    return FALSE;

  if (bfd_getb32 (buf) == BFD_MACH_O_MH_MAGIC)
    {
      header->byteorder = BFD_ENDIAN_BIG;
      header->magic = BFD_MACH_O_MH_MAGIC;
      header->version = 1;
      get32 = bfd_getb32;
    }
  else if (bfd_getl32 (buf) == BFD_MACH_O_MH_MAGIC)
    {
      header->byteorder = BFD_ENDIAN_LITTLE;
      header->magic = BFD_MACH_O_MH_MAGIC;
      header->version = 1;
      get32 = bfd_getl32;
    }
  else if (bfd_getb32 (buf) == BFD_MACH_O_MH_MAGIC_64)
    {
      header->byteorder = BFD_ENDIAN_BIG;
      header->magic = BFD_MACH_O_MH_MAGIC_64;
      header->version = 2;
      get32 = bfd_getb32;
    }
  else if (bfd_getl32 (buf) == BFD_MACH_O_MH_MAGIC_64)
    {
      header->byteorder = BFD_ENDIAN_LITTLE;
      header->magic = BFD_MACH_O_MH_MAGIC_64;
      header->version = 2;
      get32 = bfd_getl32;
    }
  else
    {
      header->byteorder = BFD_ENDIAN_UNKNOWN;
      return FALSE;
    }

  /* Once the size of the header is known, read the full header.  */
  size = (header->version == 2) ?
    BFD_MACH_O_HEADER_64_SIZE : BFD_MACH_O_HEADER_SIZE;

  bfd_seek (abfd, 0, SEEK_SET);
  if (bfd_bread ((PTR) buf, size, abfd) != size)
    return FALSE;

  header->cputype = (*get32) (buf + 4);
  header->cpusubtype = (*get32) (buf + 8);
  header->filetype = (*get32) (buf + 12);
  header->ncmds = (*get32) (buf + 16);
  header->sizeofcmds = (*get32) (buf + 20);
  header->flags = (*get32) (buf + 24);

  if (header->version == 2)
    header->reserved = (*get32) (buf + 28);

  return TRUE;
}

static asection *
bfd_mach_o_make_bfd_section (bfd *abfd, bfd_mach_o_section *section,
			     unsigned long prot)
{
  asection *bfdsec;
  char *sname;
  flagword flags;

  sname = bfd_mach_o_convert_section_name_to_bfd (abfd, section);
  if (sname == NULL)
    return NULL;

  if (section->flags & BFD_MACH_O_S_ATTR_DEBUG)
    flags = SEC_HAS_CONTENTS | SEC_DEBUGGING;
  else
    {
      flags = SEC_ALLOC;
      if ((section->flags & BFD_MACH_O_SECTION_TYPE_MASK)
	  != BFD_MACH_O_S_ZEROFILL)
	{
	  flags |= SEC_HAS_CONTENTS | SEC_LOAD;
	  if (prot & BFD_MACH_O_PROT_EXECUTE)
	    flags |= SEC_CODE;
	  if (prot & BFD_MACH_O_PROT_WRITE)
	    flags |= SEC_DATA;
	  else if (prot & BFD_MACH_O_PROT_READ)
	    flags |= SEC_READONLY;
	}
    }
  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, flags);
  if (bfdsec == NULL)
    return NULL;

  bfdsec->vma = section->addr;
  bfdsec->lma = section->addr;
  bfdsec->size = section->size;
  bfdsec->filepos = section->offset;
  bfdsec->alignment_power = section->align;
  bfdsec->segment_mark = 0;

  return bfdsec;
}

static int
bfd_mach_o_scan_read_section_32 (bfd *abfd,
				 bfd_mach_o_section *section,
				 bfd_vma offset,
				 unsigned long prot)
{
  unsigned char buf[BFD_MACH_O_SECTION_SIZE];

  bfd_seek (abfd, offset, SEEK_SET);
  if (bfd_bread ((PTR) buf, BFD_MACH_O_SECTION_SIZE, abfd)
      != BFD_MACH_O_SECTION_SIZE)
    return -1;

  memcpy (section->sectname, buf, 16);
  section->sectname[16] = '\0';
  memcpy (section->segname, buf + 16, 16);
  section->segname[16] = '\0';
  section->addr = bfd_h_get_32 (abfd, buf + 32);
  section->size = bfd_h_get_32 (abfd, buf + 36);
  section->offset = bfd_h_get_32 (abfd, buf + 40);
  section->align = bfd_h_get_32 (abfd, buf + 44);
  section->reloff = bfd_h_get_32 (abfd, buf + 48);
  section->nreloc = bfd_h_get_32 (abfd, buf + 52);
  section->flags = bfd_h_get_32 (abfd, buf + 56);
  section->reserved1 = bfd_h_get_32 (abfd, buf + 60);
  section->reserved2 = bfd_h_get_32 (abfd, buf + 64);
  section->reserved3 = 0;
  section->bfdsection = bfd_mach_o_make_bfd_section (abfd, section, prot);

  if (section->bfdsection == NULL)
    return -1;

  return 0;
}

static int
bfd_mach_o_scan_read_section_64 (bfd *abfd,
				 bfd_mach_o_section *section,
				 bfd_vma offset,
				 unsigned long prot)
{
  unsigned char buf[BFD_MACH_O_SECTION_64_SIZE];

  bfd_seek (abfd, offset, SEEK_SET);
  if (bfd_bread ((PTR) buf, BFD_MACH_O_SECTION_64_SIZE, abfd)
      != BFD_MACH_O_SECTION_64_SIZE)
    return -1;

  memcpy (section->sectname, buf, 16);
  section->sectname[16] = '\0';
  memcpy (section->segname, buf + 16, 16);
  section->segname[16] = '\0';
  section->addr = bfd_h_get_64 (abfd, buf + 32);
  section->size = bfd_h_get_64 (abfd, buf + 40);
  section->offset = bfd_h_get_32 (abfd, buf + 48);
  section->align = bfd_h_get_32 (abfd, buf + 52);
  section->reloff = bfd_h_get_32 (abfd, buf + 56);
  section->nreloc = bfd_h_get_32 (abfd, buf + 60);
  section->flags = bfd_h_get_32 (abfd, buf + 64);
  section->reserved1 = bfd_h_get_32 (abfd, buf + 68);
  section->reserved2 = bfd_h_get_32 (abfd, buf + 72);
  section->reserved3 = bfd_h_get_32 (abfd, buf + 76);
  section->bfdsection = bfd_mach_o_make_bfd_section (abfd, section, prot);

  if (section->bfdsection == NULL)
    return -1;

  return 0;
}

static int
bfd_mach_o_scan_read_section (bfd *abfd,
			      bfd_mach_o_section *section,
			      bfd_vma offset,
			      unsigned long prot,
			      unsigned int wide)
{
  if (wide)
    return bfd_mach_o_scan_read_section_64 (abfd, section, offset, prot);
  else
    return bfd_mach_o_scan_read_section_32 (abfd, section, offset, prot);
}

int
bfd_mach_o_scan_read_symtab_symbol (bfd *abfd,
				    bfd_mach_o_symtab_command *sym,
				    asymbol *s,
				    unsigned long i)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  unsigned int wide = (mdata->header.version == 2);
  unsigned int symwidth = wide ? 16 : 12;
  bfd_vma symoff = sym->symoff + (i * symwidth);
  unsigned char buf[16];
  unsigned char type = -1;
  unsigned char section = -1;
  short desc = -1;
  symvalue value = -1;
  unsigned long stroff = -1;
  unsigned int symtype = -1;

  BFD_ASSERT (sym->strtab != NULL);

  bfd_seek (abfd, symoff, SEEK_SET);
  if (bfd_bread ((PTR) buf, symwidth, abfd) != symwidth)
    {
      fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbol: unable to read %d bytes at %lu\n",
	       symwidth, (unsigned long) symoff);
      return -1;
    }

  stroff = bfd_h_get_32 (abfd, buf);
  type = bfd_h_get_8 (abfd, buf + 4);
  symtype = (type & 0x0e);
  section = bfd_h_get_8 (abfd, buf + 5);
  desc = bfd_h_get_16 (abfd, buf + 6);
  if (wide)
    value = bfd_h_get_64 (abfd, buf + 8);
  else
    value = bfd_h_get_32 (abfd, buf + 8);

  if (stroff >= sym->strsize)
    {
      fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbol: symbol name out of range (%lu >= %lu)\n",
	       (unsigned long) stroff, (unsigned long) sym->strsize);
      return -1;
    }

  s->the_bfd = abfd;
  s->name = sym->strtab + stroff;
  s->value = value;
  s->udata.i = (type << 24) | (section << 16) | desc;
  s->flags = 0x0;

  if (type & BFD_MACH_O_N_STAB)
    {
      s->flags |= BSF_DEBUGGING;
      s->section = bfd_und_section_ptr;
      switch (type)
	{
	case N_FUN:
	case N_STSYM:
	case N_LCSYM:
	case N_BNSYM:
	case N_SLINE:
	case N_ENSYM:
	case N_ECOMM:
	case N_ECOML:
	case N_GSYM:
	  if ((section > 0) && (section <= mdata->nsects))
	    {
	      s->section = mdata->sections[section - 1]->bfdsection;
	      s->value = s->value - mdata->sections[section - 1]->addr;
	    }
	  break;
	}
    }
  else
    {
      if (type & BFD_MACH_O_N_PEXT)
	s->flags |= BSF_GLOBAL;
      
      if (type & BFD_MACH_O_N_EXT)
	s->flags |= BSF_GLOBAL;

      if (!(type & (BFD_MACH_O_N_PEXT | BFD_MACH_O_N_EXT)))
	s->flags |= BSF_LOCAL;

      switch (symtype)
	{
	case BFD_MACH_O_N_UNDF:
	  s->section = bfd_und_section_ptr;
	  break;
	case BFD_MACH_O_N_PBUD:
	  s->section = bfd_und_section_ptr;
	  break;
	case BFD_MACH_O_N_ABS:
	  s->section = bfd_abs_section_ptr;
	  break;
	case BFD_MACH_O_N_SECT:
	  if ((section > 0) && (section <= mdata->nsects))
	    {
	      s->section = mdata->sections[section - 1]->bfdsection;
	      s->value = s->value - mdata->sections[section - 1]->addr;
	    }
	  else
	    {
	      /* Mach-O uses 0 to mean "no section"; not an error.  */
	      if (section != 0)
		{
		  fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbol: "
			   "symbol \"%s\" specified invalid section %d (max %lu): setting to undefined\n",
			   s->name, section, mdata->nsects);
		}
	      s->section = bfd_und_section_ptr;
	    }
	  break;
	case BFD_MACH_O_N_INDR:
	  fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbol: "
		   "symbol \"%s\" is unsupported 'indirect' reference: setting to undefined\n",
		   s->name);
	  s->section = bfd_und_section_ptr;
	  break;
	default:
	  fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbol: "
		   "symbol \"%s\" specified invalid type field 0x%x: setting to undefined\n",
		   s->name, symtype);
	  s->section = bfd_und_section_ptr;
	  break;
	}
    }

  return 0;
}

int
bfd_mach_o_scan_read_symtab_strtab (bfd *abfd,
				    bfd_mach_o_symtab_command *sym)
{
  BFD_ASSERT (sym->strtab == NULL);

  if (abfd->flags & BFD_IN_MEMORY)
    {
      struct bfd_in_memory *b;

      b = (struct bfd_in_memory *) abfd->iostream;

      if ((sym->stroff + sym->strsize) > b->size)
	{
	  bfd_set_error (bfd_error_file_truncated);
	  return -1;
	}
      sym->strtab = (char *) b->buffer + sym->stroff;
      return 0;
    }

  sym->strtab = bfd_alloc (abfd, sym->strsize);
  if (sym->strtab == NULL)
    return -1;

  bfd_seek (abfd, sym->stroff, SEEK_SET);
  if (bfd_bread ((PTR) sym->strtab, sym->strsize, abfd) != sym->strsize)
    {
      fprintf (stderr, "bfd_mach_o_scan_read_symtab_strtab: unable to read %lu bytes at %lu\n",
	       sym->strsize, sym->stroff);
      return -1;
    }

  return 0;
}

int
bfd_mach_o_scan_read_symtab_symbols (bfd *abfd,
				     bfd_mach_o_symtab_command *sym)
{
  unsigned long i;
  int ret;

  BFD_ASSERT (sym->symbols == NULL);
  sym->symbols = bfd_alloc (abfd, sym->nsyms * sizeof (asymbol));

  if (sym->symbols == NULL)
    {
      fprintf (stderr, "bfd_mach_o_scan_read_symtab_symbols: unable to allocate memory for symbols\n");
      return -1;
    }

  ret = bfd_mach_o_scan_read_symtab_strtab (abfd, sym);
  if (ret != 0)
    return ret;

  for (i = 0; i < sym->nsyms; i++)
    {
      ret = bfd_mach_o_scan_read_symtab_symbol (abfd, sym, &sym->symbols[i], i);
      if (ret != 0)
	return ret;
    }

  return 0;
}

int
bfd_mach_o_scan_read_dysymtab_symbol (bfd *abfd,
				      bfd_mach_o_dysymtab_command *dysym,
				      bfd_mach_o_symtab_command *sym,
				      asymbol *s,
				      unsigned long i)
{
  unsigned long isymoff = dysym->indirectsymoff + (i * 4);
  unsigned long symindex;
  unsigned char buf[4];

  BFD_ASSERT (i < dysym->nindirectsyms);

  bfd_seek (abfd, isymoff, SEEK_SET);
  if (bfd_bread ((PTR) buf, 4, abfd) != 4)
    {
      fprintf (stderr, "bfd_mach_o_scan_read_dysymtab_symbol: unable to read %lu bytes at %lu\n",
	       (unsigned long) 4, isymoff);
      return -1;
    }
  symindex = bfd_h_get_32 (abfd, buf);

  return bfd_mach_o_scan_read_symtab_symbol (abfd, sym, s, symindex);
}

static const char *
bfd_mach_o_i386_flavour_string (unsigned int flavour)
{
  switch ((int) flavour)
    {
    case BFD_MACH_O_x86_THREAD_STATE32:    return "x86_THREAD_STATE32";
    case BFD_MACH_O_x86_FLOAT_STATE32:     return "x86_FLOAT_STATE32";
    case BFD_MACH_O_x86_EXCEPTION_STATE32: return "x86_EXCEPTION_STATE32";
    case BFD_MACH_O_x86_THREAD_STATE64:    return "x86_THREAD_STATE64";
    case BFD_MACH_O_x86_FLOAT_STATE64:     return "x86_FLOAT_STATE64";
    case BFD_MACH_O_x86_EXCEPTION_STATE64: return "x86_EXCEPTION_STATE64";
    case BFD_MACH_O_x86_THREAD_STATE:      return "x86_THREAD_STATE";
    case BFD_MACH_O_x86_FLOAT_STATE:       return "x86_FLOAT_STATE";
    case BFD_MACH_O_x86_EXCEPTION_STATE:   return "x86_EXCEPTION_STATE";
    case BFD_MACH_O_x86_DEBUG_STATE32:     return "x86_DEBUG_STATE32";
    case BFD_MACH_O_x86_DEBUG_STATE64:     return "x86_DEBUG_STATE64";
    case BFD_MACH_O_x86_DEBUG_STATE:       return "x86_DEBUG_STATE";
    case BFD_MACH_O_THREAD_STATE_NONE:     return "THREAD_STATE_NONE";
    default: return "UNKNOWN";
    }
}

static const char *
bfd_mach_o_ppc_flavour_string (unsigned int flavour)
{
  switch ((int) flavour)
    {
    case BFD_MACH_O_PPC_THREAD_STATE: return "PPC_THREAD_STATE";
    case BFD_MACH_O_PPC_FLOAT_STATE: return "PPC_FLOAT_STATE";
    case BFD_MACH_O_PPC_EXCEPTION_STATE: return "PPC_EXCEPTION_STATE";
    case BFD_MACH_O_PPC_VECTOR_STATE: return "PPC_VECTOR_STATE";
    default: return "UNKNOWN";
    }
}

static int
bfd_mach_o_scan_read_dylinker (bfd *abfd,
			       bfd_mach_o_load_command *command)
{
  bfd_mach_o_dylinker_command *cmd = &command->command.dylinker;
  unsigned char buf[4];
  unsigned int nameoff;
  asection *bfdsec;
  char *sname;
  const char *prefix;

  BFD_ASSERT ((command->type == BFD_MACH_O_LC_ID_DYLINKER)
	      || (command->type == BFD_MACH_O_LC_LOAD_DYLINKER));

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bread ((PTR) buf, 4, abfd) != 4)
    return -1;

  nameoff = bfd_h_get_32 (abfd, buf + 0);

  cmd->name_offset = command->offset + nameoff;
  cmd->name_len = command->len - nameoff;

  if (command->type == BFD_MACH_O_LC_LOAD_DYLINKER)
    prefix = "LC_LOAD_DYLINKER";
  else if (command->type == BFD_MACH_O_LC_ID_DYLINKER)
    prefix = "LC_ID_DYLINKER";
  else
    abort ();

  sname = bfd_alloc (abfd, strlen (prefix) + 1);
  if (sname == NULL)
    return -1;
  strcpy (sname, prefix);

  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, SEC_HAS_CONTENTS);
  if (bfdsec == NULL)
    return -1;

  bfdsec->vma = 0;
  bfdsec->lma = 0;
  bfdsec->size = command->len - nameoff;
  bfdsec->filepos = command->offset + nameoff;
  bfdsec->alignment_power = 0;

  cmd->section = bfdsec;

  return 0;
}

static int
bfd_mach_o_scan_read_dylib (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_dylib_command *cmd = &command->command.dylib;
  unsigned char buf[16];
  unsigned int nameoff;
  asection *bfdsec;
  char *sname;
  const char *prefix;

  BFD_ASSERT ((command->type == BFD_MACH_O_LC_ID_DYLIB)
	      || (command->type == BFD_MACH_O_LC_LOAD_DYLIB)
	      || (command->type == BFD_MACH_O_LC_LOAD_WEAK_DYLIB));

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bread ((PTR) buf, 16, abfd) != 16)
    return -1;

  nameoff = bfd_h_get_32 (abfd, buf + 0);
  cmd->timestamp = bfd_h_get_32 (abfd, buf + 4);
  cmd->current_version = bfd_h_get_32 (abfd, buf + 8);
  cmd->compatibility_version = bfd_h_get_32 (abfd, buf + 12);

  cmd->name_offset = command->offset + nameoff;
  cmd->name_len = command->len - nameoff;

  if (command->type == BFD_MACH_O_LC_LOAD_DYLIB)
    prefix = "LC_LOAD_DYLIB";
  else if (command->type == BFD_MACH_O_LC_LOAD_WEAK_DYLIB)
    prefix = "LC_LOAD_WEAK_DYLIB";
  else if (command->type == BFD_MACH_O_LC_ID_DYLIB)
    prefix = "LC_ID_DYLIB";
  else
    abort ();

  sname = bfd_alloc (abfd, strlen (prefix) + 1);
  if (sname == NULL)
    return -1;
  strcpy (sname, prefix);

  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, SEC_HAS_CONTENTS);
  if (bfdsec == NULL)
    return -1;

  bfdsec->vma = 0;
  bfdsec->lma = 0;
  bfdsec->size = command->len - 8;
  bfdsec->filepos = command->offset + 8;
  bfdsec->alignment_power = 0;

  cmd->section = bfdsec;

  return 0;
}

static int
bfd_mach_o_scan_read_prebound_dylib (bfd *abfd ATTRIBUTE_UNUSED,
				     bfd_mach_o_load_command *command ATTRIBUTE_UNUSED)
{
  /* bfd_mach_o_prebound_dylib_command *cmd = &command->command.prebound_dylib; */

  BFD_ASSERT (command->type == BFD_MACH_O_LC_PREBOUND_DYLIB);
  return 0;
}

static int
bfd_mach_o_scan_read_thread (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_data_struct *mdata = NULL;
  bfd_mach_o_thread_command *cmd = &command->command.thread;
  unsigned char buf[8];
  bfd_vma offset;
  unsigned int nflavours;
  unsigned int i;

  BFD_ASSERT ((command->type == BFD_MACH_O_LC_THREAD)
	      || (command->type == BFD_MACH_O_LC_UNIXTHREAD));

  BFD_ASSERT (bfd_mach_o_valid (abfd));
  mdata = abfd->tdata.mach_o_data;

  offset = 8;
  nflavours = 0;
  while (offset != command->len)
    {
      if (offset >= command->len)
	return -1;

      bfd_seek (abfd, command->offset + offset, SEEK_SET);

      if (bfd_bread ((PTR) buf, 8, abfd) != 8)
	return -1;

      offset += 8 + bfd_h_get_32 (abfd, buf + 4) * 4;
      nflavours++;
    }

  cmd->flavours = bfd_alloc (abfd, nflavours * sizeof (bfd_mach_o_thread_flavour));
  if (cmd->flavours == NULL)
    return -1;
  cmd->nflavours = nflavours;

  offset = 8;
  nflavours = 0;
  while (offset != command->len)
    {
      if (offset >= command->len)
	return -1;

      if (nflavours >= cmd->nflavours)
	return -1;

      bfd_seek (abfd, command->offset + offset, SEEK_SET);

      if (bfd_bread ((PTR) buf, 8, abfd) != 8)
	return -1;

      cmd->flavours[nflavours].flavour = bfd_h_get_32 (abfd, buf);
      cmd->flavours[nflavours].offset = command->offset + offset + 8;
      cmd->flavours[nflavours].size = bfd_h_get_32 (abfd, buf + 4) * 4;
      offset += cmd->flavours[nflavours].size + 8;
      nflavours++;
    }

  for (i = 0; i < nflavours; i++)
    {
      asection *bfdsec;
      unsigned int snamelen;
      char *sname;
      const char *flavourstr;
      const char *prefix = "LC_THREAD";
      unsigned int j = 0;

      switch (mdata->header.cputype)
	{
	case BFD_MACH_O_CPU_TYPE_POWERPC:
	case BFD_MACH_O_CPU_TYPE_POWERPC_64:
	  flavourstr = bfd_mach_o_ppc_flavour_string (cmd->flavours[i].flavour);
	  break;
	case BFD_MACH_O_CPU_TYPE_I386:
	case BFD_MACH_O_CPU_TYPE_X86_64:
	  flavourstr = bfd_mach_o_i386_flavour_string (cmd->flavours[i].flavour);
	  break;
	default:
	  flavourstr = "UNKNOWN_ARCHITECTURE";
	  break;
	}

      snamelen = strlen (prefix) + 1 + 20 + 1 + strlen (flavourstr) + 1;
      sname = bfd_alloc (abfd, snamelen);
      if (sname == NULL)
	return -1;

      for (;;)
	{
	  sprintf (sname, "%s.%s.%u", prefix, flavourstr, j);
	  if (bfd_get_section_by_name (abfd, sname) == NULL)
	    break;
	  j++;
	}

      bfdsec = bfd_make_section_with_flags (abfd, sname, SEC_HAS_CONTENTS);

      bfdsec->vma = 0;
      bfdsec->lma = 0;
      bfdsec->size = cmd->flavours[i].size;
      bfdsec->filepos = cmd->flavours[i].offset;
      bfdsec->alignment_power = 0x0;

      cmd->section = bfdsec;
    }

  return 0;
}

static int
bfd_mach_o_scan_read_dysymtab (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_dysymtab_command *seg = &command->command.dysymtab;
  unsigned char buf[72];

  BFD_ASSERT (command->type == BFD_MACH_O_LC_DYSYMTAB);

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bread ((PTR) buf, 72, abfd) != 72)
    return -1;

  seg->ilocalsym = bfd_h_get_32 (abfd, buf + 0);
  seg->nlocalsym = bfd_h_get_32 (abfd, buf + 4);
  seg->iextdefsym = bfd_h_get_32 (abfd, buf + 8);
  seg->nextdefsym = bfd_h_get_32 (abfd, buf + 12);
  seg->iundefsym = bfd_h_get_32 (abfd, buf + 16);
  seg->nundefsym = bfd_h_get_32 (abfd, buf + 20);
  seg->tocoff = bfd_h_get_32 (abfd, buf + 24);
  seg->ntoc = bfd_h_get_32 (abfd, buf + 28);
  seg->modtaboff = bfd_h_get_32 (abfd, buf + 32);
  seg->nmodtab = bfd_h_get_32 (abfd, buf + 36);
  seg->extrefsymoff = bfd_h_get_32 (abfd, buf + 40);
  seg->nextrefsyms = bfd_h_get_32 (abfd, buf + 44);
  seg->indirectsymoff = bfd_h_get_32 (abfd, buf + 48);
  seg->nindirectsyms = bfd_h_get_32 (abfd, buf + 52);
  seg->extreloff = bfd_h_get_32 (abfd, buf + 56);
  seg->nextrel = bfd_h_get_32 (abfd, buf + 60);
  seg->locreloff = bfd_h_get_32 (abfd, buf + 64);
  seg->nlocrel = bfd_h_get_32 (abfd, buf + 68);

  return 0;
}

static int
bfd_mach_o_scan_read_symtab (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_symtab_command *seg = &command->command.symtab;
  unsigned char buf[16];
  asection *bfdsec;
  char *sname;
  const char *prefix = "LC_SYMTAB.stabs";
  int nlist_size = (bfd_mach_o_version (abfd) > 1) ? 16 : 12;

  BFD_ASSERT (command->type == BFD_MACH_O_LC_SYMTAB);

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bread ((PTR) buf, 16, abfd) != 16)
    return -1;

  seg->symoff = bfd_h_get_32 (abfd, buf);
  seg->nsyms = bfd_h_get_32 (abfd, buf + 4);
  seg->stroff = bfd_h_get_32 (abfd, buf + 8);
  seg->strsize = bfd_h_get_32 (abfd, buf + 12);
  seg->symbols = NULL;
  seg->strtab = NULL;

  sname = bfd_alloc (abfd, strlen (prefix) + 1);
  if (sname == NULL)
    return -1;
  strcpy (sname, prefix);

  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, SEC_HAS_CONTENTS);
  if (bfdsec == NULL)
    return -1;

  bfdsec->vma = 0;
  bfdsec->lma = 0;
  bfdsec->size = seg->nsyms * nlist_size;
  bfdsec->filepos = seg->symoff;
  bfdsec->alignment_power = 0;

  seg->stabs_segment = bfdsec;

  if (seg->nsyms != 0)
    abfd->flags |= HAS_SYMS;

  prefix = "LC_SYMTAB.stabstr";
  sname = bfd_alloc (abfd, strlen (prefix) + 1);
  if (sname == NULL)
    return -1;
  strcpy (sname, prefix);

  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, SEC_HAS_CONTENTS);
  if (bfdsec == NULL)
    return -1;

  bfdsec->vma = 0;
  bfdsec->lma = 0;
  bfdsec->size = seg->strsize;
  bfdsec->filepos = seg->stroff;
  bfdsec->alignment_power = 0;

  seg->stabstr_segment = bfdsec;

  return 0;
}

static int
bfd_mach_o_scan_read_uuid (bfd *abfd, bfd_mach_o_load_command *command)
{
  bfd_mach_o_uuid_command *cmd = &command->command.uuid;
  asection *bfdsec;
  char *sname;
  static const char prefix[] = "LC_UUID";

  BFD_ASSERT (command->type == BFD_MACH_O_LC_UUID);

  bfd_seek (abfd, command->offset + 8, SEEK_SET);
  if (bfd_bread ((PTR) cmd->uuid, 16, abfd) != 16)
    return -1;

  sname = bfd_alloc (abfd, strlen (prefix) + 1);
  if (sname == NULL)
    return -1;
  strcpy (sname, prefix);

  bfdsec = bfd_make_section_anyway_with_flags (abfd, sname, SEC_HAS_CONTENTS);
  if (bfdsec == NULL)
    return -1;

  bfdsec->vma = 0;
  bfdsec->lma = 0;
  bfdsec->size = command->len - 8;
  bfdsec->filepos = command->offset + 8;
  bfdsec->alignment_power = 0;

  cmd->section = bfdsec;

  return 0;
}

static int
bfd_mach_o_scan_read_segment (bfd *abfd,
			      bfd_mach_o_load_command *command,
			      unsigned int wide)
{
  unsigned char buf[64];
  bfd_mach_o_segment_command *seg = &command->command.segment;
  unsigned long i;

  if (wide)
    {
      BFD_ASSERT (command->type == BFD_MACH_O_LC_SEGMENT_64);

      bfd_seek (abfd, command->offset + 8, SEEK_SET);
      if (bfd_bread ((PTR) buf, 64, abfd) != 64)
	return -1;

      memcpy (seg->segname, buf, 16);
      seg->segname[16] = '\0';

      seg->vmaddr = bfd_h_get_64 (abfd, buf + 16);
      seg->vmsize = bfd_h_get_64 (abfd, buf + 24);
      seg->fileoff = bfd_h_get_64 (abfd, buf + 32);
      seg->filesize = bfd_h_get_64 (abfd, buf + 40);
      seg->maxprot = bfd_h_get_32 (abfd, buf + 48);
      seg->initprot = bfd_h_get_32 (abfd, buf + 52);
      seg->nsects = bfd_h_get_32 (abfd, buf + 56);
      seg->flags = bfd_h_get_32 (abfd, buf + 60);
    }
  else
    {
      BFD_ASSERT (command->type == BFD_MACH_O_LC_SEGMENT);

      bfd_seek (abfd, command->offset + 8, SEEK_SET);
      if (bfd_bread ((PTR) buf, 48, abfd) != 48)
	return -1;

      memcpy (seg->segname, buf, 16);
      seg->segname[16] = '\0';

      seg->vmaddr = bfd_h_get_32 (abfd, buf + 16);
      seg->vmsize = bfd_h_get_32 (abfd, buf + 20);
      seg->fileoff = bfd_h_get_32 (abfd, buf + 24);
      seg->filesize = bfd_h_get_32 (abfd, buf +  28);
      seg->maxprot = bfd_h_get_32 (abfd, buf + 32);
      seg->initprot = bfd_h_get_32 (abfd, buf + 36);
      seg->nsects = bfd_h_get_32 (abfd, buf + 40);
      seg->flags = bfd_h_get_32 (abfd, buf + 44);
    }

  if (seg->nsects != 0)
    {
      seg->sections = bfd_alloc (abfd, seg->nsects
                                 * sizeof (bfd_mach_o_section));
      if (seg->sections == NULL)
	return -1;

      for (i = 0; i < seg->nsects; i++)
	{
	  bfd_vma segoff;
          if (wide)
            segoff = command->offset + BFD_MACH_O_LC_SEGMENT_64_SIZE 
              + (i * BFD_MACH_O_SECTION_64_SIZE);
          else
            segoff = command->offset + BFD_MACH_O_LC_SEGMENT_SIZE 
              + (i * BFD_MACH_O_SECTION_SIZE);

	  if (bfd_mach_o_scan_read_section
	      (abfd, &seg->sections[i], segoff, seg->initprot, wide) != 0)
	    return -1;
	}
    }

  return 0;
}

static int
bfd_mach_o_scan_read_segment_32 (bfd *abfd, bfd_mach_o_load_command *command)
{
  return bfd_mach_o_scan_read_segment (abfd, command, 0);
}

static int
bfd_mach_o_scan_read_segment_64 (bfd *abfd, bfd_mach_o_load_command *command)
{
  return bfd_mach_o_scan_read_segment (abfd, command, 1);
}

static int
bfd_mach_o_scan_read_command (bfd *abfd, bfd_mach_o_load_command *command)
{
  unsigned char buf[8];

  bfd_seek (abfd, command->offset, SEEK_SET);
  if (bfd_bread ((PTR) buf, 8, abfd) != 8)
    return -1;

  command->type = bfd_h_get_32 (abfd, buf) & ~BFD_MACH_O_LC_REQ_DYLD;
  command->type_required = (bfd_h_get_32 (abfd, buf) & BFD_MACH_O_LC_REQ_DYLD
			    ? TRUE : FALSE);
  command->len = bfd_h_get_32 (abfd, buf + 4);

  switch (command->type)
    {
    case BFD_MACH_O_LC_SEGMENT:
      if (bfd_mach_o_scan_read_segment_32 (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_SEGMENT_64:
      if (bfd_mach_o_scan_read_segment_64 (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_SYMTAB:
      if (bfd_mach_o_scan_read_symtab (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_SYMSEG:
      break;
    case BFD_MACH_O_LC_THREAD:
    case BFD_MACH_O_LC_UNIXTHREAD:
      if (bfd_mach_o_scan_read_thread (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_LOAD_DYLINKER:
    case BFD_MACH_O_LC_ID_DYLINKER:
      if (bfd_mach_o_scan_read_dylinker (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_LOAD_DYLIB:
    case BFD_MACH_O_LC_ID_DYLIB:
    case BFD_MACH_O_LC_LOAD_WEAK_DYLIB:
      if (bfd_mach_o_scan_read_dylib (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_PREBOUND_DYLIB:
      if (bfd_mach_o_scan_read_prebound_dylib (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_LOADFVMLIB:
    case BFD_MACH_O_LC_IDFVMLIB:
    case BFD_MACH_O_LC_IDENT:
    case BFD_MACH_O_LC_FVMFILE:
    case BFD_MACH_O_LC_PREPAGE:
    case BFD_MACH_O_LC_ROUTINES:
    case BFD_MACH_O_LC_SUB_FRAMEWORK:
      break;
    case BFD_MACH_O_LC_DYSYMTAB:
      if (bfd_mach_o_scan_read_dysymtab (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_SUB_UMBRELLA:
    case BFD_MACH_O_LC_SUB_CLIENT:
    case BFD_MACH_O_LC_SUB_LIBRARY:
    case BFD_MACH_O_LC_TWOLEVEL_HINTS:
    case BFD_MACH_O_LC_PREBIND_CKSUM:
      break;
    case BFD_MACH_O_LC_UUID:
      if (bfd_mach_o_scan_read_uuid (abfd, command) != 0)
	return -1;
      break;
    case BFD_MACH_O_LC_CODE_SIGNATURE:
    case BFD_MACH_O_LC_SEGMENT_SPLIT_INFO:
    case BFD_MACH_O_LC_REEXPORT_DYLIB:
      break;
    default:
      fprintf (stderr, "unable to read unknown load command 0x%lx\n",
	       (unsigned long) command->type);
      break;
    }

  return 0;
}

static void
bfd_mach_o_flatten_sections (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  long csect = 0;
  unsigned long i, j;

  /* Count total number of sections.  */
  mdata->nsects = 0;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      if (mdata->commands[i].type == BFD_MACH_O_LC_SEGMENT
	  || mdata->commands[i].type == BFD_MACH_O_LC_SEGMENT_64)
	{
	  bfd_mach_o_segment_command *seg;

	  seg = &mdata->commands[i].command.segment;
	  mdata->nsects += seg->nsects;
	}
    }

  /* Allocate sections array.  */
  mdata->sections = bfd_alloc (abfd,
			       mdata->nsects * sizeof (bfd_mach_o_section *));

  /* Fill the array.  */
  csect = 0;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      if (mdata->commands[i].type == BFD_MACH_O_LC_SEGMENT
	  || mdata->commands[i].type == BFD_MACH_O_LC_SEGMENT_64)
	{
	  bfd_mach_o_segment_command *seg;

	  seg = &mdata->commands[i].command.segment;
	  BFD_ASSERT (csect + seg->nsects <= mdata->nsects);

	  for (j = 0; j < seg->nsects; j++)
	    mdata->sections[csect++] = &seg->sections[j];
	}
    }
}

int
bfd_mach_o_scan_start_address (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  bfd_mach_o_thread_command *cmd = NULL;
  unsigned long i;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      if ((mdata->commands[i].type == BFD_MACH_O_LC_THREAD) ||
	  (mdata->commands[i].type == BFD_MACH_O_LC_UNIXTHREAD))
	{
	  if (cmd == NULL)
	    cmd = &mdata->commands[i].command.thread;
	  else
	    return 0;
	}
    }

  if (cmd == NULL)
    return 0;

  for (i = 0; i < cmd->nflavours; i++)
    {
      if ((mdata->header.cputype == BFD_MACH_O_CPU_TYPE_I386)
	  && (cmd->flavours[i].flavour
	      == (unsigned long) BFD_MACH_O_x86_THREAD_STATE32))
	{
	  unsigned char buf[4];

	  bfd_seek (abfd, cmd->flavours[i].offset + 40, SEEK_SET);

	  if (bfd_bread (buf, 4, abfd) != 4)
	    return -1;

	  abfd->start_address = bfd_h_get_32 (abfd, buf);
	}
      else if ((mdata->header.cputype == BFD_MACH_O_CPU_TYPE_POWERPC)
	       && (cmd->flavours[i].flavour == BFD_MACH_O_PPC_THREAD_STATE))
	{
	  unsigned char buf[4];

	  bfd_seek (abfd, cmd->flavours[i].offset + 0, SEEK_SET);

	  if (bfd_bread (buf, 4, abfd) != 4)
	    return -1;

	  abfd->start_address = bfd_h_get_32 (abfd, buf);
	}
      else if ((mdata->header.cputype == BFD_MACH_O_CPU_TYPE_POWERPC_64)
               && (cmd->flavours[i].flavour == BFD_MACH_O_PPC_THREAD_STATE_64))
        {
          unsigned char buf[8];

          bfd_seek (abfd, cmd->flavours[i].offset + 0, SEEK_SET);

          if (bfd_bread (buf, 8, abfd) != 8)
            return -1;

          abfd->start_address = bfd_h_get_64 (abfd, buf);
        }
      else if ((mdata->header.cputype == BFD_MACH_O_CPU_TYPE_X86_64)
               && (cmd->flavours[i].flavour == BFD_MACH_O_x86_THREAD_STATE64))
        {
          unsigned char buf[8];

          bfd_seek (abfd, cmd->flavours[i].offset + (16 * 8), SEEK_SET);

          if (bfd_bread (buf, 8, abfd) != 8)
            return -1;

          abfd->start_address = bfd_h_get_64 (abfd, buf);
        }
    }

  return 0;
}

int
bfd_mach_o_scan (bfd *abfd,
		 bfd_mach_o_header *header,
		 bfd_mach_o_data_struct *mdata)
{
  unsigned int i;
  enum bfd_architecture cputype;
  unsigned long cpusubtype;
  unsigned int hdrsize;

  hdrsize = (header->version == 2) ? 
    BFD_MACH_O_HEADER_64_SIZE : BFD_MACH_O_HEADER_SIZE;

  mdata->header = *header;
  mdata->symbols = NULL;

  abfd->flags = abfd->flags & BFD_IN_MEMORY;
  switch (header->filetype)
    {
    case BFD_MACH_O_MH_OBJECT:
      abfd->flags |= HAS_RELOC;
      break;
    case BFD_MACH_O_MH_EXECUTE:
      abfd->flags |= EXEC_P;
      break;
    case BFD_MACH_O_MH_DYLIB:
    case BFD_MACH_O_MH_BUNDLE:
      abfd->flags |= DYNAMIC;
      break;
    }

  abfd->tdata.mach_o_data = mdata;

  bfd_mach_o_convert_architecture (header->cputype, header->cpusubtype,
				   &cputype, &cpusubtype);
  if (cputype == bfd_arch_unknown)
    {
      fprintf (stderr, "bfd_mach_o_scan: unknown architecture 0x%lx/0x%lx\n",
	       header->cputype, header->cpusubtype);
      return -1;
    }

  bfd_set_arch_mach (abfd, cputype, cpusubtype);

  if (header->ncmds != 0)
    {
      mdata->commands = bfd_alloc (abfd, header->ncmds * sizeof (bfd_mach_o_load_command));
      if (mdata->commands == NULL)
	return -1;

      for (i = 0; i < header->ncmds; i++)
	{
	  bfd_mach_o_load_command *cur = &mdata->commands[i];

	  if (i == 0)
	    cur->offset = hdrsize;
	  else
	    {
	      bfd_mach_o_load_command *prev = &mdata->commands[i - 1];
	      cur->offset = prev->offset + prev->len;
	    }

	  if (bfd_mach_o_scan_read_command (abfd, cur) < 0)
	    return -1;
	}
    }

  if (bfd_mach_o_scan_start_address (abfd) < 0)
    return -1;

  bfd_mach_o_flatten_sections (abfd);
  return 0;
}

bfd_boolean
bfd_mach_o_mkobject_init (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata = NULL;

  mdata = bfd_alloc (abfd, sizeof (bfd_mach_o_data_struct));
  if (mdata == NULL)
    return FALSE;
  abfd->tdata.mach_o_data = mdata;

  mdata->header.magic = 0;
  mdata->header.cputype = 0;
  mdata->header.cpusubtype = 0;
  mdata->header.filetype = 0;
  mdata->header.ncmds = 0;
  mdata->header.sizeofcmds = 0;
  mdata->header.flags = 0;
  mdata->header.byteorder = BFD_ENDIAN_UNKNOWN;
  mdata->commands = NULL;
  mdata->nsymbols = 0;
  mdata->symbols = NULL;
  mdata->nsects = 0;
  mdata->sections = NULL;
  mdata->ibfd = NULL;

  return TRUE;
}

const bfd_target *
bfd_mach_o_header_p (bfd *abfd,
                     bfd_mach_o_filetype filetype,
                     bfd_mach_o_cpu_type cputype)
{
  struct bfd_preserve preserve;
  bfd_mach_o_header header;

  preserve.marker = NULL;
  if (!bfd_mach_o_read_header (abfd, &header))
    goto wrong;

  if (! (header.byteorder == BFD_ENDIAN_BIG
	 || header.byteorder == BFD_ENDIAN_LITTLE))
    {
      fprintf (stderr, "unknown header byte-order value 0x%lx\n",
	       (unsigned long) header.byteorder);
      goto wrong;
    }

  if (! ((header.byteorder == BFD_ENDIAN_BIG
	  && abfd->xvec->byteorder == BFD_ENDIAN_BIG
	  && abfd->xvec->header_byteorder == BFD_ENDIAN_BIG)
	 || (header.byteorder == BFD_ENDIAN_LITTLE
	     && abfd->xvec->byteorder == BFD_ENDIAN_LITTLE
	     && abfd->xvec->header_byteorder == BFD_ENDIAN_LITTLE)))
    goto wrong;

  /* Check cputype and filetype.
     In case of wildcard, do not accept magics that are handled by existing
     targets.  */
  if (cputype)
    {
      if (header.cputype != cputype)
        goto wrong;
    }
  else
    {
      switch (header.cputype)
        {
        case BFD_MACH_O_CPU_TYPE_I386:
          /* Handled by mach-o-i386 */
          goto wrong;
        default:
          break;
        }
    }
  if (filetype)
    {
      if (header.filetype != filetype)
        goto wrong;
    }
  else
    {
      switch (header.filetype)
        {
        case BFD_MACH_O_MH_CORE:
          /* Handled by core_p */
          goto wrong;
        default:
          break;
        }
    }

  preserve.marker = bfd_zalloc (abfd, sizeof (bfd_mach_o_data_struct));
  if (preserve.marker == NULL
      || !bfd_preserve_save (abfd, &preserve))
    goto fail;

  if (bfd_mach_o_scan (abfd, &header,
		       (bfd_mach_o_data_struct *) preserve.marker) != 0)
    goto wrong;

  bfd_preserve_finish (abfd, &preserve);
  return abfd->xvec;

 wrong:
  bfd_set_error (bfd_error_wrong_format);

 fail:
  if (preserve.marker != NULL)
    bfd_preserve_restore (abfd, &preserve);
  return NULL;
}

static const bfd_target *
bfd_mach_o_gen_object_p (bfd *abfd)
{
  return bfd_mach_o_header_p (abfd, 0, 0);
}

static const bfd_target *
bfd_mach_o_gen_core_p (bfd *abfd)
{
  return bfd_mach_o_header_p (abfd, BFD_MACH_O_MH_CORE, 0);
}

typedef struct mach_o_fat_archentry
{
  unsigned long cputype;
  unsigned long cpusubtype;
  unsigned long offset;
  unsigned long size;
  unsigned long align;
} mach_o_fat_archentry;

typedef struct mach_o_fat_data_struct
{
  unsigned long magic;
  unsigned long nfat_arch;
  mach_o_fat_archentry *archentries;
} mach_o_fat_data_struct;

const bfd_target *
bfd_mach_o_archive_p (bfd *abfd)
{
  mach_o_fat_data_struct *adata = NULL;
  unsigned char buf[20];
  unsigned long i;

  bfd_seek (abfd, 0, SEEK_SET);
  if (bfd_bread ((PTR) buf, 8, abfd) != 8)
    goto error;

  adata = bfd_alloc (abfd, sizeof (mach_o_fat_data_struct));
  if (adata == NULL)
    goto error;

  adata->magic = bfd_getb32 (buf);
  adata->nfat_arch = bfd_getb32 (buf + 4);
  if (adata->magic != 0xcafebabe)
    goto error;
  /* Avoid matching Java bytecode files, which have the same magic number.
     In the Java bytecode file format this field contains the JVM version,
     which starts at 43.0.  */
  if (adata->nfat_arch > 30)
    goto error;

  adata->archentries = 
    bfd_alloc (abfd, adata->nfat_arch * sizeof (mach_o_fat_archentry));
  if (adata->archentries == NULL)
    goto error;

  for (i = 0; i < adata->nfat_arch; i++)
    {
      bfd_seek (abfd, 8 + 20 * i, SEEK_SET);

      if (bfd_bread ((PTR) buf, 20, abfd) != 20)
	goto error;
      adata->archentries[i].cputype = bfd_getb32 (buf);
      adata->archentries[i].cpusubtype = bfd_getb32 (buf + 4);
      adata->archentries[i].offset = bfd_getb32 (buf + 8);
      adata->archentries[i].size = bfd_getb32 (buf + 12);
      adata->archentries[i].align = bfd_getb32 (buf + 16);
    }

  abfd->tdata.mach_o_fat_data = adata;
  return abfd->xvec;

 error:
  if (adata != NULL)
    bfd_release (abfd, adata);
  bfd_set_error (bfd_error_wrong_format);
  return NULL;
}

bfd *
bfd_mach_o_openr_next_archived_file (bfd *archive, bfd *prev)
{
  mach_o_fat_data_struct *adata;
  mach_o_fat_archentry *entry = NULL;
  unsigned long i;
  bfd *nbfd;
  enum bfd_architecture arch_type;
  unsigned long arch_subtype;

  adata = (mach_o_fat_data_struct *) archive->tdata.mach_o_fat_data;
  BFD_ASSERT (adata != NULL);

  /* Find index of previous entry.  */
  if (prev == NULL)
    i = 0;	/* Start at first one.  */
  else
    {
      for (i = 0; i < adata->nfat_arch; i++)
	{
	  if (adata->archentries[i].offset == prev->origin)
	    break;
	}

      if (i == adata->nfat_arch)
	{
	  /* Not found.  */
	  bfd_set_error (bfd_error_bad_value);
	  return NULL;
	}
    i++;	/* Get next entry.  */
  }

  if (i >= adata->nfat_arch)
    {
      bfd_set_error (bfd_error_no_more_archived_files);
      return NULL;
    }

  entry = &adata->archentries[i];
  nbfd = _bfd_new_bfd_contained_in (archive);
  if (nbfd == NULL)
    return NULL;

  nbfd->origin = entry->offset;

  bfd_mach_o_convert_architecture (entry->cputype, entry->cpusubtype,
				   &arch_type, &arch_subtype);
  /* Create the member filename.
     Use FILENAME:ARCH_NAME.  */
  {
    char *s = NULL;
    const char *arch_name;
    size_t arch_file_len = strlen (bfd_get_filename (archive));

    arch_name = bfd_printable_arch_mach (arch_type, arch_subtype);
    s = bfd_malloc (arch_file_len + 1 + strlen (arch_name) + 1);
    if (s == NULL)
      return NULL;
    memcpy (s, bfd_get_filename (archive), arch_file_len);
    s[arch_file_len] = ':';
    strcpy (s + arch_file_len + 1, arch_name);
    nbfd->filename = s;
  }
  nbfd->iostream = NULL;
  bfd_set_arch_mach (nbfd, arch_type, arch_subtype);

  return nbfd;
}

/* If ABFD format is FORMAT and architecture is ARCH, return it.
   If ABFD is a fat image containing a member that corresponds to FORMAT
   and ARCH, returns it.
   In other case, returns NULL.
   This function allows transparent uses of fat images.  */
bfd *
bfd_mach_o_fat_extract (bfd *abfd,
			bfd_format format,
			const bfd_arch_info_type *arch)
{
  bfd *res;
  mach_o_fat_data_struct *adata;
  unsigned int i;

  if (bfd_check_format (abfd, format))
    {
      if (bfd_get_arch_info (abfd) == arch)
	return abfd;
      return NULL;
    }
  if (!bfd_check_format (abfd, bfd_archive)
      || abfd->xvec != &mach_o_fat_vec)
    return NULL;
  
  /* This is a Mach-O fat image.  */
  adata = (mach_o_fat_data_struct *) abfd->tdata.mach_o_fat_data;
  BFD_ASSERT (adata != NULL);

  for (i = 0; i < adata->nfat_arch; i++)
    {
      struct mach_o_fat_archentry *e = &adata->archentries[i];
      enum bfd_architecture cpu_type;
      unsigned long cpu_subtype;

      bfd_mach_o_convert_architecture (e->cputype, e->cpusubtype,
				       &cpu_type, &cpu_subtype);
      if (cpu_type != arch->arch || cpu_subtype != arch->mach)
	continue;

      /* The architecture is found.  */
      res = _bfd_new_bfd_contained_in (abfd);
      if (res == NULL)
	return NULL;

      res->origin = e->offset;

      res->filename = strdup (abfd->filename);
      res->iostream = NULL;

      if (bfd_check_format (res, format))
	{
	  BFD_ASSERT (bfd_get_arch_info (res) == arch);
	  return res;
	}
      bfd_close (res);
      return NULL;
    }

  return NULL;
}

int
bfd_mach_o_lookup_section (bfd *abfd,
			   asection *section,
			   bfd_mach_o_load_command **mcommand,
			   bfd_mach_o_section **msection)
{
  struct mach_o_data_struct *md = abfd->tdata.mach_o_data;
  unsigned int i, j, num;

  bfd_mach_o_load_command *ncmd = NULL;
  bfd_mach_o_section *nsect = NULL;

  BFD_ASSERT (mcommand != NULL);
  BFD_ASSERT (msection != NULL);

  num = 0;
  for (i = 0; i < md->header.ncmds; i++)
    {
      struct bfd_mach_o_load_command *cmd = &md->commands[i];
      struct bfd_mach_o_segment_command *seg = NULL;

      if (cmd->type != BFD_MACH_O_LC_SEGMENT
	  || cmd->type != BFD_MACH_O_LC_SEGMENT_64)
	continue;
      seg = &cmd->command.segment;

      if (seg->segment == section)
	{
	  if (num == 0)
	    ncmd = cmd;
	  num++;
	}

      for (j = 0; j < seg->nsects; j++)
	{
	  struct bfd_mach_o_section *sect = &seg->sections[j];

	  if (sect->bfdsection == section)
	    {
	      if (num == 0)
		nsect = sect;
	      num++;
	    }
	}
    }

  *mcommand = ncmd;
  *msection = nsect;
  return num;
}

int
bfd_mach_o_lookup_command (bfd *abfd,
			   bfd_mach_o_load_command_type type,
			   bfd_mach_o_load_command **mcommand)
{
  struct mach_o_data_struct *md = NULL;
  bfd_mach_o_load_command *ncmd = NULL;
  unsigned int i, num;

  md = abfd->tdata.mach_o_data;

  BFD_ASSERT (md != NULL);
  BFD_ASSERT (mcommand != NULL);

  num = 0;
  for (i = 0; i < md->header.ncmds; i++)
    {
      struct bfd_mach_o_load_command *cmd = &md->commands[i];

      if (cmd->type != type)
	continue;

      if (num == 0)
	ncmd = cmd;
      num++;
    }

  *mcommand = ncmd;
  return num;
}

unsigned long
bfd_mach_o_stack_addr (enum bfd_mach_o_cpu_type type)
{
  switch (type)
    {
    case BFD_MACH_O_CPU_TYPE_MC680x0:
      return 0x04000000;
    case BFD_MACH_O_CPU_TYPE_MC88000:
      return 0xffffe000;
    case BFD_MACH_O_CPU_TYPE_POWERPC:
      return 0xc0000000;
    case BFD_MACH_O_CPU_TYPE_I386:
      return 0xc0000000;
    case BFD_MACH_O_CPU_TYPE_SPARC:
      return 0xf0000000;
    case BFD_MACH_O_CPU_TYPE_I860:
      return 0;
    case BFD_MACH_O_CPU_TYPE_HPPA:
      return 0xc0000000 - 0x04000000;
    default:
      return 0;
    }
}

bfd_boolean
bfd_mach_o_bfd_print_private_bfd_data (bfd *abfd, PTR ptr)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  FILE *file = (FILE *) ptr;
  unsigned int i, j;
  unsigned int sec_nbr = 0;

  fprintf (file, _("Segments and Sections:\n"));
  fprintf (file, _(" #: Segment name     Section name     Address\n"));

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      bfd_mach_o_segment_command *seg;

      if (mdata->commands[i].type != BFD_MACH_O_LC_SEGMENT
	  && mdata->commands[i].type != BFD_MACH_O_LC_SEGMENT_64)
	continue;

      seg = &mdata->commands[i].command.segment;

      fprintf (file, "[Segment %-16s ", seg->segname);
      fprintf_vma (file, seg->vmaddr);
      fprintf (file, "-");
      fprintf_vma  (file, seg->vmaddr + seg->vmsize - 1);
      fputc (' ', file);
      fputc (seg->initprot & BFD_MACH_O_PROT_READ ? 'r' : '-', file);
      fputc (seg->initprot & BFD_MACH_O_PROT_WRITE ? 'w' : '-', file);
      fputc (seg->initprot & BFD_MACH_O_PROT_EXECUTE ? 'x' : '-', file);
      fprintf (file, "]\n");
      for (j = 0; j < seg->nsects; j++)
	{
	  bfd_mach_o_section *sec = &seg->sections[j];
	  fprintf (file, "%02u: %-16s %-16s ", ++sec_nbr,
		   sec->segname, sec->sectname);
	  fprintf_vma (file, sec->addr);
	  fprintf (file, " ");
	  fprintf_vma  (file, sec->size);
	  fprintf (file, " %08lx\n", sec->flags);
	}
    }

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      bfd_mach_o_load_command *cmd = &mdata->commands[i];

      switch (cmd->type)
	{
	case BFD_MACH_O_LC_SEGMENT:
	case BFD_MACH_O_LC_SEGMENT_64:
	  break;
	case BFD_MACH_O_LC_UUID:
	  {
	    bfd_mach_o_uuid_command *uuid = &cmd->command.uuid;
	    unsigned int i;

	    fprintf (file, "\n"
		     "UUID:");
	    for (i = 0; i < sizeof (uuid->uuid); i++)
	      fprintf (file, " %02x", uuid->uuid[i]);
	    fputc ('\n', file);
	  }
	  break;
	case BFD_MACH_O_LC_LOAD_DYLIB:
	  {
	    bfd_mach_o_dylib_command *dylib = &cmd->command.dylib;
	    bfd_byte *data = NULL;

	    if (! bfd_malloc_and_get_section (abfd, dylib->section, &data))
	      {
		if (data != NULL)
		  free (data);
		break;
	      }
	    fprintf (file, "\n"
		     "LOAD_DYLIB: %s\n",
		     data + dylib->name_offset - cmd->offset - 8);
	    fprintf (file, "            time stamp: 0x%08lx\n",
		     dylib->timestamp);
	    fprintf (file, "       current version: 0x%08lx\n",
		     dylib->current_version);
	    fprintf (file, "  comptibility version: 0x%08lx\n",
		     dylib->compatibility_version);
	    free (data);
	    break;
	  }
	case BFD_MACH_O_LC_LOAD_DYLINKER:
	  {
	    bfd_mach_o_dylinker_command *linker = &cmd->command.dylinker;
	    bfd_byte *data = NULL;

	    if (! bfd_malloc_and_get_section (abfd, linker->section, &data))
	      {
		if (data != NULL)
		  free (data);
		break;
	      }
	    fprintf (file, "\n"
		     "LOAD_DYLINKER: %s\n",
		     data + linker->name_offset - cmd->offset - 8);
	    free (data);
	    break;
	  }
	case BFD_MACH_O_LC_SYMTAB:
	  {
	    bfd_mach_o_symtab_command *symtab = &cmd->command.symtab;
	    fprintf (file, "\n"
		     "LC_SYMTAB: nsyms: %lu, strsize: %lu\n",
		     symtab->nsyms, symtab->strsize);
	    break;
	  }
	case BFD_MACH_O_LC_DYSYMTAB:
	  {
	    bfd_mach_o_dysymtab_command *dysymtab = &cmd->command.dysymtab;
	    fprintf (file, "\n"
		     "LC_DYSYMTAB:\n"
		     "      local symbols: index: %lu  number: %lu\n",
		     dysymtab->ilocalsym, dysymtab->nlocalsym);
	    fprintf (file,
		     "   external symbols: index: %lu  number: %lu\n",
		     dysymtab->iextdefsym, dysymtab->nextdefsym);
	    fprintf (file,
		     "  undefined symbols: index: %lu  number: %lu\n",
		     dysymtab->iundefsym, dysymtab->nundefsym);
	    fprintf (file,
		     "               ntoc: offset: %lu  number: %lu\n",
		     dysymtab->tocoff, dysymtab->ntoc);
	    fprintf (file,
		     "       module table: offset: %lu  number: %lu\n",
		     dysymtab->modtaboff, dysymtab->nmodtab);
	    break;
	  }
	default:
	  fprintf (file, "LC_%d\n", cmd->type);
	  break;
	}
    }

  return TRUE;
}

int
bfd_mach_o_core_fetch_environment (bfd *abfd,
				   unsigned char **rbuf,
				   unsigned int *rlen)
{
  bfd_mach_o_data_struct *mdata = abfd->tdata.mach_o_data;
  unsigned long stackaddr = bfd_mach_o_stack_addr (mdata->header.cputype);
  unsigned int i = 0;

  for (i = 0; i < mdata->header.ncmds; i++)
    {
      bfd_mach_o_load_command *cur = &mdata->commands[i];
      bfd_mach_o_segment_command *seg = NULL;

      if (cur->type != BFD_MACH_O_LC_SEGMENT)
	continue;

      seg = &cur->command.segment;

      if ((seg->vmaddr + seg->vmsize) == stackaddr)
	{
	  unsigned long start = seg->fileoff;
	  unsigned long end = seg->fileoff + seg->filesize;
	  unsigned char *buf = bfd_malloc (1024);
	  unsigned long size = 1024;

	  for (;;)
	    {
	      bfd_size_type nread = 0;
	      unsigned long offset;
	      int found_nonnull = 0;

	      if (size > (end - start))
		size = (end - start);

	      buf = bfd_realloc_or_free (buf, size);
	      if (buf == NULL)
		return -1;
	      
	      bfd_seek (abfd, end - size, SEEK_SET);
	      nread = bfd_bread (buf, size, abfd);

	      if (nread != size)
		{
		  free (buf);
		  return -1;
		}

	      for (offset = 4; offset <= size; offset += 4)
		{
		  unsigned long val;

		  val = *((unsigned long *) (buf + size - offset));
		  if (! found_nonnull)
		    {
		      if (val != 0)
			found_nonnull = 1;
		    }
		  else if (val == 0x0)
		    {
		      unsigned long bottom;
		      unsigned long top;

		      bottom = seg->fileoff + seg->filesize - offset;
		      top = seg->fileoff + seg->filesize - 4;
		      *rbuf = bfd_malloc (top - bottom);
		      *rlen = top - bottom;

		      memcpy (*rbuf, buf + size - *rlen, *rlen);
		      free (buf);
		      return 0;
		    }
		}

	      if (size == (end - start))
		break;

	      size *= 2;
	    }

	  free (buf);
	}
    }

  return -1;
}

char *
bfd_mach_o_core_file_failing_command (bfd *abfd)
{
  unsigned char *buf = NULL;
  unsigned int len = 0;
  int ret = -1;

  ret = bfd_mach_o_core_fetch_environment (abfd, &buf, &len);
  if (ret < 0)
    return NULL;

  return (char *) buf;
}

int
bfd_mach_o_core_file_failing_signal (bfd *abfd ATTRIBUTE_UNUSED)
{
  return 0;
}

#define TARGET_NAME 		mach_o_be_vec
#define TARGET_STRING     	"mach-o-be"
#define TARGET_BIG_ENDIAN 	1
#define TARGET_ARCHIVE 		0

#include "mach-o-target.c"

#undef TARGET_NAME
#undef TARGET_STRING
#undef TARGET_BIG_ENDIAN
#undef TARGET_ARCHIVE

#define TARGET_NAME 		mach_o_le_vec
#define TARGET_STRING 		"mach-o-le"
#define TARGET_BIG_ENDIAN 	0
#define TARGET_ARCHIVE 		0

#include "mach-o-target.c"

#undef TARGET_NAME
#undef TARGET_STRING
#undef TARGET_BIG_ENDIAN
#undef TARGET_ARCHIVE

#define TARGET_NAME 		mach_o_fat_vec
#define TARGET_STRING 		"mach-o-fat"
#define TARGET_BIG_ENDIAN 	1
#define TARGET_ARCHIVE 		1

#include "mach-o-target.c"

#undef TARGET_NAME
#undef TARGET_STRING
#undef TARGET_BIG_ENDIAN
#undef TARGET_ARCHIVE
