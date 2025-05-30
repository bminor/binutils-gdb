#!/usr/bin/env python3
# pylint: disable=invalid-name

# Copyright (C) 2024-2025 Free Software Foundation, Inc.
# Contributed by Timur Golubovich

# This file is part of GDB.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# To get help message for this script, run:
# ./gdb/syscalls/riscv-canonicalize-syscall-gen.py --help

# Execution result:

# usage: riscv-canonicalize-syscall-gen.py [-h] -i INPUT
#
# Generate file gdb/riscv-canonicalize-syscall-gen.c from path to riscv linux syscalls.
#
# options:
#   -h, --help            show this help message and exit
#   -i INPUT, --input INPUT
#                         path to riscv linux syscalls (glibc/sysdeps/unix/sysv/linux/riscv/rv64/arch-syscall.h)

import argparse
import re
import sys
from pathlib import Path as _Path

head = """\
/* DO NOT EDIT: Autogenerated by riscv-canonicalize-syscall-gen.py

   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "riscv-linux-tdep.h"

/* riscv64_canonicalize_syscall maps from the native riscv 64 Linux set
   of syscall ids into a canonical set of syscall ids used by
   process record.  */

enum gdb_syscall
riscv64_canonicalize_syscall (int syscall)
{
  switch (syscall)
    {
"""

tail = """\
    default:
      return gdb_sys_no_syscall;
    }
}
"""


class Generator:
    def _get_gdb_syscalls(self, gdb_syscalls_path: _Path) -> list[str]:
        gdb_syscalls: list[str] = []
        with open(gdb_syscalls_path, "r", encoding="UTF-8") as file:
            lines = file.readlines()
            for line in lines:
                match = re.search(r"\s*(?P<name>gdb_sys_[^S]+)\S*=", line)
                if match:
                    gdb_syscalls.append(match.group("name").strip())
        return gdb_syscalls

    def _get_canon_syscalls_lines(
        self, syscalls_path: _Path, gdb_syscalls: list[str]
    ) -> list[str]:
        canon_syscalls: dict[int, str] = {}
        with open(syscalls_path, "r", encoding="UTF-8") as file:
            lines = file.readlines()
            for line in lines:
                match = re.match(
                    r"#define\s+__NR_(?P<name>[^\s]+)\s+(?P<number>\d+)", line
                )
                if match:
                    syscall_name = match.group("name")
                    syscall_num = int(match.group("number"))
                    gdb_syscall_name = f"gdb_sys_{syscall_name}"
                    if gdb_syscall_name in gdb_syscalls:
                        value = f"    case {syscall_num}: return {gdb_syscall_name};\n"
                        canon_syscalls[syscall_num] = value
                    # this is a place for corner cases
                    elif syscall_name == "mmap":
                        gdb_old_syscall_name = "gdb_sys_old_mmap"
                        value = (
                            f"    case {syscall_num}: return {gdb_old_syscall_name};\n"
                        )
                        canon_syscalls[syscall_num] = value
                    else:
                        value = f"    /* case {syscall_num}: return {gdb_syscall_name};  */\n"
                        canon_syscalls[syscall_num] = value
        return [canon_syscalls[syscall_num] for syscall_num in sorted(canon_syscalls)]

    def generate(self, syscalls_path: _Path) -> None:
        repo_path = _Path(__file__).parent.parent.parent
        gdb_syscalls_path = repo_path / "gdb" / "linux-record.h"
        canon_syscalls_path = repo_path / "gdb" / "riscv-canonicalize-syscall-gen.c"

        gdb_syscalls = self._get_gdb_syscalls(gdb_syscalls_path)
        canon_syscalls_lines = self._get_canon_syscalls_lines(
            syscalls_path, gdb_syscalls
        )

        with open(canon_syscalls_path, "w", encoding="UTF-8") as file:
            file.writelines(head)
            file.writelines(canon_syscalls_lines)
            file.writelines(tail)


help_message = """\
Generate file gdb/riscv-canonicalize-syscall-gen.c
from path to riscv linux syscalls.
"""


def setup_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=help_message)
    parser.add_argument(
        "-i",
        "--input",
        type=_Path,
        required=True,
        help="path to riscv linux syscalls (glibc/sysdeps/unix/sysv/linux/riscv/rv64/arch-syscall.h)",
    )
    return parser


def main(argv: list[str]) -> int:
    try:
        parser = setup_parser()
        args = parser.parse_args(argv)
        generator = Generator()
        generator.generate(args.input)
        return 0
    except RuntimeError as e:
        print(str(e))
        return -1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
