/* simple.c -- BFD simple client routines
   Copyright 2002
   Free Software Foundation, Inc.
   Contributed by MontaVista Software, Inc.

   This file is part of BFD, the Binary File Descriptor library.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "bfdlink.h"

static boolean simple_dummy_warning
  PARAMS ((struct bfd_link_info *, const char *, const char *, bfd *,
	   asection *, bfd_vma));

static boolean simple_dummy_undefined_symbol
  PARAMS ((struct bfd_link_info *, const char *, bfd *, asection *,
	   bfd_vma, boolean));

static boolean simple_dummy_reloc_overflow 
  PARAMS ((struct bfd_link_info *, const char *, const char *, bfd_vma,
	   bfd *, asection *, bfd_vma));

static boolean simple_dummy_reloc_dangerous
  PARAMS ((struct bfd_link_info *, const char *, bfd *, asection *, bfd_vma));

static boolean simple_dummy_unattached_reloc
  PARAMS ((struct bfd_link_info *, const char *, bfd *, asection *, bfd_vma));

bfd_byte * bfd_simple_get_relocated_section_contents
  PARAMS ((bfd *, asection *, bfd_byte *));

static boolean
simple_dummy_warning (link_info, warning, symbol, abfd, section, address)
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED;
     const char *warning ATTRIBUTE_UNUSED;
     const char *symbol ATTRIBUTE_UNUSED;
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *section ATTRIBUTE_UNUSED;
     bfd_vma address ATTRIBUTE_UNUSED;
{
  return true;
}

static boolean
simple_dummy_undefined_symbol (link_info, name, abfd, section, address, fatal)
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED;
     const char *name ATTRIBUTE_UNUSED;
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *section ATTRIBUTE_UNUSED;
     bfd_vma address ATTRIBUTE_UNUSED;
     boolean fatal ATTRIBUTE_UNUSED;
{
  return true;
}

static boolean
simple_dummy_reloc_overflow (link_info, name, reloc_name, addend, abfd,
			     section, address)
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED;
     const char *name ATTRIBUTE_UNUSED;
     const char *reloc_name ATTRIBUTE_UNUSED;
     bfd_vma addend ATTRIBUTE_UNUSED;
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *section ATTRIBUTE_UNUSED;
     bfd_vma address ATTRIBUTE_UNUSED;
{
  return true;
}

static boolean
simple_dummy_reloc_dangerous (link_info, message, abfd, section, address)
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED;
     const char *message ATTRIBUTE_UNUSED;
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *section ATTRIBUTE_UNUSED;
     bfd_vma address ATTRIBUTE_UNUSED;
{
  return true;
}

static boolean
simple_dummy_unattached_reloc (link_info, name, abfd, section, address)
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED;
     const char *name ATTRIBUTE_UNUSED;
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *section ATTRIBUTE_UNUSED;
     bfd_vma address ATTRIBUTE_UNUSED;
{
  return true;
}

/*
FUNCTION
	bfd_simple_relocate_secton

SYNOPSIS
	bfd_byte *bfd_simple_get_relocated_section_contents (bfd *abfd, asection *sec, bfd_byte *outbuf);

DESCRIPTION
	Returns the relocated contents of section @var{sec}.  Only symbols
	from @var{abfd} and the output offsets assigned to sections in
	@var{abfd} are used.  The result will be stored at @var{outbuf}
	or allocated with @code{bfd_malloc} if @var{outbuf} is @code{NULL}.

	Generally all sections in @var{abfd} should have their
	@code{output_section} pointing back to the original section.

	Returns @code{NULL} on a fatal error; ignores errors applying
	particular relocations.
*/

bfd_byte *
bfd_simple_get_relocated_section_contents (abfd, sec, outbuf)
     bfd *abfd;
     asection *sec;
     bfd_byte *outbuf;
{
  struct bfd_link_info link_info;
  struct bfd_link_order link_order;
  struct bfd_link_callbacks callbacks;
  bfd_byte *contents, *data;
  int storage_needed, number_of_symbols;
  asymbol **symbol_table;

  /* In order to use bfd_get_relocated_section_contents, we need
     to forge some data structures that it expects.  */

  /* Fill in the bare minimum number of fields for our purposes.  */
  memset (&link_info, 0, sizeof (link_info));
  link_info.input_bfds = abfd;

  link_info.hash = bfd_link_hash_table_create (abfd);
  link_info.callbacks = &callbacks;
  callbacks.warning = simple_dummy_warning;
  callbacks.undefined_symbol = simple_dummy_undefined_symbol;
  callbacks.reloc_overflow = simple_dummy_reloc_overflow;
  callbacks.reloc_dangerous = simple_dummy_reloc_dangerous;
  callbacks.unattached_reloc = simple_dummy_unattached_reloc;

  memset (&link_order, 0, sizeof (link_order));
  link_order.next = NULL;
  link_order.type = bfd_indirect_link_order;
  link_order.offset = 0;
  link_order.size = bfd_section_size (abfd, sec);
  link_order.u.indirect.section = sec;

  data = NULL;
  if (outbuf == NULL)
    {
      data = bfd_malloc (bfd_section_size (abfd, sec));
      if (data == NULL)
	return NULL;
      outbuf = data;
    }
  bfd_link_add_symbols (abfd, &link_info);

  storage_needed = bfd_get_symtab_upper_bound (abfd);
  symbol_table = (asymbol **) bfd_malloc (storage_needed);
  number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

  contents = bfd_get_relocated_section_contents (abfd,
						 &link_info,
						 &link_order,
						 outbuf,
						 0,
						 symbol_table);
  if (contents == NULL && data != NULL)
    free (data);

  /* Foul hack to prevent bfd_section_size aborts.  This flag only controls
     that macro (and the related size macros), selecting between _raw_size
     and _cooked_size.  Debug sections won't change size while we're only
     relocating.  There may be trouble here someday if it tries to run
     relaxation unexpectedly, so make sure.  */
  BFD_ASSERT (sec->_raw_size == sec->_cooked_size);
  sec->reloc_done = 0;

  bfd_link_hash_table_free (abfd, link_info.hash);

  return contents;
}
