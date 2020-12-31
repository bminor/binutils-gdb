/* Emulation code used by all ELF targets.
   Copyright (C) 1991-2021 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

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
#include "ctf-api.h"
#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldexp.h"
#include "ldlang.h"
#include "elf-bfd.h"
#include "elf/internal.h"
#include "ldelfgen.h"

void
ldelf_map_segments (bfd_boolean need_layout)
{
  int tries = 10;

  do
    {
      lang_relax_sections (need_layout);
      need_layout = FALSE;

      if (link_info.output_bfd->xvec->flavour == bfd_target_elf_flavour
	  && !bfd_link_relocatable (&link_info))
	{
	  bfd_size_type phdr_size;

	  phdr_size = elf_program_header_size (link_info.output_bfd);
	  /* If we don't have user supplied phdrs, throw away any
	     previous linker generated program headers.  */
	  if (lang_phdr_list == NULL)
	    elf_seg_map (link_info.output_bfd) = NULL;
	  if (!_bfd_elf_map_sections_to_segments (link_info.output_bfd,
						  &link_info))
	    einfo (_("%F%P: map sections to segments failed: %E\n"));

	  if (phdr_size != elf_program_header_size (link_info.output_bfd))
	    {
	      if (tries > 6)
		/* The first few times we allow any change to
		   phdr_size .  */
		need_layout = TRUE;
	      else if (phdr_size
		       < elf_program_header_size (link_info.output_bfd))
		/* After that we only allow the size to grow.  */
		need_layout = TRUE;
	      else
		elf_program_header_size (link_info.output_bfd) = phdr_size;
	    }
	}
    }
  while (need_layout && --tries);

  if (tries == 0)
    einfo (_("%F%P: looping in map_segments"));

  if (link_info.output_bfd->xvec->flavour == bfd_target_elf_flavour
      && lang_phdr_list == NULL)
    {
      /* If we don't have user supplied phdrs, strip zero-sized dynamic
	 sections and regenerate program headers.  */
      const struct elf_backend_data *bed
	= get_elf_backend_data (link_info.output_bfd);
      if (bed->elf_backend_strip_zero_sized_dynamic_sections
	  && !bed->elf_backend_strip_zero_sized_dynamic_sections
		(&link_info))
	  einfo (_("%F%P: failed to strip zero-sized dynamic sections"));
    }
}

#ifdef ENABLE_LIBCTF
/* We want to emit CTF early if and only if we are not targetting ELF with this
   invocation.  */

int
ldelf_emit_ctf_early (void)
{
  if (bfd_get_flavour (link_info.output_bfd) == bfd_target_elf_flavour)
    return 0;
  return 1;
}

/* Callbacks used to map from bfd types to libctf types, under libctf's
   control.  */

struct ctf_strtab_iter_cb_arg
{
  struct elf_strtab_hash *strtab;
  size_t next_i;
  size_t next_idx;
};

/* Return strings from the strtab to libctf, one by one.  Returns NULL when
   iteration is complete.  */

static const char *
ldelf_ctf_strtab_iter_cb (uint32_t *offset, void *arg_)
{
  bfd_size_type off;
  const char *ret;

  struct ctf_strtab_iter_cb_arg *arg =
    (struct ctf_strtab_iter_cb_arg *) arg_;

  /* There is no zeroth string.  */
  if (arg->next_i == 0)
    arg->next_i = 1;

  if (arg->next_i >= _bfd_elf_strtab_len (arg->strtab))
    {
      arg->next_i = 0;
      return NULL;
    }

  ret = _bfd_elf_strtab_str (arg->strtab, arg->next_i++, &off);
  *offset = off;

  /* If we've overflowed, we cannot share any further strings: the CTF
     format cannot encode strings with such high offsets.  */
  if (*offset != off)
    return NULL;

  return ret;
}

void
ldelf_acquire_strings_for_ctf
  (struct ctf_dict *ctf_output, struct elf_strtab_hash *strtab)
{
  struct ctf_strtab_iter_cb_arg args = { strtab, 0, 0 };
  if (!ctf_output)
    return;

  if (bfd_get_flavour (link_info.output_bfd) == bfd_target_elf_flavour)
    {
      if (ctf_link_add_strtab (ctf_output, ldelf_ctf_strtab_iter_cb,
			       &args) < 0)
	einfo (_("%F%P: warning: CTF strtab association failed; strings will "
		 "not be shared: %s\n"),
	       ctf_errmsg (ctf_errno (ctf_output)));
    }
}

void
ldelf_new_dynsym_for_ctf (struct ctf_dict *ctf_output, int symidx,
			  struct elf_internal_sym *sym)
{
  ctf_link_sym_t lsym;

  if (!ctf_output)
     return;

  /* New symbol.  */
  if (sym != NULL)
    {
      lsym.st_name = NULL;
      lsym.st_nameidx = sym->st_name;
      lsym.st_nameidx_set = 1;
      lsym.st_symidx = symidx;
      lsym.st_shndx = sym->st_shndx;
      lsym.st_type = ELF_ST_TYPE (sym->st_info);
      lsym.st_value = sym->st_value;
      if (ctf_link_add_linker_symbol (ctf_output, &lsym) < 0)
	{
	  einfo (_("%F%P: warning: CTF symbol addition failed; CTF will "
		   "not be tied to symbols: %s\n"),
		 ctf_errmsg (ctf_errno (ctf_output)));
	}
    }
  else
    {
      /* Shuffle all the symbols.  */

      if (ctf_link_shuffle_syms (ctf_output) < 0)
	einfo (_("%F%P: warning: CTF symbol shuffling failed; CTF will "
		 "not be tied to symbols: %s\n"),
	       ctf_errmsg (ctf_errno (ctf_output)));
    }
}
#else
int
ldelf_emit_ctf_early (void)
{
  return 0;
}

void
ldelf_acquire_strings_for_ctf (struct ctf_dict *ctf_output ATTRIBUTE_UNUSED,
			       struct elf_strtab_hash *strtab ATTRIBUTE_UNUSED)
{}
void
ldelf_new_dynsym_for_ctf (struct ctf_dict *ctf_output ATTRIBUTE_UNUSED,
			  int symidx ATTRIBUTE_UNUSED,
			  struct elf_internal_sym *sym ATTRIBUTE_UNUSED)
{}
#endif
