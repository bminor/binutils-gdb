// target.cc

// Copyright 2009 Free Software Foundation, Inc.
// Written by Doug Kwan <dougkwan@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"
#include "target.h"

namespace gold
{

// Return whether NAME is a local label name.  This is used to implement the
// --discard-locals options and can be overriden by children classes to
// implement system-specific behaviour.  The logic here is the same as that
// in _bfd_elf_is_local_label_name().

bool
Target::do_is_local_label_name (const char* name) const
{
  // Normal local symbols start with ``.L''.
  if (name[0] == '.' && name[1] == 'L')
    return true;

  // At least some SVR4 compilers (e.g., UnixWare 2.1 cc) generate
  // DWARF debugging symbols starting with ``..''.
  if (name[0] == '.' && name[1] == '.')
    return true;

  // gcc will sometimes generate symbols beginning with ``_.L_'' when
  // emitting DWARF debugging output.  I suspect this is actually a
  // small bug in gcc (it calls ASM_OUTPUT_LABEL when it should call
  // ASM_GENERATE_INTERNAL_LABEL, and this causes the leading
  // underscore to be emitted on some ELF targets).  For ease of use,
  // we treat such symbols as local.
  if (name[0] == '_' && name[1] == '.' && name[2] == 'L' && name[3] == '_')
    return true;

  return false;
}

} // End namespace gold.
