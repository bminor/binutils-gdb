# This shell script emits a C file. -*- C -*-
#   Copyright (C) 2025 Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the license, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3. If not,
# see <http://www.gnu.org/licenses/>.
#

# This file is sourced from elf.em, and defines i386 glibc specific
# routines.
#

source_em ${srcdir}/emultempl/elf-x86.em
source_em ${srcdir}/emultempl/elf-x86-glibc.em

# Define some shell vars to insert bits of code into the standard elf
# parse_args and list_options functions.
#

fragment <<EOF
static void
elf_i386_glibc_before_parse (void)
{
  elf_x86_before_parse ();
  elf_x86_glibc_before_parse ();
}
EOF

LDEMUL_BEFORE_PARSE=elf_i386_glibc_before_parse
