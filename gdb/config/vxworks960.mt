# VxWorks running on an Intel 960, as a target system
# Copyright (C) 1990-1991 Free Software Foundation, Inc.

# This file is part of GDB.

# GDB is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.

# GDB is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with GDB; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

TDEPFILES= exec.o i960-pinsn.o i960-tdep.o remote-vx.o xdr_ld.o xdr_ptrace.o xdr_rdb.o xdr_regs.o
TM_FILE= tm-vx960.h
# Define this for the vx-share routines, which don't see param.h.
TM_CFLAGS= -DI80960
