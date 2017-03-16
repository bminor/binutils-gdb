/* Target-dependent code for linux-kernel target on S390.
   Copyright (C) 2017 Free Software Foundation, Inc.

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

#ifndef S390_LK_TDEP_H
#define S390_LK_TDEP_H

extern void s390_gdbarch_lk_init (struct gdbarch_info info,
				  struct gdbarch *gdbarch);

/* Core file privileged register sets, defined in s390-lk-tdep.c.  */
extern const struct regset s390x_gregset_lk;
extern const struct regset s390x_cr_regset;
extern const struct regset s390x_timer_regset;
extern const struct regset s390x_todcmp_regset;
extern const struct regset s390x_todpreg_regset;
extern const struct regset s390x_prefix_regset;

extern struct target_desc *tdesc_s390x_cr_linux64;
extern struct target_desc *tdesc_s390x_vxcr_linux64;

#endif /* S390_LK_TDEP_H */
