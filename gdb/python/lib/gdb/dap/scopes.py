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
from .varref import BaseReference


# Helper function to return a frame's block without error.
@in_gdb_thread
def _safe_block(frame):
    try:
        return frame.block()
    except gdb.error:
        return None


# Helper function to return two lists of variables of block, up to the
# enclosing function.  The result is of the form (ARGS, LOCALS), where
# each element is itself a list.
@in_gdb_thread
def _block_vars(block):
    args = []
    locs = []
    while True:
        for var in block:
            if var.is_argument:
                args.append(var)
            elif var.is_variable or var.is_constant:
                locs.append(var)
        if block.function is not None:
            break
        block = block.superblock
    return (args, locs)


class ScopeReference(BaseReference):
    def __init__(self, name, hint, frame, var_list):
        super().__init__(name)
        self.hint = hint
        self.frame = frame
        self.func = frame.function()
        self.var_list = var_list

    def to_object(self):
        result = super().to_object()
        result["presentationHint"] = self.hint
        # How would we know?
        result["expensive"] = False
        result["namedVariables"] = len(self.var_list)
        if self.func is not None:
            result["line"] = self.func.line
            # FIXME construct a Source object
        return result

    def child_count(self):
        return len(self.var_list)

    @in_gdb_thread
    def fetch_one_child(self, idx):
        sym = self.var_list[idx]
        if sym.needs_frame:
            val = sym.value(self.frame)
        else:
            val = sym.value()
        return (sym.print_name, val)


class RegisterReference(ScopeReference):
    def __init__(self, name, frame):
        super().__init__(
            name, "registers", frame, list(frame.architecture().registers())
        )

    @in_gdb_thread
    def fetch_one_child(self, idx):
        return (self.var_list[idx].name, self.frame.read_register(self.var_list[idx]))


# Helper function to create a DAP scopes for a given frame ID.
@in_gdb_thread
def _get_scope(id):
    frame = frame_for_id(id)
    block = _safe_block(frame)
    scopes = []
    if block is not None:
        (args, locs) = _block_vars(block)
        if args:
            scopes.append(ScopeReference("Arguments", "arguments", frame, args))
        if locs:
            scopes.append(ScopeReference("Locals", "locals", frame, locs))
    scopes.append(RegisterReference("Registers", frame))
    return [x.to_object() for x in scopes]


@request("scopes")
def scopes(*, frameId, **extra):
    scopes = send_gdb_with_response(lambda: _get_scope(frameId))
    return {"scopes": scopes}
