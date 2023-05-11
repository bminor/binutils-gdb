# Copyright 2023 Free Software Foundation, Inc.

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

import base64
import gdb

from .server import request, capability
from .startup import send_gdb_with_response, in_gdb_thread


@in_gdb_thread
def _read_memory(addr, count):
    buf = gdb.selected_inferior().read_memory(addr, count)
    return base64.b64encode(buf).decode("ASCII")


@request("readMemory")
@capability("supportsReadMemoryRequest")
def read_memory(*, memoryReference: str, offset: int = 0, count: int, **extra):
    addr = int(memoryReference, 0) + offset
    buf = send_gdb_with_response(lambda: _read_memory(addr, count))
    return {
        "address": hex(addr),
        "data": buf,
    }


@in_gdb_thread
def _write_memory(addr, contents):
    buf = base64.b64decode(contents)
    gdb.selected_inferior().write_memory(addr, buf)


@request("writeMemory")
@capability("supportsWriteMemoryRequest")
def write_memory(*, memoryReference: str, offset: int = 0, data: str, **extra):
    addr = int(memoryReference, 0) + offset
    send_gdb_with_response(lambda: _write_memory(addr, data))
    return {}
