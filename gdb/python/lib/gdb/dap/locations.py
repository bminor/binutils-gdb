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

import gdb

# This is deprecated in 3.9, but required in older versions.
from typing import Optional

from .server import request
from .startup import in_gdb_thread, send_gdb_with_response


@in_gdb_thread
def _find_lines(filename, start_line, end_line):
    lines = set()
    for entry in gdb.execute_mi("-symbol-list-lines", filename)["lines"]:
        line = entry["line"]
        if line >= start_line and line <= end_line:
            lines.add(line)
    return {"breakpoints": [{"line": x} for x in sorted(lines)]}


@request("breakpointLocations")
def breakpoint_locations(*, source, line: int, endLine: Optional[int] = None, **extra):
    if endLine is None:
        endLine = line
    if "path" in source:
        filename = source["path"]
    elif "name" in source:
        filename = source["name"]
    else:
        raise Exception("")
    return send_gdb_with_response(lambda: _find_lines(filename, line, endLine))
