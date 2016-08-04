#!/bin/sh

# split_aarch64.sh -- test -fstack-split for AArch64

# Copyright (C) 2009-2017 Free Software Foundation, Inc.

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

match()
{
  if ! egrep "$1" "$2" >/dev/null 2>&1; then
    echo 1>&2 "could not find '$1' in $2"
    exit 1
  fi
}

nomatch()
{
  if egrep "$1" "$2" >/dev/null 2>&1; then
    echo 1>&2 "found unexpected '$1' in $2"
    exit 1
  fi
}

match 'mov.*+x10,.#0x0( |$)' split_aarch64_1.stdout
match 'mov.*+x10,.#0x0,.lsl.#16( |$)' split_aarch64_1.stdout
match 'b.*__morestack>?$' split_aarch64_1.stdout

match 'mov.*+x10,.#0x4000( |$)' split_aarch64_2.stdout
match 'mov.*+x10,.#0x0,.lsl.#16( |$)' split_aarch64_2.stdout
match 'b.*__morestack_non_split>?$' split_aarch64_2.stdout
match 'mov.*+x10,.#0x4210( |$)' split_aarch64_2.stdout
match 'mov.*+x10,.#0x0,.lsl.#16( |$)' split_aarch64_2.stdout
match 'b.*__morestack_non_split>?$' split_aarch64_2.stdout
match 'mov.*+x10,.#0x4110( |$)' split_aarch64_2.stdout
match 'mov.*+x10,.#0x1,.lsl.#16( |$)' split_aarch64_2.stdout
match 'b.*__morestack_non_split>?$' split_aarch64_2.stdout

match 'failed to match' split_aarch64_3.stdout

match 'mov.*+x10,.#0x0( |$)' split_aarch64_4.stdout
match 'mov.*+x10,.#0x0,.lsl.#16( |$)' split_aarch64_4.stdout
match 'b.*__morestack_non_split>?$' split_aarch64_2.stdout

match 'cannot mix' split_aarch64_r.stdout
