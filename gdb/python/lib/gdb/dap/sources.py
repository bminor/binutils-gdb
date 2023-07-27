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

from .server import request, capability
from .startup import send_gdb_with_response, in_gdb_thread


# The next available source reference ID.  Must be greater than 0.
_next_source = 1

# Map from full paths to Source dictionaries.
_source_map = {}

# Map from a source reference ID back to the same Source that is
# stored in _source_map.
_id_map = {}


@in_gdb_thread
def make_source(fullname, filename):
    """Return the Source for a given file name.

    FULLNAME is the full name.  This is used as the key.
    FILENAME is the base name.
    """
    global _source_map
    if fullname in _source_map:
        result = _source_map[fullname]
    else:
        global _next_source
        result = {
            "name": filename,
            "path": fullname,
            "sourceReference": _next_source,
        }
        _source_map[fullname] = result
        global _id_map
        _id_map[_next_source] = result
        _next_source += 1
    return result


@in_gdb_thread
def _sources():
    result = []
    for elt in gdb.execute_mi("-file-list-exec-source-files")["files"]:
        result.append(make_source(elt["fullname"], elt["file"]))
    return {
        "sources": result,
    }


@request("loadedSources")
@capability("supportsLoadedSourcesRequest")
def sources(**extra):
    return send_gdb_with_response(_sources)
