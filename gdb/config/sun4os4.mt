# Sun 4 or Sparcstation, running SunOS 4
# Copyright (C) 1990 Free Software Foundation, Inc.

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

# The system-supplied assembler re-orders the symbols so that gdb
# can't find "gcc_compiled.".
#msg If you compile your program with GCC, use the GNU assembler.
#msg

TDEPFILES= exec.o sparc-tdep.o sparc-pinsn.o solib.o
TM_FILE= tm-sun4os4.h
