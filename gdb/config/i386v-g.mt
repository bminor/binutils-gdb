# Intel 386 running System V, using COFF encapsulation
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

#msg Use of the COFF encapsulation features requires the GNU binary utilities
#msg to be ahead of their System V counterparts in your path.
#msg

TDEPFILES= exec.o i386-tdep.o i386-pinsn.o
TM_FILE= tm-i386v-g.h
