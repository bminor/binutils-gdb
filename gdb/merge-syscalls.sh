#!/bin/sh

# Copyright 1998 Free Software Foundation, Inc.
#
# This file is part of GDB.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

################
# Usage: merge-syscalls syscall.tab [HEADER-FILE ...]
#
# This will scan every HEADER-FILE
# (default is /usr/include/sys/syscall.h)
# for #define SYS_xxx directives,
# recursively scanning #included files.
#
# Then it updates "syscall.tab", which is a file that has a
# C fragment to initialize a table of known syscall names.
# Any syscalls already in "syscall.tab" will be kept.
#
# In principle, you don't need to run this unless you're
# porting gdb to a new host that uses procfs.c.
#
# FIXME: perhaps use gcc -E -dM instead.
# FIXME: perhaps track which hosts have which syscalls.

DEST=$1; shift

WS=`printf "[\t ]*" 2>/dev/null || echo '[	 ]*'`


scan_all () {
    for file
    do
	scan_one $file
    done
}

scan_one () {
    echo >&2 "scanning $1..."
    <$1 sed -n -e "
	/^#${WS}define${WS}SYS_[_a-zA-Z0-9]${WS}/ {
	    s/^${WS}#${WS}define${WS}\(SYS_[_a-zA-Z0-9]*\)${WS}.*/\1/
	    p
	}
	/${WS}syscall_table${WS}\[SYS_[_a-zA-Z0-9]*\]/ {
	    s/^.*${WS}syscall_table${WS}\[\(SYS_[_a-zA-Z0-9]*\)\].*/\1/
	    p
	}
    " $1 &&

    includes=`
	<$1 sed -n -e "
	    /^${WS}#${WS}include${WS}</{
		s/^${WS}#${WS}include${WS}<\([^>]*\)>.*/\1/
		p
	    }
	" |
	while read inc; do
	    for dir in /usr/include; do
		if [ -f "$dir/$inc" ]; then
		    echo $dir/$inc
		fi
	    done
	done
    ` &&

    scan_all $includes
}

case $# in
0)
    set -- /usr/include/sys/syscall.h
    ;;
esac

scan_all $DEST "$@" |
sort -u |
(
    echo "/* This file is semi-automatically updated by `basename $0` */"
    while read sys; do
	tail=`echo "$sys" | sed 's/^SYS_//'`
	echo "#if defined ($sys)"
	echo "  syscall_table[$sys] = "\""$tail"\"";"
	echo "#endif"
    done
) > tmp$$ || exit 1

if cmp -s $DEST tmp$$; then
   echo "$DEST unchanged"
   rm -f tmp$$

else
    echo "creating new $DEST..."
    if [ -f $DEST ]; then
       mv $DEST $DEST.orig || exit 1
       echo " (original moved to $DEST.orig)"
    fi
    mv tmp$$ $DEST
fi


