#! /bin/sh
# Wrapper around gcc to run 'dwz' when running the testsuite.

# Copyright (C) 2010-2012 Free Software Foundation, Inc.
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

# This program requires dwz in addition to gcc.
#
# Example usage:
#
# bash$ cd $objdir/gdb/testsuite
# bash$ runtest \
#   CC_FOR_TARGET="/bin/sh $srcdir/cc-with-dwz.sh gcc" \
#   CXX_FOR_TARGET="/bin/sh $srcdir/cc-with-dwz.sh g++"
#

myname=cc-with-dwz.sh

DWZ=${DWZ:-dwz}

have_link=unknown
next_is_output_file=no
output_file=a.out

for arg in "$@"
do
    if [ "$next_is_output_file" = "yes" ]
    then
	output_file="$arg"
	next_is_output_file=no
	continue
    fi

    # Poor man's gcc argument parser.
    # We don't need to handle all arguments, we just need to know if we're
    # doing a link and what the output file is.
    # It's not perfect, but it seems to work well enough for the task at hand.
    case "$arg" in
    "-c") have_link=no ;;
    "-E") have_link=no ;;
    "-S") have_link=no ;;
    "-o") next_is_output_file=yes ;;
    esac
done

if [ "$next_is_output_file" = "yes" ]
then
    echo "$myname: Unable to find output file" >&2
    exit 1
fi

if [ "$have_link" = "no" ]
then
    "$@"
    exit $?
fi

"$@"
rc=$?
[ $rc != 0 ] && exit $rc
if [ ! -f "$output_file" ]
then
    echo "$myname: Internal error: $output_file missing." >&2
    exit 1
fi

$DWZ "$output_file" > /dev/null 2>&1

exit 0
