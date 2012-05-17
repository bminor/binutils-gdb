/* PowerPC-specific support for 64-bit ELF.
   Copyright 2003, 2005, 2007, 2009, 2012 Free Software Foundation, Inc.

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

enum ppc_elf_plt_type
{
  PLT_UNSET,
  PLT_OLD,
  PLT_NEW,
  PLT_VXWORKS
};

/* Rename some of the generic section flags to better document how they
   are used for ppc32.  These macros should be private to elf32-ppc.c,
   but opcodes/ppc-dis.c needs is_ppc_vle.  The flags are only valid
   for ppc32 elf objects.  */

/* Nonzero if this section has TLS related relocations.  */
#define has_tls_reloc sec_flg0

/* Nonzero if this section has a call to __tls_get_addr.  */
#define has_tls_get_addr_call sec_flg1

/* Nonzero if this section has the VLE bit set.  */
#define has_vle_insns sec_flg2

#define is_ppc_vle(SEC) \
  ((SEC)->owner != NULL						\
   && bfd_get_flavour ((SEC)->owner) == bfd_target_elf_flavour	\
   && elf_object_id ((SEC)->owner) == PPC32_ELF_DATA		\
   && (SEC)->has_vle_insns)

int ppc_elf_select_plt_layout (bfd *, struct bfd_link_info *,
			       enum ppc_elf_plt_type, int);
asection *ppc_elf_tls_setup (bfd *, struct bfd_link_info *, int);
bfd_boolean ppc_elf_tls_optimize (bfd *, struct bfd_link_info *);
void ppc_elf_set_sdata_syms (bfd *, struct bfd_link_info *);
extern bfd_boolean ppc_elf_modify_segment_map (bfd *,
		           struct bfd_link_info * ATTRIBUTE_UNUSED);
extern bfd_boolean ppc_elf_section_processing (bfd *, Elf_Internal_Shdr *);
