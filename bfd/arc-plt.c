/* ARC-specific support for PLT relocations.
   Copyright (C) 2023 Free Software Foundation, Inc.
   Contributed by Cupertino Miranda (cmiranda@synopsys.com).

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

#include "arc-plt.h"

#define PLT_TYPE_START(NAME)			\
  const insn_hword NAME##_plt_entry[] = {
#define PLT_TYPE_END(NAME) };
#define PLT_ENTRY(...) __VA_ARGS__,
#define PLT_ELEM(...)
#define ENTRY_RELOC(...)
#define ELEM_RELOC(...)

#include "arc-plt.def"

#undef PLT_TYPE_START
#undef PLT_TYPE_END
#undef PLT_ENTRY
#undef PLT_ELEM
#undef ENTRY_RELOC
#undef ELEM_RELOC

#define PLT_TYPE_START(NAME)				\
  const struct plt_reloc NAME##_plt_entry_relocs[] = {
#define PLT_TYPE_END(NAME)   \
  {0, 0, 0, LAST_RELOC, 0}   \
  };
#define PLT_ENTRY(...)
#define PLT_ELEM(...)
#define ENTRY_RELOC(...) { __VA_ARGS__ },
#define ELEM_RELOC(...)

#include "arc-plt.def"

#undef PLT_TYPE_START
#undef PLT_TYPE_END
#undef PLT_ENTRY
#undef PLT_ELEM
#undef ENTRY_RELOC
#undef ELEM_RELOC


#define PLT_TYPE_START(NAME)			\
  const insn_hword NAME##_plt_elem[] = {
#define PLT_TYPE_END(NAME) };
#define PLT_ENTRY(...)
#define PLT_ELEM(...) __VA_ARGS__,
#define ENTRY_RELOC(...)
#define ELEM_RELOC(...)

#include "arc-plt.def"

#undef PLT_TYPE_START
#undef PLT_TYPE_END
#undef PLT_ENTRY
#undef PLT_ELEM
#undef ENTRY_RELOC
#undef ELEM_RELOC

#define PLT_TYPE_START(NAME)				\
  const struct plt_reloc NAME##_plt_elem_relocs[] = {
#define PLT_TYPE_END(NAME)   \
  {0, 0, 0, LAST_RELOC, 0}   \
  };
#define PLT_ENTRY(...)
#define PLT_ELEM(...)
#define ENTRY_RELOC(...)
#define ELEM_RELOC(...) { __VA_ARGS__ },

#include "arc-plt.def"

#undef PLT_TYPE_START
#undef PLT_TYPE_END
#undef PLT_ENTRY
#undef PLT_ELEM
#undef ENTRY_RELOC
#undef ELEM_RELOC


#define PLT_TYPE_START(NAME)			\
  {						\
    .entry = &NAME##_plt_entry,			\
    .entry_size = sizeof (NAME##_plt_entry),	\
    .elem = &NAME##_plt_elem,			\
    .elem_size = sizeof (NAME##_plt_elem),	\
    .entry_relocs = NAME##_plt_entry_relocs,	\
    .elem_relocs = NAME##_plt_elem_relocs
#define PLT_TYPE_END(NAME) },
#define PLT_ENTRY(...)
#define PLT_ELEM(...)
#define ENTRY_RELOC(...)
#define ELEM_RELOC(...)
struct plt_version_t plt_versions[PLT_MAX] =
  {
#include "arc-plt.def"
  };

#undef PLT_TYPE_START
#undef PLT_TYPE_END
#undef PLT_ENTRY
#undef PLT_ELEM
#undef ENTRY_RELOC
#undef ELEM_RELOC
