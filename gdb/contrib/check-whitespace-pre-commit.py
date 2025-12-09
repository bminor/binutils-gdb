#! /usr/bin/env python3

# Copyright (C) 2025 Free Software Foundation, Inc.
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

import re
import subprocess
import sys

re_clean = re.compile(
    "(^(gdb/testsuite/|gdbsupport/|gdbserver/)|[.](m4|ac|def|[chly])$|NEWS)"
)
re_clean_exclude = re.compile("^(gdb/symfile.c|gdb/cli/cli-cmds.c)$")

clean = []
other = []
for f in sys.argv[1:]:
    m = re_clean.search(f)
    if m and re_clean_exclude.search(f):
        m = None
    if m:
        clean.append(f)
    else:
        other.append(f)

if other:
    cmd = ["git", "--no-pager", "diff", "--staged", "--check"] + other
    res = subprocess.run(cmd)
    if res.returncode != 0:
        sys.exit(res.returncode)

if clean:
    cmd = ["git", "hash-object", "-t", "tree", "/dev/null"]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.exit(res.returncode)
    null_tree = res.stdout.rstrip("\n")

    cmd = ["git", "diff-index", "--cached", "--check", null_tree] + clean
    res = subprocess.run(cmd)
    sys.exit(res.returncode)
