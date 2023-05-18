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

# These are deprecated in 3.9, but required in older versions.
from typing import Optional, Sequence

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
        result = {
            "id": bp.number,
            "verified": True,
            "source": {
                "name": os.path.basename(basename),
                # We probably don't need this but it doesn't hurt to
                # be explicit.
                "sourceReference": 0,
            },
            "line": line,
            "instructionReference": hex(loc.address),
        }
        path = loc.fullname
        if path is not None:
            result["source"]["path"] = path
        return result
    else:
        return {
            "id": bp.number,
            "verified": False,
        }


# Extract entries from a hash table and return a list of them.  Each
# entry is a string.  If a key of that name appears in the hash table,
# it is removed and pushed on the result list; if it does not appear,
# None is pushed on the list.
def _remove_entries(table, *names):
    return [table.pop(name, None) for name in names]


# Helper function to set some breakpoints according to a list of
# specifications and a callback function to do the work of creating
# the breakpoint.
@in_gdb_thread
def _set_breakpoints_callback(kind, specs, creator):
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

        (condition, hit_condition) = _remove_entries(spec, "condition", "hitCondition")

        if keyspec in saved_map:
            bp = saved_map.pop(keyspec)
        else:
            # FIXME handle exceptions here
            bp = creator(**spec)

        if condition is not None:
            bp.condition = condition
        if hit_condition is not None:
            bp.ignore_count = hit_condition

        breakpoint_map[kind][keyspec] = bp
        result.append(breakpoint_descriptor(bp))
    # Delete any breakpoints that were not reused.
    for entry in saved_map.values():
        entry.delete()
    return result


# Helper function to set odinary breakpoints according to a list of
# specifications.
@in_gdb_thread
def _set_breakpoints(kind, specs):
    return _set_breakpoints_callback(kind, specs, gdb.Breakpoint)


# Turn a DAP SourceBreakpoint, FunctionBreakpoint, or
# InstructionBreakpoint into a "spec" that is used by
# _set_breakpoints.  SPEC is a dictionary of parameters that is used
# as the base of the result; it is modified in place.
def _basic_spec(bp_info, spec):
    for name in ("condition", "hitCondition"):
        if name in bp_info:
            spec[name] = bp_info[name]
    return spec


# FIXME we do not specify a type for 'source'.
# FIXME 'breakpoints' is really a list[SourceBreakpoint].
@request("setBreakpoints")
@capability("supportsHitConditionalBreakpoints")
@capability("supportsConditionalBreakpoints")
def set_breakpoint(*, source, breakpoints: Sequence = (), **args):
    if "path" not in source:
        result = []
    else:
        specs = []
        for obj in breakpoints:
            specs.append(
                _basic_spec(
                    obj,
                    {
                        "source": source["path"],
                        "line": obj["line"],
                    },
                )
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
def set_fn_breakpoint(*, breakpoints: Sequence, **args):
    specs = []
    for bp in breakpoints:
        specs.append(
            _basic_spec(
                bp,
                {
                    "function": bp["name"],
                },
            )
        )
    result = send_gdb_with_response(lambda: _set_breakpoints("function", specs))
    return {
        "breakpoints": result,
    }


@request("setInstructionBreakpoints")
@capability("supportsInstructionBreakpoints")
def set_insn_breakpoints(
    *, breakpoints: Sequence, offset: Optional[int] = None, **args
):
    specs = []
    for bp in breakpoints:
        # There's no way to set an explicit address breakpoint
        # from Python, so we rely on "spec" instead.
        val = "*" + bp["instructionReference"]
        if offset is not None:
            val = val + " + " + str(offset)
        specs.append(
            _basic_spec(
                bp,
                {
                    "spec": val,
                },
            )
        )
    result = send_gdb_with_response(lambda: _set_breakpoints("instruction", specs))
    return {
        "breakpoints": result,
    }


@in_gdb_thread
def _catch_exception(filterId, **args):
    if filterId == "assert":
        cmd = "-catch-assert"
    elif filterId == "exception":
        cmd = "-catch-exception"
    else:
        raise Exception(f"Invalid exception filterID: {filterId}")
    result = gdb.execute_mi(cmd)
    # A little lame that there's no more direct way.
    for bp in gdb.breakpoints():
        if bp.number == result["bkptno"]:
            return bp
    raise Exception("Could not find catchpoint after creating")


@in_gdb_thread
def _set_exception_catchpoints(filter_options):
    return _set_breakpoints_callback("exception", filter_options, _catch_exception)


@request("setExceptionBreakpoints")
@capability("supportsExceptionFilterOptions")
@capability(
    "exceptionBreakpointFilters",
    (
        {
            "filter": "assert",
            "label": "Ada assertions",
            "supportsCondition": True,
        },
        {
            "filter": "exception",
            "label": "Ada exceptions",
            "supportsCondition": True,
        },
    ),
)
def set_exception_breakpoints(
    *, filters: Sequence[str], filterOptions: Sequence = (), **args
):
    # Convert the 'filters' to the filter-options style.
    options = [{"filterId": filter} for filter in filters]
    options.extend(filterOptions)
    result = send_gdb_with_response(lambda: _set_exception_catchpoints(options))
    return {
        "breakpoints": result,
    }
