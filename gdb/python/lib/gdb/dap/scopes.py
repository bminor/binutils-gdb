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

from .frames import frame_for_id
from .startup import send_gdb_with_response, in_gdb_thread
from .server import request


# Helper function to return a frame's block without error.
@in_gdb_thread
def _safe_block(frame):
    try:
        return frame.block()
    except gdb.error:
        return None


# Helper function to return a list of variables of block, up to the
# enclosing function.
@in_gdb_thread
def _block_vars(block):
    result = []
    while True:
        result += list(block)
        if block.function is not None:
            break
        block = block.superblock
    return result


# Helper function to create a DAP scopes for a given frame ID.
@in_gdb_thread
def _get_scope(id):
    frame = frame_for_id(id)
    block = _safe_block(frame)
    scopes = []
    if block is not None:
        new_scope = {
            # FIXME
            "name": "Locals",
            "expensive": False,
            "namedVariables": len(_block_vars(block)),
        }
        scopes.append(new_scope)
    return scopes


@request("scopes")
def scopes(*, frameId, **extra):
    scopes = send_gdb_with_response(lambda: _get_scope(frameId))
    return {"scopes": scopes}
