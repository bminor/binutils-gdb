/* Native-dependent code for SPARC BSD's.

   Copyright 2003 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef SPARCBSD_NAT_H
#define SPARCBSD_NAT_H

/* Functions translating between `struct reg' and `struct fpreg' and
   GDB's register cache.  */
extern void (*sparcbsd_supply_reg)(const char *, int);
extern void (*sparcbsd_fill_reg)(char *, int);
extern void (*sparcbsd_supply_fpreg)(const char *, int);
extern void (*sparcbsd_fill_fpreg)(char *, int);

/* Functions indication whether `struct reg' or `struct fpreg' provides
   a certain register.  */
extern int (*sparcbsd_reg_supplies_p)(int);
extern int (*sparcbsd_fpreg_supplies_p)(int);

#endif /* sparcbsd-nat.h */
