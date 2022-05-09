#!/bin/sh

# Copyright (C) 2022 Free Software Foundation, Inc.
#
# This file is part of GDB.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Used to generate .xml.in files, like so:
#
# ./update-linux.sh amd64-linux.xml.in
# ./update-linux.sh i386-linux.xml.in -m32


if [ $# -lt 1 ]; then
    echo "file argument needed"
    exit 1
fi

f="$1"
shift

if [ ! -f "$f" ]; then
    echo "cannot find $f"
    exit 1
fi

(
    python gen-header.py "<sys/syscall.h>"

    tmp=$(mktemp)

    echo '#include <sys/syscall.h>' \
	| gcc -E - -dD "$@" \
	| grep -E '#define __NR_' \
		> "$tmp"

    echo '<syscalls_info>'

    while read -r line; do
	name=$(echo "$line" | awk '{print $2}' | sed 's/^__NR_//')
	nr=$(echo "$line" | awk '{print $3}')
	echo "  <syscall name=\"$name\" number=\"$nr\"/>"
    done < "$tmp"

    echo '</syscalls_info>'

    rm -f "$tmp"
) > "$f"
