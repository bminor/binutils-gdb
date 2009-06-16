/* ELF STT_GNU_IFUNC support.
   Copyright 2009
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
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"
#define ARCH_SIZE 0
#include "elf-bfd.h"
#include "safe-ctype.h"
#include "libiberty.h"
#include "objalloc.h"

/* Create sections needed by STT_GNU_IFUNC symbol.  */

bfd_boolean
_bfd_elf_create_ifunc_sections (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags, pltflags;
  asection *s;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);
  struct elf_link_hash_table *htab = elf_hash_table (info);

  if (htab->irelifunc != NULL || htab->iplt != NULL)
    return TRUE;

  flags = bed->dynamic_sec_flags;
  pltflags = flags;
  if (bed->plt_not_loaded)
    /* We do not clear SEC_ALLOC here because we still want the OS to
       allocate space for the section; it's just that there's nothing
       to read in from the object file.  */
    pltflags &= ~ (SEC_CODE | SEC_LOAD | SEC_HAS_CONTENTS);
  else
    pltflags |= SEC_ALLOC | SEC_CODE | SEC_LOAD;
  if (bed->plt_readonly)
    pltflags |= SEC_READONLY;

  if (info->shared)
    {
      /* We need to create .rel[a].ifunc for shared objects.  */
      const char *rel_sec = (bed->rela_plts_and_copies_p
			     ? ".rela.ifunc" : ".rel.ifunc");

      s = bfd_make_section_with_flags (abfd, rel_sec,
				       flags | SEC_READONLY);
      if (s == NULL
	  || ! bfd_set_section_alignment (abfd, s,
					  bed->s->log_file_align))
	return FALSE;
      htab->irelifunc = s;
    }
  else
    {
      /* We need to create .iplt, .rel[a].iplt, .igot and .igot.plt
	 for static executables.   */
      s = bfd_make_section_with_flags (abfd, ".iplt", pltflags);
      if (s == NULL
	  || ! bfd_set_section_alignment (abfd, s, bed->plt_alignment))
	return FALSE;
      htab->iplt = s;

      s = bfd_make_section_with_flags (abfd,
				       (bed->rela_plts_and_copies_p
					? ".rela.iplt" : ".rel.iplt"),
				       flags | SEC_READONLY);
      if (s == NULL
	  || ! bfd_set_section_alignment (abfd, s,
					  bed->s->log_file_align))
	return FALSE;
      htab->irelplt = s;

      /* We don't need the .igot section if we have the .igot.plt
	 section.  */
      if (bed->want_got_plt)
	s = bfd_make_section_with_flags (abfd, ".igot.plt", flags);
      else
	s = bfd_make_section_with_flags (abfd, ".igot", flags);
      if (s == NULL
	  || !bfd_set_section_alignment (abfd, s,
					 bed->s->log_file_align))
	return FALSE;
      htab->igotplt = s;
    }

  return TRUE;
}
