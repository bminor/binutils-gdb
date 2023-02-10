# Copyright 2022, 2023 Free Software Foundation, Inc.

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
import os

from .server import request, capability
from .startup import send_gdb_with_response, in_gdb_thread


# Map from the breakpoint "kind" (like "function") to a second map, of
# breakpoints of that type.  The second map uses the breakpoint spec
# as a key, and the gdb.Breakpoint itself as a value.  This is used to
# implement the clearing behavior specified by the protocol, while
# allowing for reuse when a breakpoint can be kept.
breakpoint_map = {}


@in_gdb_thread
def breakpoint_descriptor(bp):
    "Return the Breakpoint object descriptor given a gdb Breakpoint."
    if bp.locations:
        # Just choose the first location, because DAP doesn't allow
        # multiple locations.  See
        # https://github.com/microsoft/debug-adapter-protocol/issues/13
        loc = bp.locations[0]
        (basename, line) = loc.source
        return {
            "id": bp.number,
            "verified": True,
            "source": {
                "name": os.path.basename(basename),
                "path": loc.fullname,
                # We probably don't need this but it doesn't hurt to
                # be explicit.
                "sourceReference": 0,
            },
            "line": line,
            "instructionReference": hex(loc.address),
        }
    else:
        return {
            "id": bp.number,
            "verified": False,
        }


# Helper function to set some breakpoints according to a list of
# specifications.
@in_gdb_thread
def _set_breakpoints(kind, specs):
    global breakpoint_map
    # Try to reuse existing breakpoints if possible.
    if kind in breakpoint_map:
        saved_map = breakpoint_map[kind]
    else:
        saved_map = {}
    breakpoint_map[kind] = {}
    result = []
    for spec in specs:
        keyspec = frozenset(spec.items())
        if keyspec in saved_map:
            bp = saved_map.pop(keyspec)
        else:
            # FIXME handle exceptions here
            bp = gdb.Breakpoint(**spec)
        breakpoint_map[kind][keyspec] = bp
        result.append(breakpoint_descriptor(bp))
    # Delete any breakpoints that were not reused.
    for entry in saved_map.values():
        entry.delete()
    return result


@request("setBreakpoints")
def set_breakpoint(*, source, breakpoints=[], **args):
    if "path" not in source:
        result = []
    else:
        specs = []
        for obj in breakpoints:
            specs.append(
                {
                    "source": source["path"],
                    "line": obj["line"],
                }
            )
        # Be sure to include the path in the key, so that we only
        # clear out breakpoints coming from this same source.
        key = "source:" + source["path"]
        result = send_gdb_with_response(lambda: _set_breakpoints(key, specs))
    return {
        "breakpoints": result,
    }


@request("setFunctionBreakpoints")
@capability("supportsFunctionBreakpoints")
def set_fn_breakpoint(*, breakpoints, **args):
    specs = []
    for bp in breakpoints:
        specs.append(
            {
                "function": bp["name"],
            }
        )
    result = send_gdb_with_response(lambda: _set_breakpoints("function", specs))
    return {
        "breakpoints": result,
    }


@request("setInstructionBreakpoints")
@capability("supportsInstructionBreakpoints")
def set_insn_breakpoints(*, breakpoints, offset=None, **args):
    specs = []
    for bp in breakpoints:
        # There's no way to set an explicit address breakpoint
        # from Python, so we rely on "spec" instead.
        val = "*" + bp["instructionReference"]
        if offset is not None:
            val = val + " + " + str(offset)
        specs.append(
            {
                "spec": val,
            }
        )
    result = send_gdb_with_response(lambda: _set_breakpoints("instruction", specs))
    return {
        "breakpoints": result,
    }
