#!/bin/sh

# memory_test.sh -- test MEMORY regions.

# Copyright 2010 Free Software Foundation, Inc.
# Written by Nick Clifton  <nickc@redhat.com>

# This file is part of gold.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.


check()
{
    file=$1
    pattern=$2
    found=`grep "$pattern" $file`
    if test -z "$found"; then
        echo "pattern \"$pattern\" not found in file $file."
	echo $found
        exit 1
    fi
}

check memory_test.stdout \
  "  LOAD           0x001000 0x0*00000000 0x0*00000000 0x00000\?2 0x00000\?2 R   0x1000"
check memory_test.stdout \
  "  LOAD           0x00112c 0x0*00001000 0x0*0000012c 0x00000\?2 0x00000\?2 R   0x1000"
check memory_test.stdout \
  "  LOAD           0x002000 0x0*00005000 0x0*00005000 0x00000\?2 0x00000\?2 R   0x1000"
check memory_test.stdout \
  "  LOAD           0x00203c 0x0*00004000 0x0*0000603c 0x0000\?2a 0x0000\?2a R E 0x1000"


exit 0
