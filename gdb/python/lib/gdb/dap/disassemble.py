# Copyright 2022-2023 Free Software Foundation, Inc.

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

import gdb

from .server import request, capability
from .startup import in_gdb_thread


@in_gdb_thread
def _disassemble(pc, skip_insns, count):
    inf = gdb.selected_inferior()
    try:
        arch = gdb.selected_frame().architecture()
    except gdb.error:
        # Maybe there was no frame.
        arch = inf.architecture()
    result = []
    total_count = skip_insns + count
    for elt in arch.disassemble(pc, count=total_count)[skip_insns:]:
        mem = inf.read_memory(elt["addr"], elt["length"])
        result.append(
            {
                "address": hex(elt["addr"]),
                "instruction": elt["asm"],
                "instructionBytes": mem.hex(),
            }
        )
    return {
        "instructions": result,
    }


@request("disassemble")
@capability("supportsDisassembleRequest")
def disassemble(
    *,
    memoryReference: str,
    offset: int = 0,
    instructionOffset: int = 0,
    instructionCount: int,
    **extra
):
    pc = int(memoryReference, 0) + offset
    return _disassemble(pc, instructionOffset, instructionCount)
