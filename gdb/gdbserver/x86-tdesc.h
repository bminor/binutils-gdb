/* Copyright (C) 2018 Free Software Foundation, Inc.

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

#ifndef X86_TDESC_H

/* The "expedite" registers for x86 targets.  */
static const char *i386_expedite_regs[] = {"ebp", "esp", "eip", NULL};

/* The "expedite" registers for x86_64 targets.  */
static const char *amd64_expedite_regs[] = {"rbp", "rsp", "rip", NULL};

#endif /* X86_TDESC_H */
