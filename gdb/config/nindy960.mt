# Intel 80960, in an embedded system under the NINDY monitor
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

TDEPFILES= exec.o i960-pinsn.o i960-tdep.o nindy-tdep.o remote-nindy.o nindy.o Onindy.o ttybreak.o ttyflush.o
TM_FILE= tm-nindy960.h
TM_CFLAGS=
