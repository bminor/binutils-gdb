/* Common target dependent code for GNU/Linux on PPC systems.

   Copyright (C) 2018 Free Software Foundation, Inc.

   This file is part of GDB.

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

#ifndef ARCH_PPC_LINUX_COMMON_H
#define ARCH_PPC_LINUX_COMMON_H

struct target_desc;

/* Check if the hwcap auxv entry indicates that isa205 is supported.  */
bool ppc_linux_has_isa205 (unsigned long hwcap);

/* Features used to determine the target description.  */
struct ppc_linux_features
{
  unsigned int wordsize;
  bool altivec;
  bool vsx;
  bool isa205;
  bool cell;
};

/* Base value for ppc_linux_features variables.  */
const struct ppc_linux_features ppc_linux_no_features = {
  0,
  false,
  false,
  false,
  false,
};

/* Return a target description that matches FEATURES.  */
const struct target_desc * ppc_linux_match_description
(struct ppc_linux_features features);

#endif /* ARCH_PPC_LINUX_COMMON_H */
