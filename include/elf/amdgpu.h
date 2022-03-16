/* AMDGPU ELF support for BFD.

   Copyright (C) 2019-2021 Free Software Foundation, Inc.

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _ELF_AMDGPU_H
#define _ELF_AMDGPU_H

/* e_ident[EI_ABIVERSION] values, when e_ident[EI_OSABI] is
   ELFOSABI_AMDGPU_HSA.  */

#define ELFABIVERSION_AMDGPU_HSA_V2 0
#define ELFABIVERSION_AMDGPU_HSA_V3 1
#define ELFABIVERSION_AMDGPU_HSA_V4 2
#define ELFABIVERSION_AMDGPU_HSA_V5 3

/* Processor selection mask for EF_AMDGPU_MACH_* values.  */

#define EF_AMDGPU_MACH 0x0ff
#define EF_AMDGPU_MACH_AMDGCN_MIN 0x020

#endif /* _ELF_AMDGPU_H */
